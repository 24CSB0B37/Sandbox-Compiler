#include "gcc-plugin.h"
#include "plugin-version.h"

#include "context.h"
#include "tree.h"
#include "basic-block.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-pass.h"

#include "detection_engine.h"
#include "policy_engine.h"
#include "enforcement.h"
#include "logger.h"

static unsigned int sandbox_execute(function *fun)
{
    initialize_policy_engine();
    logger_init();

    basic_block bb;

    FOR_EACH_BB_FN(bb, fun)
    {
        for (gimple_stmt_iterator gsi = gsi_start_bb(bb);
             !gsi_end_p(gsi);
             gsi_next(&gsi))
        {
            gimple *stmt = gsi_stmt(gsi);
            analyze_statement(stmt, &gsi);
        }
    }

    evaluate_policies();
    enforce_policies();

    logger_close();
    return 0;
}

namespace {

const pass_data sandbox_pass_data = {
    GIMPLE_PASS,
    "sandbox_pass",
    OPTGROUP_NONE,
    TV_NONE,
    PROP_gimple_any,
    0,
    0,
    0,
    0
};

struct sandbox_pass : gimple_opt_pass {
    sandbox_pass(gcc::context *ctxt)
        : gimple_opt_pass(sandbox_pass_data, ctxt) {}

    unsigned int execute(function *fun) override {
        return sandbox_execute(fun);
    }
};

}

gimple_opt_pass *make_sandbox_pass(gcc::context *ctxt)
{
    return new sandbox_pass(ctxt);
}
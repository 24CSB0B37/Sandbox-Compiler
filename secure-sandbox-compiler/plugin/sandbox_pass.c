#include "gcc-plugin.h"
#include "tree-pass.h"
#include "context.h"
#include "basic-block.h"
#include "gimple.h"

#include "detection_engine.h"
#include "policy_engine.h"
#include "enforcement.h"
#include "logger.h"

static unsigned int sandbox_execute(void)
{
    initialize_policy_engine();
    logger_init();

    basic_block bb;
    FOR_EACH_BB_FN(bb, cfun)
    {
        for (gimple_stmt_iterator gsi = gsi_start_bb(bb);
             !gsi_end_p(gsi);
             gsi_next(&gsi))
        {
            analyze_statement(gsi_stmt(gsi), &gsi);
        }
    }

    evaluate_policies();
    enforce_policies();

    logger_close();
    return 0;
}

struct gimple_opt_pass sandbox_pass = {
    {
        GIMPLE_PASS,
        "sandbox_pass",
        NULL,
        sandbox_execute,
        NULL,
        NULL,
        0,
        TV_NONE,
        0,
        0,
        0,
        0
    }
};

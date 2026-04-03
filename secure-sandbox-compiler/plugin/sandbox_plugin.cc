#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree-pass.h"
#include "context.h"
#include "coretypes.h"

int plugin_is_GPL_compatible;

extern gcc::context *g;
extern gimple_opt_pass *make_sandbox_pass(gcc::context *ctxt);

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version)
{
    if (!plugin_default_version_check(version, &gcc_version))
    {
        printf("[Sandbox] Version mismatch — plugin not loaded.\n");
        return 1;
    }

    struct register_pass_info pass_info;
    pass_info.pass                  = make_sandbox_pass(g);
    pass_info.reference_pass_name   = "cfg";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op                = PASS_POS_INSERT_AFTER;

    register_callback(plugin_info->base_name,
                      PLUGIN_PASS_MANAGER_SETUP,
                      NULL,
                      &pass_info);

    printf("[Sandbox] Plugin loaded successfully.\n");
    return 0;
}
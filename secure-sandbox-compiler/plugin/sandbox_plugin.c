#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree-pass.h"

int plugin_is_GPL_compatible;

extern struct gimple_opt_pass sandbox_pass;

int plugin_init(struct plugin_name_args *plugin_info,
                struct plugin_gcc_version *version)
{
    register_callback(plugin_info->base_name,
                      PLUGIN_PASS_MANAGER_SETUP,
                      NULL,
                      &sandbox_pass);

    return 0;
}

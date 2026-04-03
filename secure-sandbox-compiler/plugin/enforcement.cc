#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "policy_engine.h"
#include "enforcement.h"

void enforce_policies()
{
    if (get_violation_count() == 0)
    {
        printf("[Sandbox] No violations found. Clean.\n");
        return;
    }

    const char *enforcement = get_highest_enforcement();

    if (!strcmp(enforcement, "BLOCK"))
    {
        remove("out");
        printf("[Sandbox] Unsafe binary deleted.\n");
        if (get_fix_applied())
            printf("[Sandbox] Fixed binary ready: ./out_fixed\n");
        else
            printf("[Sandbox] No fix available — fix manually.\n");
        exit(1);
    }
    else if (!strcmp(enforcement, "WARN"))
        printf("[Sandbox] Violations found — warnings issued.\n");
    else
        printf("[Sandbox] Violations logged only.\n");
}
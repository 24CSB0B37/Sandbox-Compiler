#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "policy_engine.h"

void enforce_policies()
{
    if (get_violation_count() == 0)
        return;

    const char *severity = get_highest_severity();

    if (!strcmp(severity, "CRITICAL"))
    {
        printf("[Sandbox] Compilation aborted.\n");
        exit(1);
    }
    else if (!strcmp(severity, "HIGH"))
    {
        printf("[Sandbox] Warning: HIGH severity issues.\n");
    }
}

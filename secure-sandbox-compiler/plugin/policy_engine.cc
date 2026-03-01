#include <stdio.h>
#include <string.h>

static int violation_count = 0;
static const char *highest = "LOW";

void initialize_policy_engine()
{
    violation_count = 0;
    highest = "LOW";
}

void record_violation(const char *type, const char *severity)
{
    violation_count++;
    printf("[Sandbox] Violation: %s (%s)\n", type, severity);

    if (!strcmp(severity, "CRITICAL"))
        highest = "CRITICAL";
    else if (!strcmp(severity, "HIGH") &&
             strcmp(highest, "CRITICAL"))
        highest = "HIGH";
}

void evaluate_policies()
{
    printf("[Sandbox] Total Violations: %d\n", violation_count);
}

int get_violation_count()
{
    return violation_count;
}

const char* get_highest_severity()
{
    return highest;
}

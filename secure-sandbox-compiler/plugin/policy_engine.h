#ifndef POLICY_ENGINE_H
#define POLICY_ENGINE_H

void initialize_policy_engine();
void record_violation(const char *type, const char *severity);
void evaluate_policies();
int get_violation_count();
const char* get_highest_severity();

#endif

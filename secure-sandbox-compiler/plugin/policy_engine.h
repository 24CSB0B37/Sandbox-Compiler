#ifndef POLICY_ENGINE_H
#define POLICY_ENGINE_H

void initialize_policy_engine();
void load_policy(const char *path);
void record_violation(const char *type, const char *severity,
                      const char *message, const char *file, int line);
void evaluate_policies();
int  get_violation_count();
const char *get_highest_severity();
int get_fix_applied();
const char *get_highest_enforcement();


int  is_fixable(const char *type);
const char *get_fix(const char *type);
void apply_fix(const char *type, const char *file, int line);

#endif
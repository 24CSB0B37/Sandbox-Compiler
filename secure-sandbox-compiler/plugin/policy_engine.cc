#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "policy_engine.h"
#include "logger.h"

#define MAX_RULES 32

struct policy_rule
{
    char name[64];
    char severity[16];
    char message[256];
    char enforcement[16];
    char fix[256];
    int  fixable;
    int  enabled;
};

static policy_rule rules[MAX_RULES];
static int  rule_count          = 0;
static int  violation_count     = 0;
static int  fix_applied         = 0;
static char highest[16]         = "LOW";
static char highest_enforcement[16] = "LOG";

void initialize_policy_engine()
{
    violation_count = 0;
    fix_applied     = 0;
    strncpy(highest,             "LOW", sizeof(highest));
    strncpy(highest_enforcement, "LOG", sizeof(highest_enforcement));
}

void load_policy(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        printf("[Sandbox] Could not load policy: %s\n", path);
        return;
    }

    char line[512];
    policy_rule current;
    memset(&current, 0, sizeof(current));
    current.enabled = 1;
    int in_rule = 0;

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "RULE:", 5) == 0)
        {
            if (in_rule && rule_count < MAX_RULES)
                rules[rule_count++] = current;
            memset(&current, 0, sizeof(current));
            current.enabled = 1;
            strncpy(current.name, line + 6, sizeof(current.name));
            in_rule = 1;
        }
        else if (strncmp(line, "SEVERITY:", 9) == 0)
            strncpy(current.severity,    line + 10, sizeof(current.severity));
        else if (strncmp(line, "ISSUE:", 6) == 0)
            strncpy(current.message,     line + 7,  sizeof(current.message));
        else if (strncmp(line, "ENFORCEMENT:", 12) == 0)
            strncpy(current.enforcement, line + 13, sizeof(current.enforcement));
        else if (strncmp(line, "FIX:", 4) == 0)
            strncpy(current.fix,         line + 5,  sizeof(current.fix));
        else if (strncmp(line, "FIXABLE:", 8) == 0)
            current.fixable = (strncmp(line + 9, "TRUE", 4) == 0) ? 1 : 0;
        else if (strncmp(line, "ENABLED:", 8) == 0)
            current.enabled = (strncmp(line + 9, "TRUE", 4) == 0) ? 1 : 0;
        else if (strncmp(line, "END", 3) == 0)
        {
            if (in_rule && rule_count < MAX_RULES)
                rules[rule_count++] = current;
            break;
        }
    }

    fclose(f);
    printf("[Sandbox] Loaded %d rules from %s\n", rule_count, path);
}

static policy_rule *find_rule(const char *name)
{
    for (int i = 0; i < rule_count; i++)
        if (strstr(rules[i].name, name) || strstr(name, rules[i].name))
            return &rules[i];
    return NULL;
}

void record_violation(const char *type, const char *default_severity,
                      const char *default_message, const char *file, int line)
{
    policy_rule *rule = find_rule(type);

    const char *severity    = default_severity;
    const char *message     = default_message;
    const char *enforcement = "WARN";

    if (rule)
    {
        if (!rule->enabled) return;
        if (strlen(rule->severity)    > 0) severity    = rule->severity;
        if (strlen(rule->message)     > 0) message     = rule->message;
        if (strlen(rule->enforcement) > 0) enforcement = rule->enforcement;
    }

    violation_count++;

    logger_log(severity, type, message, file, line);

    printf("[Sandbox] [%s] [%s] %s\n", severity, type, message);
    printf("          File: %s  Line: %d\n", file ? file : "unknown", line);
    printf("          Enforcement: %s\n", enforcement);

    if (rule && rule->fixable)
    {
        printf("          [FIXABLE] %s\n", rule->fix);
        apply_fix(type, file, line);
    }
    else if (rule && !rule->fixable)
    {
        printf("          [NO AUTO-FIX] %s\n", rule->fix);
    }

    printf("\n");

    // update highest severity
    if (!strcmp(severity, "CRITICAL"))
        strncpy(highest, "CRITICAL", sizeof(highest));
    else if (!strcmp(severity, "HIGH") && strcmp(highest, "CRITICAL"))
        strncpy(highest, "HIGH", sizeof(highest));
    else if (!strcmp(severity, "MEDIUM") &&
             strcmp(highest, "CRITICAL") && strcmp(highest, "HIGH"))
        strncpy(highest, "MEDIUM", sizeof(highest));

    // update highest enforcement
    if (!strcmp(enforcement, "BLOCK"))
        strncpy(highest_enforcement, "BLOCK", sizeof(highest_enforcement));
    else if (!strcmp(enforcement, "WARN") &&
             strcmp(highest_enforcement, "BLOCK"))
        strncpy(highest_enforcement, "WARN", sizeof(highest_enforcement));
}

int is_fixable(const char *type)
{
    policy_rule *rule = find_rule(type);
    return rule ? rule->fixable : 0;
}

const char *get_fix(const char *type)
{
    policy_rule *rule = find_rule(type);
    return (rule && rule->fixable) ? rule->fix : "No fix available";
}

void apply_fix(const char *type, const char *file, int line)
{
    if (!file || strlen(file) == 0) return;

    FILE *f = fopen(file, "r");
    if (!f) return;

    char lines[1024][1024];
    int linecount = 0;

    while (fgets(lines[linecount], sizeof(lines[linecount]), f)
           && linecount < 1024)
        linecount++;
    fclose(f);

    int fixed  = 0;
    int target = line - 1;
    if (target < 0 || target >= linecount) return;

    char *l = lines[target];

    // fix: gets() -> fgets()
    if (!strcmp(type, "BUFFER_OVERFLOW_PATTERN"))
    {
        char *pos = strstr(l, "gets(");
        if (pos)
        {
            char varname[64] = {0};
            sscanf(pos + 5, "%63[^)]", varname);
            char tmp[512];
            snprintf(tmp, sizeof(tmp),
                "    fgets(%s, sizeof(%s), stdin); "
                "/* auto-fixed: gets->fgets */\n",
                varname, varname);
            strncpy(l, tmp, sizeof(lines[target]));
            fixed = 1;
        }

        char *pos2 = strstr(l, "strcpy(");
        if (pos2)
        {
            char dst[64] = {0}, src[64] = {0};
            sscanf(pos2 + 7, "%63[^,], %63[^)]", dst, src);
            char tmp[512];
            snprintf(tmp, sizeof(tmp),
                "    strncpy(%s, %s, sizeof(%s) - 1); "
                "/* auto-fixed: strcpy->strncpy */\n",
                dst, src, dst);
            strncpy(l, tmp, sizeof(lines[target]));
            fixed = 1;
        }
    }

    // fix: system() -> execvp
    if (!strcmp(type, "SYSTEM_CALL"))
    {
        char *pos = strstr(l, "system(");
        if (pos)
        {
            char arg[128] = {0};
            sscanf(pos + 7, "%127[^)]", arg);
            char tmp[512];
            snprintf(tmp, sizeof(tmp),
                "    { char *argv[] = {\"/bin/sh\", \"-c\", %s, NULL}; "
                "execvp(argv[0], argv); } "
                "/* auto-fixed: system->execvp */\n", arg);
            strncpy(l, tmp, sizeof(lines[target]));

            int has_unistd = 0;
            for (int i = 0; i < linecount; i++)
                if (strstr(lines[i], "unistd.h")) { has_unistd = 1; break; }
            if (!has_unistd)
            {
                for (int i = linecount; i > 0; i--)
                    strncpy(lines[i], lines[i-1], sizeof(lines[i]));
                strncpy(lines[0], "#include <unistd.h> /* auto-added */\n",
                        sizeof(lines[0]));
                linecount++;
            }
            fixed = 1;
        }
    }

    // fix: large malloc -> guarded malloc
    if (!strcmp(type, "LARGE_ALLOCATION"))
    {
        char *pos = strstr(l, "malloc(");
        if (pos)
        {
            char sizestr[64] = {0};
            sscanf(pos + 7, "%63[^)]", sizestr);
            char tmp[512];
            snprintf(tmp, sizeof(tmp),
                "    /* auto-fixed: guarded malloc */ "
                "((%s) > 10000000 ? (fprintf(stderr, "
                "\"[Sandbox] Allocation too large\\n\"), "
                "(void*)NULL) : malloc(%s));\n",
                sizestr, sizestr);
            strncpy(l, tmp, sizeof(lines[target]));
            fixed = 1;
        }
    }

    // fix: fopen path sanitization
    if (!strcmp(type, "FILE_OPEN") || !strcmp(type, "HARDCODED_PATH"))
    {
        char *pos = strstr(l, "fopen(");
        if (pos)
        {
            char patharg[128] = {0}, modearg[32] = {0};
            sscanf(pos + 6, "%127[^,], %31[^)]", patharg, modearg);
            char tmp[1024];
            snprintf(tmp, sizeof(tmp),
                "    /* auto-fixed: path sanitized */ "
                "(strstr(%s, \"../\") || strstr(%s, \"/etc\") || "
                "strstr(%s, \"/root\") ? "
                "(fprintf(stderr, \"[Sandbox] Blocked path\\n\"), "
                "(FILE*)NULL) : fopen(%s, %s));\n",
                patharg, patharg, patharg, patharg, modearg);
            strncpy(l, tmp, sizeof(lines[target]));
            fixed = 1;
        }
    }

    if (!fixed) return;

    // write fixed file
    char outpath[512];
    snprintf(outpath, sizeof(outpath), "%s", file);
    char *dot = strrchr(outpath, '.');
    if (dot) *dot = '\0';
    strncat(outpath, "_fixed.c", sizeof(outpath) - strlen(outpath) - 1);

    FILE *out = fopen(outpath, "w");
    if (!out) return;
    for (int i = 0; i < linecount; i++)
        fputs(lines[i], out);
    fclose(out);

    printf("          [AUTO-FIX APPLIED] Fixed file: %s\n", outpath);
    fix_applied = 1;
}

void evaluate_policies()
{
    printf("[Sandbox] Total violations: %d | Highest: %s\n",
           violation_count, highest);
}

int         get_violation_count()     { return violation_count; }
int         get_fix_applied()         { return fix_applied; }
const char *get_highest_severity()    { return highest; }
const char *get_highest_enforcement() { return highest_enforcement; }
#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree.h"
#include "tree-ssa.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "input.h"
#include <string.h>

#include "policy_engine.h"

// helper to get file and line from a gimple statement
static const char *get_file(gimple *stmt)
{
    location_t loc = gimple_location(stmt);
    return LOCATION_FILE(loc) ? LOCATION_FILE(loc) : "unknown";
}

static int get_line(gimple *stmt)
{
    return LOCATION_LINE(gimple_location(stmt));
}

void analyze_statement(gimple *stmt, gimple_stmt_iterator *gsi, const char *func_name)
{
    if (is_gimple_call(stmt))
    {
        tree fn = gimple_call_fn(stmt);
        if (TREE_CODE(fn) == ADDR_EXPR)
            fn = TREE_OPERAND(fn, 0);

        if (!fn || !DECL_NAME(fn)) return;

        const char *name = IDENTIFIER_POINTER(DECL_NAME(fn));
        if (!name) return;

        const char *file = get_file(stmt);
        int line         = get_line(stmt);

        if (!strcmp(name, "system"))
            record_violation("SYSTEM_CALL", "CRITICAL",
                             "Use of system() detected", file, line);
        else if (!strncmp(name, "exec", 4))
            record_violation("EXEC_USAGE", "CRITICAL",
                             "Use of exec family detected", file, line);
        else if (!strcmp(name, "fork"))
            record_violation("FORK_USAGE", "HIGH",
                             "Use of fork() detected", file, line);
        else if (!strcmp(name, "gets"))
            record_violation("BUFFER_OVERFLOW_PATTERN", "HIGH",
                             "Use of gets() detected", file, line);
        else if (!strcmp(name, "strcpy"))
            record_violation("BUFFER_OVERFLOW_PATTERN", "HIGH",
                             "Use of strcpy() detected", file, line);
        else if (!strcmp(name, "socket"))
            record_violation("NETWORK_SOCKET", "HIGH",
                             "Network socket usage detected", file, line);
        else if (!strcmp(name, "pthread_create"))
            record_violation("THREAD_CREATION", "MEDIUM",
                             "Excessive thread creation", file, line);
        if (!strcmp(name, func_name))
            record_violation("RECURSION_DEPTH", "MEDIUM",
                             "Deep recursion detected", file, line);
        else if (!strcmp(name, "malloc"))
        {
            tree arg = gimple_call_arg(stmt, 0);
            if (TREE_CODE(arg) == INTEGER_CST)
            {
                long size = TREE_INT_CST_LOW(arg);
                if (size > 10000000)
                    record_violation("LARGE_ALLOCATION", "MEDIUM",
                                     "Large memory allocation detected", file, line);
                else
                    record_violation("MEMORY_LEAK_PATTERN", "MEDIUM",
                                     "Potential memory leak detected", file, line);
            }
            else
            {
                // Dynamic size malloc, still potential leak
                record_violation("MEMORY_LEAK_PATTERN", "MEDIUM",
                                 "Potential memory leak detected", file, line);
            }
        }
        else if (!strcmp(name, "fopen"))
            record_violation("FILE_OPEN", "LOW",
                             "File opened without validation", file, line);
        else if (!strcmp(name, "fwrite") || !strcmp(name, "fputs"))
            record_violation("FILE_WRITE", "MEDIUM",
                             "File write operation detected", file, line);
    }

    if (gimple_code(stmt) == GIMPLE_ASSIGN)
    {
        tree_code rhs_code = gimple_assign_rhs_code(stmt);
        tree lhs = gimple_assign_lhs(stmt);
        tree rhs = gimple_assign_rhs1(stmt);
        if (rhs_code == POINTER_PLUS_EXPR)
            record_violation("POINTER_ARITHMETIC", "LOW",
                             "Pointer arithmetic detected",
                             get_file(stmt), get_line(stmt));
        else if (TREE_TYPE(lhs) != TREE_TYPE(rhs))
            record_violation("UNSAFE_CAST", "MEDIUM",
                             "Unsafe type casting",
                             get_file(stmt), get_line(stmt));
        else if (DECL_P(lhs) && TREE_STATIC(lhs))
            record_violation("GLOBAL_VARIABLE_USAGE", "LOW",
                             "Use of global variables",
                             get_file(stmt), get_line(stmt));
    }

    if (gimple_code(stmt) == GIMPLE_ASM)
    {
        record_violation("INLINE_ASSEMBLY", "HIGH",
                         "Inline assembly usage",
                         get_file(stmt), get_line(stmt));
    }
}
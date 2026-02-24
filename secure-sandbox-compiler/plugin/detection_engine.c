#include "gcc-plugin.h"
#include "gimple.h"
#include "tree.h"
#include <string.h>

#include "policy_engine.h"

void analyze_statement(gimple *stmt, gimple_stmt_iterator *gsi)
{
    if (is_gimple_call(stmt))
    {
        tree fn = gimple_call_fn(stmt);
        if (TREE_CODE(fn) == ADDR_EXPR)
            fn = TREE_OPERAND(fn, 0);

        const char *name = IDENTIFIER_POINTER(DECL_NAME(fn));

        if (!name) return;

        if (!strcmp(name, "system") ||
            !strcmp(name, "exec")   ||
            !strcmp(name, "fork"))
        {
            record_violation(name, "CRITICAL");
        }

        if (!strcmp(name, "gets") ||
            !strcmp(name, "strcpy"))
        {
            record_violation(name, "HIGH");
        }

        if (!strcmp(name, "malloc"))
        {
            tree arg = gimple_call_arg(stmt, 0);
            if (TREE_CODE(arg) == INTEGER_CST)
            {
                long size = TREE_INT_CST_LOW(arg);
                if (size > 10000000)
                    record_violation("malloc_large", "HIGH");
            }
        }
    }

    if (gimple_code(stmt) == GIMPLE_ASSIGN)
    {
        tree rhs = gimple_assign_rhs1(stmt);
        if (TREE_CODE(rhs) == POINTER_PLUS_EXPR)
        {
            record_violation("pointer_arithmetic", "MEDIUM");
        }
    }
}

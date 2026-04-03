#ifndef DETECTION_ENGINE_H
#define DETECTION_ENGINE_H

#include "gimple.h"

void analyze_statement(gimple *stmt, gimple_stmt_iterator *gsi, const char *func_name);

#endif
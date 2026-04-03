#include <stdio.h>
#include <time.h>
#include "logger.h"

static FILE *logfile;
static int first = 1;

void logger_init()
{
    logfile = fopen("sandbox_log.json", "w");
    fprintf(logfile, "[\n");
    first = 1;
}

void logger_log(const char *severity, const char *type,
                const char *message, const char *file, int line)
{
    if (!logfile) return;

    if (!first) fprintf(logfile, ",\n");
    first = 0;

    time_t now = time(0);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(logfile,
        "  {\n"
        "    \"timestamp\": \"%s\",\n"
        "    \"severity\": \"%s\",\n"
        "    \"type\": \"%s\",\n"
        "    \"message\": \"%s\",\n"
        "    \"file\": \"%s\",\n"
        "    \"line\": %d\n"
        "  }",
        timebuf, severity, type, message,
        file ? file : "unknown", line);
}

void logger_close()
{
    fprintf(logfile, "\n]\n");
    fclose(logfile);
}


#include <stdio.h>

static FILE *logfile;

void logger_init()
{
    logfile = fopen("sandbox_log.json", "w");
    fprintf(logfile, "[\n");
}

void logger_close()
{
    fprintf(logfile, "{}]\n");
    fclose(logfile);
}

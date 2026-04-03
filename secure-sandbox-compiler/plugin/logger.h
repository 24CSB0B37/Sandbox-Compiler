#ifndef LOGGER_H
#define LOGGER_H

void logger_init();
void logger_log(const char *severity, const char *type,
                const char *message, const char *file, int line);
void logger_close();

#endif
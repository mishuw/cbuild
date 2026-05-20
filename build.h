#include <stdbool.h>

void cmd(char* name, int *status, char* cmd);

void errorInfo(char* name, int status);

void exitError(char* name, int status);

bool isFailed(int status);

void append(char **cmd, const char *fmt, ...)
 __attribute__((format(printf, 2, 3)));
#include <unistd.h> /* auto-added */
#include <stdlib.h>
int main() {
    { char *argv[] = {"/bin/sh", "-c", "ls", NULL}; execvp(argv[0], argv); } /* auto-fixed: system->execvp */
    return 0;
}
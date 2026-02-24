#include <stdio.h>

int sandbox_system_wrapper()
{
    printf("[Sandbox] system() blocked.\n");
    return -1;
}

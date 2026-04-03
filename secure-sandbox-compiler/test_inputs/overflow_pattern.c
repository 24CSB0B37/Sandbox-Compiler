#include <stdio.h>
#include <string.h>
int main() {
    char buf[16];
    gets(buf);
    strcpy(buf, "unsafe");
    return 0;
}

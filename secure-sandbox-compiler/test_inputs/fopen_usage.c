#include <stdio.h>
int main() {
    FILE *f = fopen("/tmp/test.txt", "r");
    if (f) fclose(f);
    return 0;
}

#include <stdio.h>
int main() {
    FILE *f = fopen("/tmp/test2.txt", "w");
    if (f) {
        char d[] = "x";
        fwrite(d, 1, 1, f);
        fclose(f);
    }
    return 0;
}

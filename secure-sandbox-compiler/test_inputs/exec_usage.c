#include <unistd.h>
int main() {
    execl("/bin/echo", "echo", "hello", NULL);
    return 0;
}

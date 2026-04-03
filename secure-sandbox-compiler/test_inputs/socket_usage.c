#include <sys/socket.h>
#include <netinet/in.h>
int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    return 0;
}

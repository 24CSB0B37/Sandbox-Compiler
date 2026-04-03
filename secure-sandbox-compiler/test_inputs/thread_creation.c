#include <pthread.h>
#include <stdio.h>

void *thread_func(void *arg) { return arg; }

int main() {
    pthread_t t;
    pthread_create(&t, NULL, thread_func, NULL);
    pthread_join(t, NULL);
    return 0;
}

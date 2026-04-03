#include <stdio.h>
int main() {
    int arr[5] = {1,2,3,4,5};
    int *p = arr;
    p = p + 3;
    printf("%d\n", *p);
    return 0;
}
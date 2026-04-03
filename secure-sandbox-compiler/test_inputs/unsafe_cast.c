int main() {
    int i = 42;
    float *p = (float *)&i;
    (void)p;
    return 0;
}

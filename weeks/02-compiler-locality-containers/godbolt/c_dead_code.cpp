// -O0 vs -O2: the result is never used. What is left of the loop?
void work() {
    int scratch[1000];
    for (int i = 0; i < 1000; ++i) {
        scratch[i] = i * i;
    }
}

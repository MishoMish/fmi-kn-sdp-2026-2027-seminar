// Compare: clang -O2 vs gcc -O0. Where did the loop go?
int sumTo(int n) {
    int total = 0;
    for (int i = 0; i < n; ++i) {
        total += i;
    }
    return total;
}

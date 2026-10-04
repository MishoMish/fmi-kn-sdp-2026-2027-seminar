// -O0 vs -O2: how many calls to square() are left?
static int square(int x) {
    return x * x;
}

int answer() {
    return square(6) + square(2) + 2;
}

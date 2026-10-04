// -O2: why does one function compare and the other not?
// Hint: signed overflow is undefined behaviour, unsigned overflow wraps.
bool plusOneIsBigger(int x) {
    return x + 1 > x;
}

bool plusOneIsBiggerUnsigned(unsigned x) {
    return x + 1 > x;
}

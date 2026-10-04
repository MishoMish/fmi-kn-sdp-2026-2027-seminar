// -O3 -march=x86-64-v3: look for vaddps and ymm registers -
// 8 floats added by ONE instruction (SIMD).
void addArrays(float* __restrict out, const float* a, const float* b, int n) {
    for (int i = 0; i < n; ++i) {
        out[i] = a[i] + b[i];
    }
}

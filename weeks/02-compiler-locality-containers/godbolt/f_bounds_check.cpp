// -O2: what does at() cost compared to operator[]?
#include <cstddef>
#include <vector>

int byIndex(const std::vector<int>& v, std::size_t i) {
    return v[i];
}

int byAt(const std::vector<int>& v, std::size_t i) {
    return v.at(i);
}

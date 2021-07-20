#include <cassert>
#include <experimental/iterator>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simple_vector/simple_vector.h"

template<typename InputIt>
std::ostream& Print(const InputIt& first, const InputIt& last) {
    std::cout << '[';
    std::copy(first, last, std::experimental::make_ostream_joiner(std::cout, ", "));
    return std::cout << ']';
}

int main() {
    using namespace std;

    SimpleVector<int> v{1, 2, 3, 4};
    Print(v.begin(), v.end()) << endl;

    v.Insert(v.begin() + 1, -1);
    Print(v.begin(), v.end()) << endl;

    return 0;
}
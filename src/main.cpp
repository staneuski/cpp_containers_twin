#include <cassert>
#include <experimental/iterator>
#include <iostream>
#include <stdexcept>
#include <string>

#include "simple_vector/simple_vector.h"

int main() {
    using namespace std;

    SimpleVector<string> v{"aa", "bb", "cc", "dd"};

    cout << "[";
    std::copy(v.cbegin(), v.cend(),
              std::experimental::make_ostream_joiner(cout, ", "));
    cout  << "], " << v.GetSize() << endl;

    v.Insert(v.begin() + 2, "+");

    cout << "[";
    std::copy(v.cbegin(), v.cend(),
              std::experimental::make_ostream_joiner(cout, ", "));
    cout  << "], " << v.GetSize() << endl;

    return 0;
}
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#include "simple_vector/simple_vector.h"

int main() {
    using namespace std;

    SimpleVector<string> numbers{"aa", "bb", "cc"};

    cout << "[ ";
    for (const string& number : numbers)
        cout << number << ' ';
    cout << ']' << endl;

    return 0;
}
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "simple_vector/array_ptr.h"

int main() {
    using namespace std;

    ArrayPtr<int> numbers(10);
    const auto& const_numbers = numbers;

    numbers[2] = 42;

    assert(numbers);
    assert(const_numbers[2] == 42);
    assert(&const_numbers[2] == &numbers[2]);

    assert(numbers.Get() == &numbers[0]);

    ArrayPtr<int> numbers_2(5);
    numbers_2[2] = 43;

    numbers.swap(numbers_2);

    assert(numbers_2[2] == 42);
    assert(numbers[2] == 43);

    cout << "[ ";
    for (size_t i = 0u; i < 10u; ++i)
        cout << numbers[i] << ' ';
    cout << ']' << endl;
}
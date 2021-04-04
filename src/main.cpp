#include <cassert>
#include <stdexcept>

#include "single_linked_list/single_linked_list.h"

int main() {
    SingleLinkedList<int> numbers{0, 1, 2, 3};
    std::cout << "numbers: " << numbers << std::endl;

    SingleLinkedList<std::string> words ={"single", "linked", "list"};
    std::cout << "words: " << words << std::endl;

    return 0;
}
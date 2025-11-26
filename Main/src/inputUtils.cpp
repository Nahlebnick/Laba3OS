#include "myLib/inputUtils.h"

void inputValue(int& n)
{
    while (true)
    {
        std::cin >> n;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again." << std::endl;
        }
        else
            break;
    }
}

void EnterArray(int* arr, size_t n)
{
    std::cout << "Enter elements of the array\n";
    for (size_t i = 0; i < n; i++)
    {
        std::cout << "Element " << (i + 1) << ": ";
        inputValue(arr[i]);
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
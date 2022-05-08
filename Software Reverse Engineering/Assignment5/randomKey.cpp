#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <random>

int main()
{
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    std::mt19937 rng(seed);

    std::string str = "HBjPCJzwabHkAHsBzlfdYhmzEUPpahdL";
    std::shuffle(str.begin(), str.end(), rng);
    std::cout << "Key: " << str << std::endl;

    int sum = 0;
    for (int i = 0; i < str.length(); i++)
    {
        sum += str[i];
    }
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}


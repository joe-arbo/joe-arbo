/*Concatenate.cpp
    Purpose: Copy stdin to stdout or copy a file stream to stdout.
    Prereqs: Must use the same copy method for both.
    Author:  Joseph Arbolino
    Date:    9/2/21
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//prints to stdout
void writeToOutput(std::string inputString)
{
    char byte = 0;
    std::vector<char> bytes;

    if (inputString == "-")
    {
        //simply print read buffer
        std::cout << std::cin.rdbuf();
    }
    else
    {
        //iterate through file and print the chars
        std::ifstream file(inputString);
        while(file.get(byte))
        {
            bytes.push_back(byte);
        }
        for (const auto &i : bytes)
        {
            std::cout << i;
        }
    }
}

int main (int argc, char **argv)
{
    if (argc < 2) 
        writeToOutput("-");
    else
    {
        //if file name is included
        for (int i = 0; i < argc; ++i)
        {
            if (i != 0)
                writeToOutput(argv[i]);
        }
    }

    return 0;
} 
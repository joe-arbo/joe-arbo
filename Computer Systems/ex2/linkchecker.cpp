#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

struct module
{
    std::string fileName;
    bool isLibrary = false;
    std::vector<std::vector<std::string>> symbolList;
    std::vector<std::vector<std::string>> uLines;
};

struct definedSymbol
{
    std::string varName;
    std::string moduleName;
};


void commonSymbolCheck(module module1, module module2, std::vector<struct definedSymbol> &definedSymbols)
{
    for (int i = 0; i < module1.symbolList.size(); ++i)
    {
        //found common symbol in list 1
        if (module1.symbolList.at(i).at(2).compare("C") == 0)
        {
            //check with every common symbol in list 2
            for(int j = 0; j < module2.symbolList.size(); ++j)
            {
                //found a common symbol in list 2 and the symbols are the same
                if ((module2.symbolList.at(j).at(2).compare("C") == 0) && (module1.symbolList.at(i).at(3).compare(module2.symbolList.at(j).at(3)) == 0))
                {
                    //if symbols have already been defined
                    if (!definedSymbols.empty())
                    {
                        //check all defined symbols
                        for (int dSymbols = 0; dSymbols < definedSymbols.size(); ++dSymbols)
                        {
                            //if found symbol from above matches a found symbol in the list
                            if (module1.symbolList.at(i).at(3).compare(definedSymbols.at(dSymbols).varName) == 0)
                            {
                                //symbol was defined in another file already
                                //if the first filename is still the same we print
                                if (module1.fileName.compare(definedSymbols.at(dSymbols).moduleName) == 0)
                                {
                                    std::cout << "Common symbol " << module1.symbolList.at(i).at(3) << " multiply defined, first in "
                                              << module1.fileName << " and now in " << module2.fileName << std::endl;
                                }
                            }
                            else
                            {
                                //symbol was not defined in another file
                                std::cout << "Common symbol " << module1.symbolList.at(i).at(3) << " multiply defined, first in "
                                          << module1.fileName << " and now in " << module2.fileName << std::endl;
                                definedSymbol firstSymbol = {module1.symbolList.at(i).at(3), module1.fileName};
                                definedSymbols.push_back(firstSymbol);
                            }
                        }
                    }
                    else
                    {
                        std::cout << "Common symbol " << module1.symbolList.at(i).at(3) << " multiply defined, first in "
                                  << module1.fileName << " and now in " << module2.fileName << std::endl;
                        definedSymbol firstSymbol = {module1.symbolList.at(i).at(3), module1.fileName};
                        definedSymbols.push_back(firstSymbol);
                    }
                }
            }
        }
    }
}

void commonDominatedCheck(module &module1, module &module2)
{
    //run through every element of both lists
    for (int i = 0; i < module1.symbolList.size(); ++i)
    {
        for(int j = 0; j < module2.symbolList.size(); ++j)
        {
            //if symbol names are the same
            if (module1.symbolList.at(i).at(3).compare(module2.symbolList.at(j).at(3)) == 0)
            {
                //if the first one is C and the second one is not C (second overwrites the first one)
                if ((module1.symbolList.at(i).at(2).compare("C") == 0) && (module2.symbolList.at(j).at(2).compare("C") != 0))
                {
                    //if the overwriting symbol is capital
                    if (isupper(module2.symbolList.at(j).at(2)[0]))
                    {
                        std::cout << "Weak " << module1.symbolList.at(i).at(3) << " overridden, defined in " << module1.fileName
                                  << " and overridden as " << module2.symbolList.at(j).at(2) << " in " << module2.fileName << std::endl;

                        module1.symbolList.erase(module1.symbolList.begin() + i);
                        --i;
                    }
                }
                //if the second one is C and the first one is not C (first overwrites the second one)
                else if ((module2.symbolList.at(j).at(2).compare("C") == 0) && (module1.symbolList.at(i).at(2).compare("C") != 0))
                {
                    //if the overwriting symbol is capital
                    if (isupper(module1.symbolList.at(i).at(2)[0]))
                    {
                        std::cout << "Weak " << module2.symbolList.at(j).at(3) << " overridden, defined in " << module2.fileName
                                  << " and overridden as " << module1.symbolList.at(i).at(2) << " in " << module1.fileName << std::endl;

                        module2.symbolList.erase(module2.symbolList.begin() + j);
                        --j;
                    }
                }
            }
        }
    }
}

void staticFunctionCheck(module module1, module module2)
{
    for (int i = 0; i < module1.symbolList.size(); ++i)
    {
        //found static symbol in list 1
        if (module1.symbolList.at(i).at(2).compare("t") == 0)
        {
            //check with every static symbol in list 2
            for(int j = 0; j < module2.symbolList.size(); ++j)
            {
                //found a static symbol in list 2 and the symbol names and sizes are the same
                if ((module2.symbolList.at(j).at(2).compare("t") == 0) && (module1.symbolList.at(i).at(3).compare(module2.symbolList.at(j).at(3)) == 0)
                    && (module1.symbolList.at(i).at(1).compare(module2.symbolList.at(j).at(1)) == 0))
                {
                    std::cout << "Static function " << module1.symbolList.at(i).at(3) << " of size 0x" 
                              << module1.symbolList.at(i).at(1).substr(module1.symbolList.at(i).at(1).length() - 2, module1.symbolList.at(i).at(1).length()) << " appears in both "
                              << module1.fileName << " and " << module2.fileName << ", check for inlining" << std::endl;
                }
            }
        }
    }
}

void globalReferenceCheck(std::vector<struct module> modules, int i)
{
    //for every symbol in that core module
    for (int coreSymbol = 0; coreSymbol < modules.at(i).symbolList.size(); ++coreSymbol)
    {
        bool referenced = false;
        //if coreSymbol is a global symbol
        if (modules.at(i).symbolList.at(coreSymbol).at(2).compare("T") == 0)
        {
            //check if its a main symbol
            if (modules.at(i).symbolList.at(coreSymbol).at(3).compare("main") == 0)
            {
                referenced = true;
            }
            //run only when not main
            else
            {
                //compare with all uLine symbols in all other modules except its own module
                for (int j = 0; j < modules.size(); ++j)
                {
                    if (i != j)
                    {
                        //compare coreSymbol to all uLines in modules.at(j)
                        for (int uline = 0; uline < modules.at(j).uLines.size(); ++uline)
                        {
                            //if symbols match
                            if (modules.at(i).symbolList.at(coreSymbol).at(3).compare(modules.at(j).uLines.at(uline).at(1)) == 0)
                            {
                                referenced = true;
                            }
                        }
                    }
                }
            }
            //coreSymbol is never referenced
            if (!referenced)
            {
                std::cout << "Global symbol " << modules.at(i).symbolList.at(coreSymbol).at(3) << " defined in " << modules.at(i).fileName
                            << " is not referenced by any other file, should be static" << std::endl;
            }
        }
    }
}

void commonPrefixCheck(module module)
{
    bool commonPrefix = true;
    std::string prefix = "";
    //for all symbols in library
    for (int librarySymbol = 0; librarySymbol < module.symbolList.size(); ++librarySymbol)
    {
        //if symbol is exported
        if (module.symbolList.at(librarySymbol).at(2).compare("T") == 0)
        {
            std::string tempPrefix = "";
            std::istringstream iss(module.symbolList.at(librarySymbol).at(3));
            std::getline(iss, tempPrefix, '_');

            //find the longest prefix
            if (tempPrefix.length() > prefix.length())
                prefix = tempPrefix;
        }
    }

    //for all symbols in library
    for (int librarySymbol = 0; librarySymbol < module.symbolList.size(); ++librarySymbol)
    {
        //if symbol is exported
        if (module.symbolList.at(librarySymbol).at(2).compare("T") == 0)
        {
            //check if longest prefix is common
            if (module.symbolList.at(librarySymbol).at(3).substr(0, prefix.length()).compare(prefix) != 0)
            {
                commonPrefix = false;
            }
        }
    }
    //print message
    if (commonPrefix)
    {
        std::cout << "Global symbols exported by library " << module.fileName 
                    << " share common prefix " << prefix << "_" << std::endl;
    }
    else
    {
        std::cout << "Global symbols exported by library " << module.fileName 
                    << " do not share common prefix" << std::endl;
    }
}

int main (int argc, char **argv)
{
    std::vector<struct module> modules;
    bool lPresent = false;
    //iterate through each module
    for (int i = 1; i < argc; ++i)
    {
        //determine if library or not
        if (strcmp(argv[i], "-l") == 0)
        {
            lPresent = true;
            continue;
        }

        //create a newModule to add to the list
        module newModule;
        std::string fileName = argv[i];
        newModule.fileName = fileName.substr(0, fileName.size() - 3);

        //check if library
        if (lPresent)
        {
            newModule.isLibrary = true;
            lPresent = false;
        }   

        //parse the file
        std::ifstream infile(argv[i]);
        std::string line;
        std::vector<std::vector<std::string>> symbolList;
        while (std::getline(infile, line))
        {
            //split by spaces
            std::istringstream iss(line);
            std::vector<std::string> lineWords;
            for(std::string s; iss >> s; )
                lineWords.push_back(s);

            symbolList.push_back(lineWords);
        }

        //make seperate vector for U
        std::vector<std::vector<std::string>> uLines;
        for (int j = 0; j < symbolList.size(); ++j)
        {
            //add U lines to its own vector
            if (symbolList.at(j).at(0).compare("U") == 0)
            {
                uLines.push_back(symbolList.at(j));
                symbolList.erase(symbolList.begin() + j);
                --j;
            }
            //remove (__) symbols
            else if (symbolList.at(j).at(3).compare(0, 2, "__") == 0)
            {
                symbolList.erase(symbolList.begin() + j);
                --j;
            }
        }

        //add new module
        newModule.symbolList = symbolList;
        newModule.uLines = uLines;
        modules.push_back(newModule);
    }

    std::vector<struct definedSymbol> definedSymbols;
    //start comparisons with modules
    for (int i = 0; i < modules.size(); ++i)
    {
        //will only compare modules to each other once and only once
        for (int j = i + 1; j < modules.size(); ++j)
        {
            //if a common symbol is defined in more than one module
            commonSymbolCheck(modules.at(i), modules.at(j), definedSymbols);
            //if a common symbol is dominated by a strong global symbol
            commonDominatedCheck(modules.at(i), modules.at(j));
            //if any static functions are defined multiple times with the same name and size
            staticFunctionCheck(modules.at(i), modules.at(j));
        }
    }

    //comparisons for core and library specific modules
    //for all modules
    for (int i = 0; i < modules.size(); ++i)
    {
        //Global Reference Check
        //if a core module
        if (!modules.at(i).isLibrary)
        {
            //check if any gloabal symbols are not referenced
            globalReferenceCheck(modules, i);
        }  

        //Prefix Test
        //if module is a library
        if (modules.at(i).isLibrary)
        {
            //check for the longest common prefix
            commonPrefixCheck(modules.at(i));
        }
    }
    return 0;
}
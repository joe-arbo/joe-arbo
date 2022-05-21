/*Concatenate.c
    Purpose: Copy stdin to stdout or copy a file stream to stdout.
    Prereqs: Must use the same copy method for both.
    Author:  Joseph Arbolino
    Date:    9/1/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void writeToOuput(FILE* fileptr)
{
    char buff[1024];
    int output;

    while((output = fread(buff, 1, 1024, fileptr)) > 0)
    {
        fwrite(buff, 1, output, stdout);
    }
}

int main(int argc, char **argv) 
{
    if (argc < 2)
        writeToOuput(stdin);
    else
    {
        //if there is a file
        for (int i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "-") == 0)
            {
                writeToOuput(stdin);
            }
            else
            {
                FILE* file = fopen(argv[i], "r");
                writeToOuput(file);
                fclose(file);
            }
        }
    }

    return 0;
}
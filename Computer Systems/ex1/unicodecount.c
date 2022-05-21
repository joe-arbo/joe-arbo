/*unicodecount.c
    Purpose: count all valid UTF-8 encoded Unicode
    Prereqs: N/A
    Author:  Joseph Arbolino
    Date:    9/2/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>
#include <wctype.h>
#include <wchar.h>

int main(int argc, char **argv)
{
    setlocale(LC_CTYPE, "en_US.utf8");

    wint_t chrk;
    int characters = 0;

    while ((chrk = fgetwc(stdin)) != WEOF)
    {
        ++characters;
    }

    if (errno == EILSEQ)
    {
        printf("Invalid or incomplete multibyte or wide character");
    }
    else
    {
        printf("%d" , characters);
    }

}
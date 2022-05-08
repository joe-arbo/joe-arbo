/*
 * Joseph Arbolino
 * Used to hook the puts function call
 * Compiled with: gcc -fPIC -shared -o task1lib.o task1lib.c
 */

#include <unistd.h>

int puts(const char *s)
{
	write(1, "This will never run!\n", 21);
    return(0);	
}

# Software Reverse Engineering

For this course, we used the NSAs disassembler and decompiler `Ghidra` and learned how to change binaries back into human readable code, how to reverse engineer and analyze malware, and how to find and exploit vulnerabilities in software. Although difficult, I found this subject extremely interesting. I gained a much deeper understanding of how computers process executables, what binary files actually contain, and how to manipulate the tools we were provided to solve relatively complex problems hiding inside executable files. This class was a lot of fun and I really think I took a lot of good information away from it, even if it doesn't really pertain all that much to software engineering. 

---

### [Assignment 5](./Assignment_5/)

Given CrackMe's to solve. `randomKey.cpp` and `randomKey.py` are both variations of the same key-cracking program. `task1lib.c` is a hook function used to change the behavior of a binary's function with the same name. `task2.c` is a CrackMe decompiled into C from machine code. `randomKey.cpp` and `randomKey.py` are used to crack this.

### [Assignment 6](./Assignment_6/)

Tasked with created our own CrackMe's. All files are variations of `crackme_joearbo13.c` with different compiler options. `keyGen.py` is used to find a valid key to the custom CrackMe.

### [Assignment 7](./Assignment_7/)

Given 3 binary files that are packed using different packing techniques and tools. Tasked with unpacking them using `x32Debug` and `Scylla`. Binary 3 was then patched using `Ghidra` to change its behavior.

### [Assignment 8](./Assignment_8/)

Given a MineSweeper executable. Tasked with patching the game, directly through the binary, so when the user clicks the help in the menu, the game auto-wins. Also changed menu text options to reflect new functionality. 

### [Assignment 9](./Assignment_9/)

Tasked with experimenting with `WireShark` and internal networks on Virtual Machines. Created a "server" of sorts to monitor web-traffic on one VM and capture/respond from another.

### [hackScript](./hackScript.py/)

Used to edit encoded JSON files extracted from the Steam game BitBurner. Allows user to "cheat" the game and increase levels in-game. This was for my final project of the course, which was reverse engineering BitBurner.
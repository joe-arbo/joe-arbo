# Data Structures and Algorithms

This course explored common data structures and sorting algorithms and how to utilize them in programming applications. It covered external sorting using a Min-Heap and Merge Sort, key-value pair sorting using hash maps, and key-value pair sorting using a skip list and Quad-Tree. This class was partner based and all files were worked on by both my partner and me. The author of each file is listed in the JavaDocs of said file and only indicates the creator, it does not reflect work done.

Instead of including a README in each sub-folder, I've opted to describe each project here. These projects really speak for themselves and only served to teach us about the data structures and sorting algorithms utilized by each program. As a result, there isn't much functionality to describe since each project only takes in information to parse and then sorts and stores that data. The programs only output is formatted text used for automatic grading.

---

### [External Sorting](./ExternalSorting/)

First, a text file is parsed and its data is pulled out as commands and records (e.g. add record1). Using these commands, each record is inserted into a MinHeap data structure and sorted using replacement sort. The data, which at this point, is broken up into multiple chucks, is merged using Multi-way Merge (an external sorting technique). Once all the data has been handled, a record of the actions taken are output to the console for automatic grading.

---

### [Movie Manager](./MovieManager/)

Similar to `ExternalSorting`, this project also parses data from a text file in the form of commands and records. This time the records are Movies being stored in a data base. The records are then inserted into a Hash Map using the Key-Value pairs assigned to them at their creation. `MemMan.java` then performs the collected commands from the parsing stage on the data in the Hash Map. This project familiarized us with Hash Maps and how they store data.

---

### [Quad Tree](./PRQuadTree/)

Again, data is parsed from a text file and interpreted as commands and records. The records are stored in both a QuadTree (a type of binary search tree) and a SkipList. Each of these data structure sorts the data in its own way and then preforms the same commands on said data. At the end of execution, the output of each data structure is compared to make sure each is handling the data correctly.
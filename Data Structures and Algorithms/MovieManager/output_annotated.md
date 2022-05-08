## Annotated sample input/output
The input commands are the same as outlined in sample_input.txt, and showing in the same order, with the same whitespace as in sample_input.txt
***********************************************************************
**input commands**

```shell
print           hashtable
```

**expected output**
```shell
Total records: 0
```

***********************************************************************
**input commands**

```shell
print blocks
```

**expected output**
```shell
32: 0
```

** Note **
After some amount of processing, there might be free blocks of
various sizes. You only print lines for sizes (in ascending order)
that have free blocks, with the free blocks of a given size in
ascending order of starting byte position. For ex:

```shell
8: 48 80
32: 0
```


***********************************************************************
**input commands**

```shell
#Delete string that does not exist
delete    Death    Note
```
**expected output**
```shell
|Death Note| not deleted because it does not exist in the Name database.
```

**Notes**

The string is output normalized (extra whitespace removed) instead of as entered.

***********************************************************************
**input commands**

```shell
#Update record that does not exist
 update delete Another  Note<SEP>Genre
```
**expected output**
```shell
|Another Note| not updated because it does not exist in the Name database.
```

**Notes**
An update command (whether add or delete) that applies to a key that has no matching record in the database.

***********************************************************************
**input commands**

```shell
update delete  Death   Note<SEP>Genre
```
**expected output**
```shell
|Death Note| not updated because the field |Genre| does not exist
```

**Notes**
Another type of update delete error. The string (here "Death Note") might be in the database, but might not have this field to delete.

***********************************************************************
**input commands**

```shell
[A blank line]
````

**Notes**

Your parser should ignore these lines.

***********************************************************************
**input commands**

```shell
    add  Death Note
```
**expected output**
```shell
|Death Note| has been added to the Name database.
```

**Notes**

This is a valid `add` command.

***********************************************************************
**input commands**

```shell
print hashtable
```
**expected output**
```shell
|Death Note| 4
Total records: 1
```

**Notes**

A `print hashtable` command for any hash table that is not empty should look like this. For a table of size M, work from index 0 sequentially through to index M-1. For any slot that holds a record, print its key along with its slot number. *Do not print tombstones.* The last line is a count of the number of records stored in the table.

***********************************************************************
**input commands**

```shell
add   Death     Note
```
**expected output**
```shell
|Death Note| duplicates a record already in the Name database.
```

**Notes**

Example of what to print in the case of a duplicate. Note that the spacing is different on this input line, but the canonical form of the strings is the same, and so should be recognized to be the same.

***********************************************************************
**input commands**

```shell
  print blocks
```
**expected output**
```shell
16: 16
```

**Note**

This indicates that the memory pool now contains a single block of length 16 that begins at byte position 16. This is because 16 bytes were allocated to store the string `Death Note`, that is 10 characters long. 16 is the smallest power of 2 that is equal or bigger than 10.

***********************************************************************
**input commands**

```shell
  update delete   Death   Note<SEP>  Genre
```
**expected output**
```shell
Updated Record: |Death Note<SEP>Genre<SEP>Anime|
```

**Note**

This is the result of modifying the appropriate record. Note that this command should first release the old block that stored the original version of the record, and then allocate a new block to hold the new version of the record.

***********************************************************************
**input commands**

```shell
  print blocks
```
**expected output**
```shell
No free blocks are available.
```

**Note**

This is an example of what to print when the memory pool has no free blocks. The reason why the memory pool has no free blocks is that there is a single record being stored that needs 30 bytes, and the memory pool is currently 32 bytes long.

***********************************************************************
**input commands**

```shell
  add   	Can  	You 		Handle?
```
**expected output**
```shell
Memory pool expanded to be 64 bytes.
|Can You Handle?| has been added to the Name database.
```

**Note**

The first line occurs because the insert operation triggers an expansion of the memory pool (because we started this command with no free blocks available). Note that this input line contains tabs. Your parser should be able to handle (and remove) tabs. This is easy to do with regular expressions.


***********************************************************************
**input commands**

```shell
add  Another Test
```
**expected output**
```shell
|Another Test| has been added to the Name database.
```

**Note**

Another successful `add` operation.

***********************************************************************
**input commands**

```shell
print hashtable
```
**expected output**
```shell
|Death Note| 4
|Can You Handle?| 6
|Another Test| 9
Total records: 3
```

**Note**

Another `print hashtable` example, this time with more data.

***********************************************************************
**input commands**

```shell
delete Death                     Note
```
**expected output**
```shell
|Death Note| has been deleted from the Name database.
```

**Note**

An example of what to print when a successful `delete` operation takes place.

***********************************************************************
**input commands**

```shell
add Death Note
```
**expected output**
```shell
|Death Note| has been added to the Name database.
```

**Note**

A normal add operation. This is demonstrating that the record was properly removed from the hash table by the prior delete operation.


***********************************************************************
**input commands**

```shell
delete Another Test
```
**expected output**
```shell
|Another Test| has been deleted from the Name database.
```

**Note**

A normal delete operation.

***********************************************************************
**input commands**

```shell
print blocks
```
**expected output**
```shell
16: 16 48
```

**Note**

This shows what to print when the memory pool holds multiple
blocks of the same size. In this case, we have two blocks of size 16,
one that starts at byte 16, and another that starts at byte 48.

***********************************************************************
**input commands**

```shell
update add   Death   Note<SEP>  Genre  <SEP> Animation
```
**expected output**
```shell
Memory pool expanded to be 128 bytes.
Updated Record: |Death Note<SEP>Genre<SEP>Animation|
```

**Note**

An example of a normal update operation that triggered an expansion in the memory pool.

***********************************************************************
**input commands**

```shell
update add Death Note<SEP>Format<SEP>Series
```
**expected output**
```shell
Updated Record: |Death Note<SEP>Genre<SEP>Animation<SEP>Format<SEP>Series|
```

**Note**

This shows how to add a second field to a record. New fields go to the end.

***********************************************************************
**input commands**

```shell
update add Death Note<SEP>Genre<SEP>Anime
```
**expected output**
```shell
Updated Record: |Death Note<SEP>Format<SEP>Series<SEP>Genre<SEP>Anime|
```

**Note**

This demonstrates modifying the value of an existing field. The old field is removed, and the replacement is added at the end.

***********************************************************************
**input commands**

```shell
update delete Death Note<SEP>Genre
```
**expected output**
```shell
Updated Record: |Death Note<SEP>Format<SEP>Series|
```

**Note**

This shows a normal "update delete" result. The field has been removed from the record.

***********************************************************************
**input commands**

```shell
add  Fullmetal Alchemist
add Spirited Away
```
**expected output**
```shell
|Fullmetal Alchemist| has been added to the Name database.
|Spirited Away| has been added to the Name database.
```

**Note**

A pair of normal add commands.

***********************************************************************
**input commands**

```shell
add death note
```
**expected output**
```shell
|death note| has been added to the Name database.
```

**Note**

An add command that demonstrates that strings are case sensitive, because we do not consider that |death note| matches |Death Note|. In other words, you can use the normal `String.equals()` method to compare strings.

***********************************************************************
**input commands**

```shell
add Castle in the Sky
```
**expected output**
```shell
Memory pool expanded to be 256 bytes.
Name hash table size doubled to 20 slots.
|Castle in the Sky| has been added to the Name database.
```

**Note**

This `add` operation triggers an expansion in the hash table. It also happens to trigger an expansion in the memory pool. Note especially that the `print` statement for the memory manager comes before that of the hash table. While the hash table does have to decide whether the record will be inserted before it is given to the memory manager, the string should be inserted into the memory manager in order to get the handle prior to actually storing the record in the hash table. And therefore, before the doubling actually takes place in the hash table.

To double the hash table: Create a new array for the new table. Then move sequentially through the old table from position 0 to position M-1 (for an original hash table of size M). For each record that you encounter, insert it into the new hash table using the normal hash insert process.


***********************************************************************
**input commands**

```shell
print hashtable
```
**expected output**
```shell
|Fullmetal Alchemist| 3
|Death Note| 4
|Spirited Away| 5
|Castle in the Sky| 7
|death note| 8
|Can You Handle?| 16
Total records: 6
```

**Note**

Just a normal print statement.
import java.io.FileNotFoundException;
import java.util.ArrayList;

// On my honor:
//
// - I have not used source code obtained from another student,
// or any other unauthorized source, either modified or
// unmodified.
//
// - All source code and documentation used in my program is
// either my original work, or was derived by me from the
// source code published in the textbook for this course.
//
// - I have not discussed coding details about this project with
// anyone other than my partner (in the case of a joint
// submission), instructor, ACM/UPE tutors or the TAs assigned
// to this course. I understand that I may discuss the concepts
// of this program with other students, and that another student
// may help me debug my program so long as neither of us writes
// anything during the discussion or modifies any computer file
// during the discussion. I have violated neither the spirit nor
// letter of this restriction.

/**
 * The class containing the main method
 * 
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */
public class MemMan 
{
    private static int memorySize;
    private static ArrayList<int[]> blockData;
    private static byte[] memoryPool;
    private static Hash hashTable;
    private static final int USED = 1;
    private static final int FREE = 0;
    
    /**
     * @param args
     *            Command line parameters
     * @throws FileNotFoundException 
     */
    public static void main(String[] args) throws FileNotFoundException 
    {
        MemMan memMan = new MemMan();
        
        FileReader reader = new FileReader();
        String[] commands = reader.readFile(args[2]);
        
        //create byte array
        memorySize = Integer.parseInt(args[0]);
        memoryPool = new byte[memorySize];
        blockData = new ArrayList<int[]>();
        int[] firstBlock = {memorySize, FREE}; 
        blockData.add(firstBlock);
        
        //create hash table
        hashTable = new Hash(Integer.parseInt(args[1]));
        
        for (int i = 0; i < commands.length; i++)
        {
            if (commands[i] != null) //Checks for null lines
            {
                while (commands[i].contains("  "))
                {
                    commands[i] = commands[i].replace("  ", " ");
                }
                String[] split;
               //Splits the string if it contains spaces
                if (commands[i].contains(" ")) 
                {
                    split = commands[i].split("[ \t]");  
                }
                else
                {
                    split = new String[1];
                    split[0] = commands[i];
                }
                /*if (split.length == 1)
                {
                    if (split[0].contains("add"))
                    {
                        split[0] = split[0].replace("add", "");
                        Record r = new Record(split[0]);
                        if (hashTable.insert(r))
                        {           
                            memMan.add(r);
                            System.out.println("|" + split[0] + "| "
                                + "has been added to the Name database.");
                            
                            if (hashTable.reHashed())
                            {
                                System.out.println("Name hash table "
                                    + "size doubled to " + 
                                    hashTable.getSize() + " slots.");
                                hashTable.setReHashed(false);
                            }
                            
                            
                        }
                        else
                        {
                            System.out.println("|" + split[0] + "| "
                                + "duplicates a record already "
                                + "in the Name database.");
                        }
                    }
                }*/

                
                if (split[0].contentEquals("add"))
                {
                    String title = "";
                    for (int j = 1; j < split.length; j++)
                    {
                        if (!split[j].isEmpty())
                        {
                            if (title.contentEquals(""))
                            {
                                title = title.concat(split[j]);
                            }
                            else
                            {
                                title = title.concat(" " + split[j]);
                            }
                        }
                    }

                    Record r = new Record(title);
                    
                    if (hashTable.insert(r))
                    {           
                        memMan.add(r);
                        
                        
                        if (hashTable.reHashed())
                        {
                            System.out.println("Name hash table "
                                + "size doubled to " + 
                                hashTable.getSize() + " slots.");
                            hashTable.setReHashed(false);
                        }
                        System.out.println("|" + title + "| has been "
                            + "added to the Name database.");
                        
                    }
                    else
                    {
                        System.out.println("|" + title + "| duplicates a "
                            + "record already in the Name database.");
                    }
                }
                
                /*else if (split[0].contains("add"))
                {
                    split[0] = split[0].replace("add", "");
                    String title = "";
                    for (int j = 0; j < split.length; j++)
                    {
                        if (!split[j].isEmpty())
                        {
                            if (title.contentEquals(""))
                            {
                                title = title.concat(split[j]);
                            }
                            else
                            {
                                title = title.concat(" " + split[j]);
                            }
                        }
                    }
                    Record r = new Record(title);

                    if (hashTable.insert(r))
                    {           
                        memMan.add(r);
                        System.out.println("|" + title + "| has been "
                            + "added to the Name database.");
                        
                        if (hashTable.reHashed())
                        {
                            System.out.println("Name hash table size doubled "
                                + "to " + hashTable.getSize() + " slots.");
                            hashTable.setReHashed(false);
                        }
                        
                        
                    }
                    else
                    {
                        System.out.println("|" + title + "| duplicates a "
                            + "record already in the Name database.");
                    }
                }*/
                
                else if (split[0].contentEquals("delete"))
                {
                    String title = "";
                    for (int j = 1; j < split.length; j++)
                    {
                        if (!split[j].isEmpty())
                        {
                            if (title.contentEquals(""))
                            {
                                title = title.concat(split[j]);
                            }
                            else
                            {
                                title = title.concat(" " + split[j]);
                            }
                        }
                    }
                    
                    Record deleted = hashTable.remove(title);
                    if (deleted == null)
                    {
                        System.out.println("|" + title + "| not deleted "
                            + "because it does not "
                            + "exist in the Name database.");   
                    }
                    else
                    {
                        memMan.delete(deleted);
                        System.out.println("|" + title + "| has been "
                            + "deleted from the Name database.");
                    }
                }
                
                else if (split[0].contentEquals("update"))
                {
                    if (split[1].contentEquals("add"))
                    {
                        String[] separated = commands[i].split("<SEP>");
                        String title = separated[0];
                        title = title.replace("update", "");
                        title = title.replace("add", "");
                        title = title.trim();
                        
                        Record updated = hashTable.search(title);
                        if (updated == null)
                        {
                            System.out.println("|" + title + "| not updated "
                                + "because it does not "
                                + "exist in the Name database.");
                        }
                        else
                        {
                            for (int j = 1; j < separated.length; j += 2)
                            {
                                String field = separated[j].trim();
                                String value;
                                if (separated.length <= j + 1)
                                {
                                    value = "";
                                }
                                else
                                {
                                    value = separated[j + 1].trim();
                                }
                                
                                //delete the original record from memory
                                memMan.delete(updated);
                                //update record in hash table
                                updated.addField(new KVPair<String, 
                                    String>(field, value));
                                //add new record
                                memMan.add(updated);
                                
                            }                        
                            System.out.println("Updated Record: |" + 
                                updated.toString() + "|");
                        }
                        
                    }
                    else if (split[1].contentEquals("delete"))
                    {
                        String[] separated = commands[i].split("<SEP>");
                        String title = separated[0];
                        title = title.replace("update", "");
                        title = title.replace("delete", "");
                        title = title.trim();
                        Record updated = hashTable.search(title);
                        if (updated == null)
                        {
                            System.out.println("|" + title + "| not updated "
                                + "because it does not exist in "
                                + "the Name database.");
                        }
                        else
                        {
                            String field = separated[1].trim();
                            
                            //update record in hash table if it exists
                            if (updated.removeField(field))
                            {
                                //delete the original record from memory
                                memMan.delete(updated);
                                
                                //add new record
                                memMan.add(updated);
                                System.out.println("Updated Record: |" 
                                    + updated.toString() + "|");
                            }
                            else
                            {
                                System.out.println("|" + title + "| not "
                                    + "updated "
                                    + "because the field |" + field + 
                                    "| does not exist");
                            }
                            
                        }
                    }
                }
                else if (split[0].contentEquals("print"))
                {
                    for (int j = 0; j < split.length; j++)
                    {
                        if (split[j].contentEquals("hashtable"))
                        {
                            for (int k = 0; k < hashTable.getSize(); k++)
                            {
                                if (hashTable.getTable()[k] != null && 
                                    !hashTable.getTable()[k].getName().
                                    contentEquals(""))
                                {
                                    System.out.println("|" + 
                                        hashTable.getTable()[k].getName() + 
                                        "| " + k);
                                }
                            }
                            System.out.println("Total records: " + 
                                hashTable.getRecordCount());
                            
                        }
                        else if (split[j].contentEquals("blocks"))
                        {
                            int largest = memMan.getLargestFreeBlock();
                            if (largest == 0)
                            {
                                System.out.println("No free blocks "
                                    + "are available.");
                            }
                            //for all possible sizes
                            for (int blockSize = 2; blockSize <= largest; 
                                blockSize = blockSize * 2)
                            {
                                //for all blocks
                                int bytesBefore = 0;
                                boolean isFirst = true;
                                for (int block = 0; block < 
                                    blockData.size(); block++)
                                {
                                    if (blockData.get(block)[0] == 
                                        blockSize && 
                                        blockData.get(block)[1] == FREE)
                                    {
                                        if (isFirst)
                                        {
                                            System.out.print(blockSize + ": ");
                                            isFirst = false;
                                        }
                                        System.out.print(bytesBefore + " ");
                                    }
                                    bytesBefore += blockData.get(block)[0];
                                }
                                if (!isFirst)
                                {
                                    System.out.println();
                                }
                            }
                        }
                    }  
                }
            }  
        } 
    }

    /**
     * adds bytes to memPool
     * @return true if added, false otherwise
     * @param r the record being added to the database
     */
    public boolean add(Record r)
    {
        byte[] bytesToAdd = r.toString().getBytes();
        int length = bytesToAdd.length;
                
        //find the largest free block
        //check if length is bigger than all free blocks
        int largestFreeBlock = getLargestFreeBlock();
        
        //grow if length will not fit in any free blocks
        while (length > largestFreeBlock)
        {
            growMemPool();
            largestFreeBlock = getLargestFreeBlock();
        }
        
        //once there is enough room for it to fit
        //find the smallest free block that length will fit in
        int diff = memorySize;
        int dataFitIndex = 0;
        for (int i = 0; i < blockData.size(); ++i)
        {
            if (blockData.get(i)[1] == FREE 
                    && (blockData.get(i)[0] - length) < diff 
                    && (blockData.get(i)[0] - length) >= 0)
            {
                diff = blockData.get(i)[0] - length;
                dataFitIndex = i;
            }
        }
        
        //if the smallest free block is too big, split
        while (blockData.get(dataFitIndex)[1] == FREE 
            && length < (blockData.get(dataFitIndex)[0]) / 2)
        {
            //split block until length fits well
            //return the index of the new block
            split(dataFitIndex);
        }
        
        //add bytesToAdd to memoryPool
        int totalBytesBefore = 0;
        for (int i = 0; i < dataFitIndex; ++i)
        {
            totalBytesBefore += blockData.get(i)[0];
        }
        for (int i = 0; i < length; ++i)
        {
            memoryPool[totalBytesBefore + i] = bytesToAdd[i];
        }
        
        //set as used
        int[] block = {blockData.get(dataFitIndex)[0], USED};
        blockData.set(dataFitIndex, block);
        
        //set index in Hash table
        hashTable.search(r.getName()).setByteIndex(totalBytesBefore);
        hashTable.search(r.getName()).setBlockIndex(dataFitIndex);
        
        return true;
    }

    /**
     * Will delete a record from memoryPool
     * @param r the record
     * @return true if removed, flase otherwise
     */
    public boolean delete(Record r)
    {
        byte[] bytesToDelete = r.toString().getBytes();           
        //set memory to 0
        for (int i = 0; i < bytesToDelete.length; ++i)
        {
            memoryPool[i + r.getByteIndex()] = 0;
        }
        
        //clear block and add to free
        int[] block = {blockData.get(r.getBlockIndex())[0], FREE};
        blockData.set(r.getBlockIndex(), block);

        //try to merge
        merge();
        
        return true;
    }
    
    /**
     * Will merge any adjacent free blocks
     */
    public void merge()
    {      
        //check if merge is possible
        int bytesBefore = 0;
        for (int i = 0; i < blockData.size() - 1; ++i)
        {        
            //only examine if FREE block
            if (blockData.get(i)[1] == FREE && 
                blockData.get(i + 1)[1] == FREE && 
                blockData.get(i + 1)[0] == blockData.get(i)[0])
            {
                StringBuilder block1 = new 
                    StringBuilder(Integer.toBinaryString(bytesBefore));
                StringBuilder block2 = new 
                    StringBuilder(Integer.toBinaryString(bytesBefore 
                        + blockData.get(i)[0]));
                
                //correct if 0
                int numBits = (int)(Math.log10(blockData.get(i)[0]) 
                    / Math.log10(2));
                if (block1.toString().contentEquals("0"))
                {
                    for (int bit = 0; bit < numBits; bit++)
                    {
                        block1.append('0');
                    }
                }
                
                //invert bit
                StringBuilder block1Buddy = new StringBuilder(block1);
                block1Buddy.reverse();
                char bitToInvert = block1Buddy.charAt(numBits);
                if (bitToInvert == '0')
                {
                    block1Buddy.setCharAt(numBits, '1');
                }
                else
                {
                    block1Buddy.setCharAt(numBits, '0');
                }
                
                block1Buddy.reverse();                
                
                //can merge
                if (block2.toString().contentEquals(block1Buddy.toString()))
                {
                    int[] block = {blockData.get(i)[0] * 2, FREE};
                    blockData.set(i, block);
                    blockData.remove(i + 1);
                    
                    //adjust blockIndex after i
                    for (int j = i + 2; j <= blockData.size(); j++)
                    {
                        if (blockData.get(j - 1)[1] == USED)
                        {
                            Record r = hashTable.search(j);
                            r.setBlockIndex(r.getBlockIndex() - 1);    
                        }
                    }
                    
                    //call merge again
                    merge();
                }
            }
            bytesBefore += blockData.get(i)[0];
        }
    }
    
    /**
     * Splits the memPool
     * @param blockIndex the index of
     * the block to be split
     */
    public void split(int blockIndex)
    {       
        int newBlockLength = blockData.get(blockIndex)[0] / 2;       
        int[] block = {newBlockLength, FREE};
        blockData.set(blockIndex, block);
        blockData.add(blockIndex, block);
        
        //adjust all blockIndexes after insertion
        for (int j = blockData.size() - 1; j > blockIndex + 1; j--)
        {
            if (blockData.get(j)[1] == USED)
            {
                Record r = hashTable.search(j - 1);
                r.setBlockIndex(r.getBlockIndex() + 1);    
            }
        }
    }
    
    /**
     * Grows the memPool
     */
    public void growMemPool()
    {
        //grow memoryPool
        byte[] newMemPool = new byte[memoryPool.length * 2];
        System.arraycopy(memoryPool, 0, newMemPool, 0, memoryPool.length);
        memoryPool = newMemPool;
        memorySize = memoryPool.length;
        
        //grow blockData
        int[] block = {memoryPool.length / 2, FREE};
        blockData.add(block);        
        
        System.out.println("Memory pool expanded to be " + 
            newMemPool.length + " bytes.");
        merge();
    }
    
    /**
     * gets the largest free block
     * @return largestFreeBlock index
     */
    public int getLargestFreeBlock()
    {
        int largestFreeBlock = 0;
        for (int i = 0; i < blockData.size(); ++i)
        {
            if (blockData.get(i)[1] == FREE && 
                blockData.get(i)[0] > largestFreeBlock) 
            {
                largestFreeBlock = blockData.get(i)[0];
            }
        }
        return largestFreeBlock;
    }
}

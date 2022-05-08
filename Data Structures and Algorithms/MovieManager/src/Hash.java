/**
 * Creates a new Hash Table. Inserts strings at given indexes
 * in an array based on their value in bytes.
 *
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */

public class Hash {

    
    private int size;
    private Record[] hashTable;
    private int recordCount;
    //tombstone flyweight
    private Record tombstone;
    private boolean reHashed;
    /**
     * Create a new Hash object.
     * @param size the size of the hash table
     */
    public Hash(int size) {
        this.size = size;
        hashTable = new Record[size];
        recordCount = 0;
        tombstone = new Record("");
        reHashed = false;
    }
    
    /**
     * Gets the current hash table
     * @return the list of all strings
     */
    public Record[] getTable()
    {
        return hashTable;
    }
    
    /**
     * Gets the number of records in the table
     * @return the current record count
     */
    public int getRecordCount()
    {
        return recordCount;
    }
    
    /**
     * Returns the current size of the table
     * @return the size of the table
     */
    public int getSize()
    {
        return size;
    }
    
    /**
     * Gets the value of reHashed
     * @return the value of reHashed
     */
    public boolean reHashed()
    {
        return reHashed;
    }
    
    /**
     * Sets the value of reHashed
     * @param newHash the new value of newHash
     */
    public void setReHashed(boolean newHash)
    {
        reHashed = newHash;
    }
    
    /**
     * Inserts a new string into the hash table
     * @param newRecord the string to be inserted
     * @return if the record was successfully inserted
     */
    public boolean insert(Record newRecord)
    {
        if (newRecord == null)
        {
            return false;
        }
        
        
        if (search(newRecord.getName()) != null)
        {
            return false;
        }
        
        while (recordCount >= size / 2)
        {
            reHash();
            reHashed = true;
        }
        
        int home = h(newRecord.getName(), size);
        int pos = home;
        for (int i = 1; hashTable[pos] != null 
            && !hashTable[pos].getName().equals(tombstone.getName()); i++) 
        {
            pos = (home + (i * i)) % size;
        }
        hashTable[pos] = newRecord;
        recordCount++;
        
        
        return true;
    }
    
    /**
     * Removes the given name from the list
     * @param name the name of the record to be removed
     * @return the record if removed, or null otherwise
     */
    public Record remove(String name)
    {
        if (name == null || recordCount == 0)
        {
            return null;
        }
        
        int home = h(name, size);
        int pos = home;
        //checks if the hashed value is null
        if (hashTable[pos] == null)
        {
            return null;
        }
        //checks if the current value is the record to
        //be removed
        if (hashTable[pos].getName().contentEquals(name))
        {
            Record temp = hashTable[pos];
            hashTable[pos] = tombstone;
            recordCount--;
            return temp;
        }
        
        //searches for the value in the table
        for (int i = 1; i < size/*hashTable[pos] != null*/; i++) 
        {
            pos = (home + (i * i)) % size; //check rehashing for correct index
            if (hashTable[pos] == null)
            {
                return null;
            }
            if (hashTable[pos].getName().contentEquals(name))
            {
                Record temp = hashTable[pos];
                hashTable[pos] = tombstone;
                recordCount--;
                return temp;
            }
            if (i >= size)
            {
                System.out.println("Infinite loop");
                return null;
            }
        }
        return null;
    }
    
    /**
     * Searches for a record with the given name
     * @param name the name of the record to be searched for
     * @return the found record, or null otherwise
     */
    public Record search(String name)
    {
        if (name == null || recordCount == 0)
        {
            return null;
        }
        
        int home = h(name, size);
        int pos = home;
        for (int i = 1; hashTable[pos] != null; i++) 
        {
            if (name.contentEquals(hashTable[pos].getName()))
            {
                return hashTable[pos];
            }
            pos = (home + (i * i)) % size;
        }
        return null;
    }
    
    /**
     * Searches for a record with the given index
     * @param index the index of the record to be searched for
     * @return the found record, or null otherwise
     */
    public Record search(int index)
    {
        if (recordCount == 0)
        {
            return null;
        }
        
        for (int i = 0; i < hashTable.length; ++i)
        {
            if (hashTable[i] != null
                && hashTable[i].getBlockIndex() == index)
            {
                return hashTable[i];
            }
        }
        return null;
    }
    
    /**
     * Copies the current table into one that is twice as large
     * Used when the record count is greater than or equal to
     * half of the size of the current table
     */
    private void reHash()
    {
        size = size * 2;
        Record[] newTable = new Record[size];
        for (int i = 0; i < hashTable.length; i++)
        {
            if (hashTable[i] != null 
                && !hashTable[i].getName().equals(tombstone.getName()))
            {
                insert(hashTable[i], newTable, size);
            }
        }
        hashTable = new Record[size];
        for (int i = 0; i < size; i++)
        {
            hashTable[i] = newTable[i];
        }
        //hashTable = newTable.clone();
        
        
    }
    
    /**
     * Inserts records into a new table when re-hashing
     * @param newRecord the record to be inserted
     * @param table the new table
     * @param newSize the newly doubled size of the record
     */
    private void insert(Record newRecord, Record[] table, int newSize)
    {    
        int home = h(newRecord.getName(), newSize);
        int pos = home;
        for (int i = 1; table[pos] != null 
            && !table[pos].getName().equals(tombstone.getName()); i++) 
        {
            pos = (home + (i * i)) % newSize;
        }
        table[pos] = newRecord;

    }

    /**
     * Compute the hash function. Uses the "sfold" method from the OpenDSA
     * module on hash functions
     *
     * @param s
     *            The string that we are hashing
     * @param m
     *            The size of the hash table
     * @return The home slot for that string
     */
    // You can make this private in your project.
    // This is public for distributing the hash function in a way
    // that will pass milestone 1 without change.
    public int h(String s, int m) {
        int intLength = s.length() / 4;
        long sum = 0;
        for (int j = 0; j < intLength; j++) 
        {
            char[] c = s.substring(j * 4, (j * 4) + 4).toCharArray();
            long mult = 1;
            for (int k = 0; k < c.length; k++) 
            {
                sum += c[k] * mult;
                mult *= 256;
            }
        }

        char[] c = s.substring(intLength * 4).toCharArray();
        long mult = 1;
        for (int k = 0; k < c.length; k++) 
        {
            sum += c[k] * mult;
            mult *= 256;
        }

        return (int)(Math.abs(sum) % m);
    }
}

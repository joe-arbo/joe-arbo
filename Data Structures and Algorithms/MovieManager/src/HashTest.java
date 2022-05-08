import java.util.ArrayList;
import java.util.Random;
import student.TestCase;

// -------------------------------------------------------------------------
/**
 * Test the hash function
 *
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */
public class HashTest extends TestCase {
    
    private Hash myHash;
    /**
     * Sets up the tests that follow.
     */
    public void setUp() {
        myHash = new Hash(101);
    }


    /**
     * Test the hash function
     */
    public void testh() {
        assertEquals(myHash.h("aaaabbbb", 101), 75);
        assertEquals(myHash.h("aaaabbb", 101), 1640219587 % 101);
    }
    
    /**
     * Tests the insert function
     */
    public void testInsert()
    {
        assertFalse(myHash.insert(null));
        Record record1 = new Record("aaaabbbb");
        Record record2 = new Record("aaaabbb");
        myHash.insert(record1);
        myHash.insert(record2);
        assertEquals(myHash.getTable()[75], record1);
        assertEquals(myHash.getTable()[1640219587 % 101], record2);
    }
    
    /**
     * Tests reHash
     */
    public void testReHash()
    {
        Hash shortHash = new Hash(5);
        Record record1 = new Record("aqwabbbb");
        Record record2 = new Record("afaabbb");
        Record record3 = new Record("asaabbbc");
        Record record4 = new Record("anaabbbc");
        Record record5 = new Record("afdabbbd");
        Record record6 = new Record("avaabbd");
        shortHash.insert(record1);
        shortHash.insert(record2);
        assertEquals(shortHash.getTable().length, 5);
        assertEquals(shortHash.getRecordCount(), 2);
        shortHash.insert(record3);
        assertEquals(shortHash.getTable().length, 10);
        assertEquals(shortHash.getRecordCount(), 3);
        shortHash.insert(record4);
        shortHash.insert(record5);
        assertEquals(shortHash.getRecordCount(), 5);
        shortHash.insert(record6);
        assertEquals(shortHash.getRecordCount(), 6);
        assertEquals(shortHash.getTable().length, 20);
    }
    
    /**
     * Tests inserting records when 
     * their spot is already taken
     */
    public void testProbe()
    {
        Hash probeHash = new Hash(18);
        Record record1 = new Record("aaaabbbb");
        Record record2 = new Record("bbbbaaaa");
        Record record3 = new Record("bbaaaabb");
        Record record4 = new Record("aabbbbaa");
        Record record5 = new Record("i");
        Record record6 = new Record("aaaabbbbdw");
        probeHash.insert(record1);
        probeHash.insert(record2);
        probeHash.insert(record3);
        probeHash.insert(record4);
        probeHash.insert(record5);
        probeHash.insert(record6);
        assertEquals(probeHash.getTable()[15], record1);
        assertEquals(probeHash.getTable()[16], record2);
        assertEquals(probeHash.getTable()[1], record3);
        assertEquals(probeHash.getTable()[6], record4);
        assertEquals(probeHash.getTable()[13], record5);
        assertEquals(probeHash.getTable()[4], record6);
    }
    
    /**
     * Tests remove
     */
    public void testRemove()
    {
        assertNull(myHash.remove(""));
        Record record1 = new Record("aaaabbbb");
        assertNull(myHash.remove(null));
        Record record2 = new Record("aaaabbb");
        assertNull(myHash.remove("aaaabbbb"));
        myHash.insert(record1);
        myHash.insert(record2);
        assertEquals(myHash.getRecordCount(), 2);
        myHash.remove("aaaabbbb");
        assertEquals(myHash.getRecordCount(), 1);
        myHash.insert(record1);
        assertEquals(myHash.getRecordCount(), 2);
        assertNull(myHash.remove("y"));
        myHash.remove("aaaabbb");
        assertEquals(myHash.getRecordCount(), 1);
    }
    
    /**
     * Tests search
     */
    public void testSearch()
    {
        Hash probeHash = new Hash(18);
        assertNull(probeHash.search("asdfgrwe"));
        assertNull(probeHash.search(null));
        Record record1 = new Record("aaaabbbb");
        Record record2 = new Record("bbbbaaaa");
        Record record3 = new Record("bbaaaabb");
        Record record4 = new Record("aabbbbaa");
        Record record5 = new Record("i");
        Record record6 = new Record("aaaabbbbdw");
        Record notInserted = new Record("aqwabbbb");
        Record record7 = new Record("awesrgdtf");
        Record record8 = new Record("ykjthrgdf");
        Record record9 = new Record("triuuyt");
        Record record10 = new Record("rtuykiukj");
        Record record11 = new Record("p");
        Record record12 = new Record("rdtujdrhfxb");
        probeHash.insert(record1);
        probeHash.insert(record2);
        probeHash.insert(record3);
        probeHash.insert(record4);
        probeHash.insert(record5);
        probeHash.insert(record6);
        probeHash.insert(record7);
        probeHash.insert(record8);
        probeHash.insert(record9);
        probeHash.insert(record10);
        probeHash.insert(record11);
        probeHash.insert(record12);
        assertEquals(probeHash.getRecordCount(), 12);
        assertNull(probeHash.search(notInserted.getName()));
        assertEquals(probeHash.search("aaaabbbb"), record1);
        assertEquals(probeHash.search("bbbbaaaa"), record2);
        assertEquals(probeHash.search("bbaaaabb"), record3);
        assertEquals(probeHash.search("aabbbbaa"), record4);
        assertEquals(probeHash.search("i"), record5);
        assertEquals(probeHash.search("aaaabbbbdw"), record6);
        assertEquals(probeHash.search("awesrgdtf"), record7);
        assertEquals(probeHash.search("ykjthrgdf"), record8);
        assertEquals(probeHash.search("triuuyt"), record9);
        assertEquals(probeHash.search("rtuykiukj"), record10);
        assertEquals(probeHash.search("p"), record11);
        assertEquals(probeHash.search("rdtujdrhfxb"), record12);
        
    }
    
    /**
     * Tests remove with the use of tombstones
     */
    public void testRemoveTombstones()
    {
        Hash probeHash = new Hash(18);
        Record record1 = new Record("aaaabbbb");
        Record record2 = new Record("bbbbaaaa");
        Record record3 = new Record("bbaaaabb");
        Record record4 = new Record("aabbbbaa");
        Record record5 = new Record("i");
        Record record6 = new Record("aaaabbbbdw");
        probeHash.insert(record1);
        probeHash.insert(record2);
        probeHash.insert(record3);
        probeHash.insert(record4);
        probeHash.insert(record5);
        probeHash.remove("aabbbbaa");
        assertEquals(probeHash.getRecordCount(), 4);
        assertFalse(probeHash.insert(record5));
        assertTrue(probeHash.insert(record4));
        assertEquals(probeHash.getTable()[15], record1);
        assertEquals(probeHash.getTable()[16], record2);
        assertEquals(probeHash.getTable()[1], record3);
        assertEquals(probeHash.getTable()[6], record4);
        assertEquals(probeHash.getTable()[13], record5);
        
        assertEquals(probeHash.remove("bbbbaaaa"), record2);
        assertTrue(probeHash.insert(record6));
        assertEquals(probeHash.getTable()[16], record6);
        
    }
    
    /**
     * Tests based on the given input
     */
    public void testInput()
    {
        Hash inputHash = new Hash(10);
        Record record1 = new Record("Death Note");
        Record record2 = new Record("Fullmetal Alchemist");
        Record record3 = new Record("Spirited Away");
        Record record4 = new Record("death note");
        Record record5 = new Record("Castle in the Sky");
        Record record6 = new Record("Can You Handle?");
        Record record7 = new Record("Another Test");
        
        inputHash.remove("Death Note");
        inputHash.insert(record1);
        inputHash.insert(record1);
        inputHash.insert(record1);
        inputHash.insert(record6);
        inputHash.insert(record7);
        assertEquals(inputHash.getTable()[4], record1);
        assertEquals(inputHash.getTable()[6], record6);
        assertEquals(inputHash.getTable()[9], record7);
        
        inputHash.remove("Death Note");
        inputHash.insert(record1);
        inputHash.remove("Another Test");
        inputHash.insert(record1);
        inputHash.insert(record1);
        inputHash.insert(record1);
        inputHash.insert(record2);
        inputHash.insert(record3);
        inputHash.insert(record4);
        inputHash.insert(record5);
       
        assertEquals(inputHash.getTable()[3], record2);
        assertEquals(inputHash.getTable()[4], record1);
        assertEquals(inputHash.getTable()[5], record3);
        assertEquals(inputHash.getTable()[7], record5);
        assertEquals(inputHash.getTable()[8], record4);
        assertEquals(inputHash.getTable()[16], record6);
        assertEquals(inputHash.getRecordCount(), 6);

        
    }
    
    /**
     * Tests reHash with a table that contains tombstones
     */
    public void testReHashTombstones()
    {
        Hash probeHash = new Hash(18);
        Record record1 = new Record("aaaabbbb");
        Record record2 = new Record("bbbbaaaa");
        Record record3 = new Record("bbaaaabb");
        Record record4 = new Record("aabbbbaa");
        Record record5 = new Record("i");
        Record record6 = new Record("aaaabbbbdw");
        probeHash.insert(record1);
        probeHash.insert(record2);
        probeHash.insert(record3);
        probeHash.insert(record4);
        probeHash.insert(record5);
        probeHash.insert(record6);
        probeHash.remove("aabbbbaa");

        assertEquals(probeHash.getRecordCount(), 5);
        assertEquals(probeHash.search("i"), record5);
        assertFalse(probeHash.insert(record5));
        assertFalse(probeHash.insert(record6));
        assertEquals(probeHash.getRecordCount(), 5);
        assertTrue(probeHash.insert(record4));
        assertEquals(probeHash.getTable()[6], record4);
        assertEquals(probeHash.remove("aaaabbbbdw"), record6);
        assertEquals(probeHash.remove("i"), record5);
        
        Record record7 = new Record("awesrgdtf");
        Record record8 = new Record("ykjthrgdf");
        Record record9 = new Record("triuuyt");
        Record record10 = new Record("rtuykiukj");
        Record record11 = new Record("p");
        Record record12 = new Record("rdtujdrhfxb");
        probeHash.insert(record7);
        probeHash.insert(record8);
        probeHash.insert(record9);
        probeHash.insert(record10);
        probeHash.insert(record11);
        assertEquals(probeHash.getRecordCount(), 9);
        probeHash.insert(record12);
        assertEquals(probeHash.getRecordCount(), 10);
        
        assertEquals(probeHash.search("aaaabbbb"), record1);
        assertEquals(probeHash.search("bbbbaaaa"), record2);
        assertEquals(probeHash.search("bbaaaabb"), record3);
        assertEquals(probeHash.search("aabbbbaa"), record4);
        assertNull(probeHash.search("i"));
        assertNull(probeHash.search("aaaabbbbdw"));
        assertEquals(probeHash.search("awesrgdtf"), record7);
        assertEquals(probeHash.search("ykjthrgdf"), record8);
        assertEquals(probeHash.search("triuuyt"), record9);
        assertEquals(probeHash.search("rtuykiukj"), record10);
        assertEquals(probeHash.search("p"), record11);
        assertEquals(probeHash.search("rdtujdrhfxb"), record12);
        assertNull(probeHash.remove("werew"));
        
        assertEquals(probeHash.getSize(), 36);
        
        
        Random rand = new Random();
        ArrayList<Record> records = new ArrayList<Record>();
        int count = 0;
        int oldRecords = probeHash.getRecordCount();
        while (count < 500)
        {
            Record newRecord = 
                new Record(String.valueOf(rand.nextInt(1000000))); 
            
            if (probeHash.insert(newRecord))
            {
                count++;
                records.add(newRecord);
            }
            
        }
        
        for (int i = 0; i < records.size(); i++)
        {
            assertEquals(probeHash.search(records.get(i).getName()), 
                records.get(i));
            assertEquals(probeHash.remove(records.get(i).getName()), 
                records.get(i));
            assertNull(probeHash.search(records.get(i).getName()));
        }
        assertEquals(probeHash.getRecordCount(), oldRecords);
        //assertEquals(probeHash.getSize(), 72);
        
        
    }
    
    
}

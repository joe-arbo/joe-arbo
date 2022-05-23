import student.TestCase;

/**
 * Tests the methods of the SkipList class
 * 
 * @author Ryan Fasco 906093506
 * @version 1.0
 */
public class SkipListTest extends TestCase {

    // The primary list used for testing
    private SkipList<Integer, String> list1;
    // An empty list used for testing
    private SkipList<Integer, String> emptyList;

    /**
     * Sets up the SkipLists to be tested
     */
    public void setUp() {
        list1 = new SkipList<Integer, String>();
        list1.insert(1, "one");
        list1.insert(2, "two");
        list1.insert(3, "three");
        list1.insert(4, "four");
        list1.insert(4, "five");
        list1.insert(5, "five");
        list1.insert(5, "five 2");
        list1.insert(6, "six");
        list1.insert(7, "seven");
        list1.insert(8, "eight");
        list1.insert(9, "nine");
        list1.insert(10, "ten");
        list1.insert(11, "eleven");
        emptyList = new SkipList<Integer, String>();
    }

    /**
     * Tests find(e)
     */
    public void testFindKey() {
        Integer nullInt = null;
        assertNull(emptyList.find(5));
        assertNull(list1.find(nullInt));
        assertTrue(list1.find(2).key().equals(2));
        assertNull(list1.find(20));

    }

    /**
     * Tests find(k)
     */
    public void testFindElement() {
        String nullString = null;
        assertNull(emptyList.find(nullString));
        assertNull(list1.find(nullString));
        assertNotNull(list1.find("one"));
        assertNull(list1.find("seventeen"));

    }

    /**
     * Tests insert(k, e)
     */
    public void testInsert() {
        assertNotNull(list1.find(3));
        assertNull(list1.find(25));
        assertNotNull(list1.find(5));
        assertTrue(list1.find(5).element().equals("five") 
                || list1.find(5).element().equals("five 2"));
    }

    /**
     * Tests remove(k)
     */
    public void testRemoveKey() {
        Integer nullInt = null;
        assertNull(list1.remove(nullInt));
        assertNull(list1.remove(25));
        assertTrue(list1.remove(3).key().equals(3));
        assertNull(list1.find(3));
        list1.remove(12);
        assertNull(list1.find(12));
        list1.insert(12, "twelve");
        assertNotNull(list1.find(12));
        list1.remove(12);
        list1.remove(13);
        assertNull(list1.find(12));
        assertNull(list1.find(13));
        assertNotNull(list1.find(1));
        assertNotNull(list1.find(4));
    }

    /**
     * Tests remove (k) when removing a node with the same key as another node
     */
    public void testRemoveKeyMultiple() {
        list1.remove(5);
        assertNotNull(list1.find(5));
        assertNotNull(list1.find(4));
    }

    /**
     * Tests remove(k) when removing a node after 
     * attempting to remove a null value
     */
    public void testRemoveAfterNull() {
        Integer nullInt = null;
        assertNull(list1.remove(nullInt));
        assertNotNull(list1.find(1));
        assertNotNull(list1.find(11));
    }

    /**
     * Tests removeError
     */
    public void testRemoveError() {
        list1.remove(9);
        list1.remove(10);
        assertNotNull(list1.find(8));
        assertNotNull(list1.find(2));
        assertNotNull(list1.find(11));
    }

    /**
     * Tests remove(k) on a node that was removed and re-inserted
     */
    public void testRemoveKeyAfterInsert() {
        list1.remove(12);
        assertNull(list1.find(12));
        list1.insert(12, "twelve");
        assertNotNull(list1.find(12));
        list1.remove(12);
        list1.remove(13);
        assertNull(list1.find(12));
        assertNull(list1.find(13));
        assertNotNull(list1.find(1));
    }

    /**
     * Tests remove(k) on the first node in the list
     */
    public void testRemoveKeyFirst() {
        list1.remove(1);
        assertNull(list1.find(1));
        assertNotNull(list1.find(2));
    }

    /**
     * Tests remove(e) to make sure that it doesn't 
     * effect nodes before that node that is removed
     */
    public void testRemoveKeyAfterLateRemove() {
        assertNotNull(list1.find(4));
        list1.remove(5);
        assertNotNull(list1.find(4));
    }

    /**
     * Tests remove(k) on a list with one node
     */
    public void testRemoveKeyOneNode() {
        emptyList.insert(1, "one");
        emptyList.remove(1);
        assertNull(emptyList.find(1));
        assertNull(emptyList.remove(1));
    }

    /**
     * Tests remove(e)
     */
    public void testRemoveValue() {
        String nullString = null;
        assertNull(list1.remove(nullString));
        assertNull(list1.remove(25));
        list1.remove("three");
        assertNull(list1.find("three"));
        list1.remove("twelve");
        assertNull(list1.find("twelve"));
        list1.insert(12, "twelve");
        assertNotNull(list1.find("twelve"));
        assertNotNull(list1.find("one"));
        assertNotNull(list1.find("two"));

    }

    /**
     * Tests remove(e) on the first node in the list
     */
    public void testRemoveValueFirst() {
        list1.remove("one");
        assertNull(list1.find("one"));
    }

    /**
     * Tests remove(e) on a list with one node
     */
    public void testRemoveValueOneNode() {
        emptyList.insert(1, "one");
        emptyList.remove("one");
        assertNull(emptyList.find("one"));
        assertNull(emptyList.remove("one"));
        assertNotNull(list1.find("two"));
    }

    /**
     * Tests remove(e) on an empty list
     */
    public void testRemoveValueEmptyList() {
        assertNull(emptyList.remove("one"));
    }

    /**
     * Tests remove(e) when removing a node with 
     * the same key and value as another node
     */
    public void testRemoveValueMultiple() {
        list1.insert(5, "five");
        list1.remove("five");
        assertNotNull(list1.find("five"));
    }

    /**
     * Tests toString()
     */
    public void testToString() {
        assertNotNull(list1.find(1).toString());
    }

    /**
     * Tests toArray()
     */
    public void testToArray() {
        assertNull(emptyList.toArray());
        // list1.insert(3, "three 2");
        SkipList<Integer, String>.SkipNode<Integer, String>[] test 
            = list1.toArray();
        
        System.out.println(test.length);
        assertEquals(test.length, 13);
        assertEquals(test[0].key(), 1, 0);
        assertEquals(test[1].key(), 2, 0);
        assertEquals(test[2].key(), 3, 0);
        assertEquals(test[3].key(), 4, 0);
        assertEquals(test[4].key(), 4, 0);
        assertEquals(test[5].key(), 5, 0);
        assertEquals(test[6].key(), 5, 0);
        assertEquals(test[7].key(), 6, 0);
        assertEquals(test[8].key(), 7, 0);
        assertEquals(test[9].key(), 8, 0);
        assertEquals(test[10].key(), 9, 0);
        assertEquals(test[11].key(), 10, 0);
        assertEquals(test[12].key(), 11, 0);
    }

}
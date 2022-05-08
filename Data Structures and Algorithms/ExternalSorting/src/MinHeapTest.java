import student.TestCase;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Random;

/**
 * Tests the methods of the MinHeap class
 * @author Ryan Fasco 906093506
 * @version 1.0
 *
 */
public class MinHeapTest extends TestCase {

    private MinHeap heap;
    private Record record1;
    private Record record2;
    private Record record3;
    private Record record4;
    private Record record5;
    private Record record6;
    private Record record7;
    private Record record8;
    
    /**
     * Sets up the heap and records to be tested
     */
    public void setUp()
    {
        heap = new MinHeap(0, 32);
        byte[] bytes = new byte[16];
        Random rand = new Random();
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record1 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record2 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record3 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record4 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record5 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record6 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record7 = new Record(bytes);
        bytes = new byte[16];
        rand.nextBytes(bytes);
        record8 = new Record(bytes);
    }
    
    /**
     * Tests insert()
     * @throws IOException 
     */
    @SuppressWarnings("resource")
    public void testInsert() throws IOException
    {
        heap.insert(null);
        assertEquals(0, heap.heapsize());
        RandomAccessFile randomFile = 
            new RandomAccessFile("testInput.txt", "r");
        byte[] inputBuffer = new byte[32 * 16];
        randomFile.read(inputBuffer);
        int inputBufferIter = 0;
        
        for (int i = 0; i < 32; ++i)
        {
            
            int recordBufferIter = 0;
            
            byte[] recordBuffer = new byte[16];
            while (recordBufferIter < 16)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++recordBufferIter;
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer);
            heap.insert(r);
            
        }
        assertEquals(32, heap.heapsize());
        heap.insert(record1);
        assertEquals(32, heap.heapsize());
        assertTrue(heap.getAtIndex(0).compareTo(heap.getAtIndex(1)) < 0);
        
        assertTrue(heap.getAtIndex(0).compareTo(heap.getAtIndex(2)) < 0);
        assertTrue(heap.getAtIndex(1).compareTo(heap.getAtIndex(3)) < 0);
        assertTrue(heap.getAtIndex(1).compareTo(heap.getAtIndex(4)) < 0);
        assertTrue(heap.getAtIndex(2).compareTo(heap.getAtIndex(5)) < 0);
        assertTrue(heap.getAtIndex(2).compareTo(heap.getAtIndex(6)) < 0);
    }
    
    /**
     * Tests remove(int) and removemin() on an 
     * empty heap and a full heap
     */
    public void testRemove()
    {
        assertNull(heap.getMin());
        assertNull(heap.removemin());
        assertNull(heap.remove(-1));
        heap.insert(record1);
        heap.insert(record2);
        heap.insert(record3);
        heap.insert(record4);
        heap.insert(record5);
        heap.insert(record6);
        heap.insert(record7);
        assertNull(heap.remove(500));
        heap.insert(record8);
        assertEquals(heap.heapsize(), 8);
        Record min = heap.removemin();
        assertEquals(heap.heapsize(), 7);
        Record six = heap.remove(6);
        assertEquals(heap.heapsize(), 6);
        assertNotSame(min, six);
        assertNull(heap.getAtIndex(500));
        assertNull(heap.getAtIndex(-1));
        assertEquals(heap.getMin(), heap.removemin());
    }
    
    /**
     * Tests insert(), remove(), and insertInactive()
     *  on large amounts of random records
     */
    public void testRemoveRandom()
    {
        MinHeap largeHeap = new MinHeap(0, 1000);
        Random rand = new Random();
        for (int i = 0; i < 1000; i++)
        {
            byte[] bytes = new byte[16];
            rand.nextBytes(bytes);
            Record randRecord = new Record(bytes);
            largeHeap.insert(randRecord);
        }
        for (int i = 1000; i >= 1; i--)
        {
            int index = rand.nextInt(i);
            largeHeap.remove(index);
            byte[] bytes = new byte[16];
            rand.nextBytes(bytes);
            Record randRecord = new Record(bytes);
            largeHeap.insertInactive(randRecord);
        }
        assertEquals(0, largeHeap.heapsize());
        assertTrue(largeHeap.rebuildHeap());
        assertEquals(1000, largeHeap.heapsize());
    }
    
    /**
     * Tests insertInactive() and rebuildHeap()
     * @throws IOException 
     */
    @SuppressWarnings("resource")
    public void testInsertInactive() throws IOException
    {
        
        RandomAccessFile randomFile 
            = new RandomAccessFile("testInput.txt", "r");
        byte[] inputBuffer = new byte[32 * 16];
        randomFile.read(inputBuffer);
        int inputBufferIter = 0;
        
        for (int i = 0; i < 32; ++i)
        {
            
            int recordBufferIter = 0;
            
            byte[] recordBuffer = new byte[16];
            while (recordBufferIter < 16)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++recordBufferIter;
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer);
            heap.insert(r);
            
        }
        inputBufferIter = 0;
        assertFalse(heap.rebuildHeap());
        assertEquals(32, heap.heapsize());
        
        for (int i = 0; i < 32; ++i)
        {
            heap.removemin();
            int recordBufferIter = 0;
            
            byte[] recordBuffer = new byte[16];
            while (recordBufferIter < 16)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++recordBufferIter;
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer);
            assertTrue(heap.insertInactive(r));
            assertEquals(heap.inactiveCount(), i + 1);
            assertEquals(32 - i - 1, heap.heapsize());
            assertTrue(heap.isAtCapacity());
        }
        
        assertTrue(heap.rebuildHeap());
        assertEquals(32, heap.heapsize());
    }
    
    /**
     * Tests rebuilding the heap. Mimics a basic replacement sort
     */
    public void testRebuildHeap()
    {
        assertFalse(heap.rebuildHeap());
        MinHeap largeHeap = new MinHeap(0, 20);
        Random rand = new Random();
        for (int i = 0; i < 20; i++)
        {
            byte[] bytes = new byte[16];
            rand.nextBytes(bytes);
            Record randRecord = new Record(bytes);
            largeHeap.insert(randRecord);
        }
        while (!largeHeap.rebuildHeap())
        {
            Record oldMin = largeHeap.removemin();
            byte[] bytes = new byte[16];
            rand.nextBytes(bytes);
            Record randRecord = new Record(bytes);
            if (randRecord.getKey() < oldMin.getKey())
            {
                largeHeap.insertInactive(randRecord);
            }
            else
            {
                //System.out.println("Inserted");
                largeHeap.insert(randRecord);
            }
        }
        //System.out.println(largeHeap.heapsize());
        assertEquals(largeHeap.heapsize(), 20);
    }
    
    /**
     * Tests partialRebuild()
     * @throws IOException
     */
    public void testPartialRebuild() throws IOException
    {
        RandomAccessFile randomFile 
            = new RandomAccessFile("testInput.txt", "r");
        byte[] inputBuffer = new byte[32 * 16];
        randomFile.read(inputBuffer);
        int inputBufferIter = 0;
        assertFalse(heap.partialRebuildHeap());
        
        for (int i = 0; i < 32; ++i)
        {
            
            int recordBufferIter = 0;
            
            byte[] recordBuffer = new byte[16];
            while (recordBufferIter < 16)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++recordBufferIter;
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer);
            heap.insert(r);
            
        }
        inputBufferIter = 0;
        assertFalse(heap.partialRebuildHeap());
        assertEquals(32, heap.heapsize());
        
        for (int i = 0; i < 32; ++i)
        {
            heap.removemin();
            int recordBufferIter = 0;
            
            byte[] recordBuffer = new byte[16];
            while (recordBufferIter < 16)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++recordBufferIter;
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer);
            assertTrue(heap.insertInactive(r));
            assertEquals(32 - i - 1, heap.heapsize());
            assertTrue(heap.isAtCapacity());
        }
        assertEquals(0, heap.heapsize());
        assertTrue(heap.partialRebuildHeap());
        assertEquals(32, heap.heapsize());
    }
}

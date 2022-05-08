import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import student.TestCase;

/**
 * Will test the ReplacementSort Class
 * @author Ryan Fasco
 * @version 1.0
 */
public class ReplacementSortTest extends TestCase {

    private ReplacementSort test;
    //the size of each record
    private final int recordSize = 16;
    //the size of each block
    private final int blockSize = 512;
    
    /**
     * Sets up for testing
     */
    public void setUp() throws IOException
    {       
        
        File file = new File("testInput.txt");
        RandomAccessFile randomFile = new RandomAccessFile(file, "r");
        
        //initialize the buffers
        byte[] inputBuffer = new byte[blockSize  * recordSize];
        ByteBuffer outputBuffer = ByteBuffer.allocate(blockSize * recordSize);
        
        //initialize the heap
        MinHeap heap = new MinHeap(0, blockSize * 8);
        test = new ReplacementSort(randomFile, heap, inputBuffer, outputBuffer);
        
    }
    
    /**
     * Runs replacement sort on the given input
     * @throws IOException
     */
    public void testSort() throws IOException
    {
        test.preRun();
        assertNotNull(test);
    }
}

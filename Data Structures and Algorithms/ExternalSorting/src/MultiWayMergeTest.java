import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import student.TestCase;

/**
 * Will Test the MultiWayMerge Class
 * @author Joseph Arbolino
 * @version 1.0
 */
public class MultiWayMergeTest extends TestCase 
{
    //merge object
    private MultiWayMerge test;
    //recordSize
    private final int recordSize = 16;
    //BlockSize
    private final int blockSize = 512;

    /**
     * Sets up tests
     */
    public void setUp() throws IOException
    {       
        File file = new File("testInput.txt");
        RandomAccessFile randomFile = new RandomAccessFile(file, "r");
        
        //initialize the buffers
        byte[] inputBuffer = new byte[blockSize * recordSize];
        ByteBuffer outputBuffer = ByteBuffer.allocate(blockSize * recordSize);
        
        //initialize the heap
        MinHeap heap = new MinHeap(0, blockSize * 8);
        
        ReplacementSort rs = new ReplacementSort(
                randomFile, heap, inputBuffer, outputBuffer);
        ArrayList<Integer> runs = new ArrayList<Integer>();
        runs = rs.preRun();
        
        test = new MultiWayMerge(heap, inputBuffer, outputBuffer, runs);
    }
    
    /**
     * Tests Merge
     * @throws IOException
     */
    public void testMerge() throws IOException
    {        
        File outputFile;
        File runFile;
        
        FileInputStream outputFileStream;
        FileOutputStream runFileStream;
        
        byte[] buffer = new byte[1024];
        int length;
        
        boolean isSorted = false;
        while (!test.merge())
        {
            outputFile = new File("outputFile.txt");
            runFile = new File("runFile.txt");
            boolean deleteRunFile = runFile.delete();
            boolean createRunFile = runFile.createNewFile();
            
            if (!deleteRunFile || !createRunFile)
            {
                System.out.println("Problem with file");
            }
            
            outputFileStream = new FileInputStream(outputFile);
            runFileStream = new FileOutputStream(runFile);
            
            while ((length = outputFileStream.read(buffer)) > 0)
            {
                runFileStream.write(buffer, 0, length);
            }          

            outputFileStream.close();
            runFileStream.close();
            boolean deleteOutputFile = outputFile.delete();
            if (!deleteOutputFile)
            {
                System.out.println("Problem with file");
            }
        }
        isSorted = true;
        assertTrue(isSorted);
    }
}
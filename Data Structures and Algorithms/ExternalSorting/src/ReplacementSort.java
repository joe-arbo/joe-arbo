import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.RandomAccessFile;
import java.io.IOException;
import java.util.ArrayList;
import java.nio.ByteBuffer;

/**
 * Handles the Replacement Sort and run generation for heap
 * @author Ryan Fasco
 * @version 1.0
 */
public class ReplacementSort 
{
    //constant record size
    private static final int RECORDSIZE = 16;
    //constant block size
    private static final int BLOCKSIZE = 512;
    
    private RandomAccessFile randomFile;
    private FileWriter runFileWriter;
    private FileOutputStream runFileOutput;
    
    private MinHeap heap;
    private byte[] inputBuffer;
    private ByteBuffer outputBuffer;
    
    //keeps track of the length of each run
    private ArrayList<Integer> runs;
    
    private int inputIndex;
    private int outputIndex;
    private boolean emptyFile;
    
    /**
     * Default Constructor
     * @param randomFile the random access version of file
     * @param h the heap used for sorting
     * @param ib the input buffer as an array
     * @param ob the output buffer as a ByteBuffer
     */
    @SuppressWarnings("null")
    public ReplacementSort(RandomAccessFile randomFile, MinHeap h, 
        byte[] ib, ByteBuffer ob) throws IOException
    {
        this.randomFile = randomFile;
        File runFile = new File("runFile.txt");
        boolean fileError = runFile.createNewFile();
        if (!fileError)
        {
            System.out.println("Error with file");
        }
        runFileWriter = new FileWriter("runFile.txt");
        runFileOutput = new FileOutputStream("runFile.txt");
        
        //initialize the buffers
        inputBuffer = ib;
        outputBuffer = ob;
        
        //keeps track of the positions of all runs
        runs = new ArrayList<Integer>();        
        
        //initialize the heap
        heap = h;
        
        emptyFile = false;
        
        //load all 8 blocks into memory
        for (int i = 0; i < 8; ++i)
        {
            //collect bytes of 1 block into inputBuffer
            
            if (randomFile.read(inputBuffer) == -1)//readFully
            {
                emptyFile = true;
                break; //does break here successfully
            }
            //iterate through all bytes in the inputBuffer
            int inputBufferIter = 0;
            int recordBufferIter = 0;
            
            for (int j = 0; j < BLOCKSIZE; j++)
            {
                byte[] recordBuffer = new byte[RECORDSIZE];
                while (recordBufferIter < RECORDSIZE)
                {
                    recordBuffer[recordBufferIter] = 
                        inputBuffer[inputBufferIter];
                    ++recordBufferIter;
                    ++inputBufferIter;
                }
                recordBufferIter = 0;
                Record r = new Record(recordBuffer);
                heap.insert(r);
            }
        }
        //now 8 Blocks of Records are in the Heap
    }
    
    
    /**
     * Executes a preRun setup that calls the run method and monitors the 
     * input and output buffers
     * @throws IOException
     * @return an ArrayList containing the lengths of each run
     */
    public ArrayList<Integer> preRun() throws IOException
    {
        if (!emptyFile)
        {
            if (!getInputBuffer())
            {
                emptyFile = true;
            }
        }
        outputIndex = 0;
        inputIndex = 0;
        boolean fullFile = true;
        while (fullFile) //handles runs
        {
            this.run();
            if (inputIndex == inputBuffer.length)
            {
                inputIndex = 0;
                if (!getInputBuffer())
                {
                    inputIndex = 0;
                }
            }
            heap.rebuildHeap();
            if (heap.heapsize() == 0)
            {
                if (heap.inactiveCount() > 0)
                {
                    heap.partialRebuildHeap();
                }
                else
                {
                    fullFile = false;
                }
            }                
        }
        runFileWriter.close();
        runFileOutput.close();
        return runs;
    }

    
    /**
     * Executes a run. Adds records from the heap into the output 
     * buffer and adds records from the input
     * buffer into the heap. Loads more bytes into the input buffer, 
     * outputs the output buffer, and rebuilds the heap when necessary
     * Precondition: the input buffer and heap are filled
     * Postcondition: there is no value remaining in the heap 
     * which is less than the last value in the output buffer
     * @throws IOException 
     */
    public void run() throws IOException
    {
        int runCounter = 0;

        byte[] recordArray = new byte[RECORDSIZE];
        Record newRecord = new Record(recordArray);
        
        //since an empty heap means that every inactive
        //record is less than the last output record
        while (!heap.rebuildHeap() 
            && heap.heapsize() != 0) 
        { 
            //array to get bytes for next record            
            //check for heap size, only add if it's full
            if (heap.isAtCapacity() && !emptyFile)
            { //only at capacity at 8 blocks
                recordArray = new byte[RECORDSIZE];
                for (int i = 0; i < RECORDSIZE; i++)
                {
                    //creates a new record from input buffer
                    recordArray[i] = inputBuffer[inputIndex];
                    inputIndex++;
                }
                
                newRecord = new Record(recordArray);
            }
            Record outputRecord;
                
            if (heap.isAtCapacity() && !emptyFile)
            {

                outputRecord = new Record(heap.removemin().getCompleteRecord());
                if (newRecord.getKey() < outputRecord.getKey())
                {
                    heap.insertInactive(newRecord);
                }
                else
                {
                    heap.insert(newRecord);
                }
            }
            else
            {
                outputRecord = new Record(heap.removemin().getCompleteRecord());
            }
            outputBuffer.put(outputRecord.getCompleteRecord());
            runCounter++;
            outputIndex++;
            if (outputIndex == BLOCKSIZE)
            {
                writeOutputBuffer();
                outputIndex = 0;
                outputBuffer.clear();
            }

            if (inputIndex == BLOCKSIZE * RECORDSIZE)
            {
                if (!getInputBuffer())
                {
                    emptyFile = true;
                }
                else
                {
                    inputIndex = 0;
                }  
            }
        }
        
        runs.add(runCounter);
    }
    
    /**
     * Writes the outputBuffer to a runFile
     */
    public void writeOutputBuffer() throws IOException
    {        
        runFileOutput.write(outputBuffer.array());
    }


    
    /**
     * Reads the inputBuffer data
     * @return true if read, false otherwise
     * @throws IOException
     */
    public boolean getInputBuffer() throws IOException
    {
        return (randomFile.read(inputBuffer) != -1);
    }
}

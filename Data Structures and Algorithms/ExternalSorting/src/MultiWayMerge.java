import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
//import java.io.FileWriter;
import java.io.IOException;

/**
 * Will Merge the runFile runs into one sorted file
 * @author Joseph Arbolino
 * @version 1.0
 */
public class MultiWayMerge 
{
    //record size
    private static final int RECORDSIZE = 16;
    //block size
    private static final int BLOCKSIZE = 512;
    
    //data members
    private MinHeap heap;
    private byte[] inputBuffer;
    private ByteBuffer outputBuffer;
    
    //files
    private RandomAccessFile randomFile;
    private FileOutputStream outputFileOutput;
    

    //runs arrayList
    private ArrayList<Integer> runs;
    
    //array for keeping track of which runs blocks in the heap
    private int[] runRecordsInHeap;
    private int[] runPointers;
    private int[] originalRunPointers;
    private int runRecordTotal = 0;
    
    /**
     * Constructor for Merge
     * @param h the heap
     * @param ib the inputBuffer
     * @param ob the outputBuffer
     * @param r the runs ArrayList
     * @throws IOException
     */
    public MultiWayMerge(MinHeap h, byte[] ib, ByteBuffer ob, 
            ArrayList<Integer> r) throws IOException
    {
        heap = h;
        inputBuffer = ib;
        outputBuffer = ob;
        outputBuffer.clear();  
        
        runs = r;
    }
    
    /**
     * Adds a block to the heap
     * @param run the current run
     * @throws IOException
     */
    public void addBlockToHeap(int run) throws IOException
    {
        //find and load a block from each run into the heap
        //move to the pointer position
        randomFile.seek(runPointers[run] * RECORDSIZE);    
   
        //check how much data is available
        int dataAvailable = 0;
        
        //last run
        if (run == runs.size() - 1)
        {
            if (runPointers[run] + BLOCKSIZE > runRecordTotal)
            {
                dataAvailable = runRecordTotal - runPointers[run];
            }
            else
            {
                dataAvailable = BLOCKSIZE;
            }
        }
        else
        {
            if (runPointers[run] + BLOCKSIZE > originalRunPointers[run + 1])
            {
                dataAvailable = originalRunPointers[run + 1] 
                        - runPointers[run];
            }
            else
            {
                dataAvailable = BLOCKSIZE;
            }
        }
        
        //advance pointer
        runPointers[run] += dataAvailable;
        randomFile.read(inputBuffer);

        int inputBufferIter = 0;
        for (int j = 0; j < dataAvailable; ++j)
        {
            byte[] recordBuffer = new byte[RECORDSIZE];
            for (int recordBufferIter = 0; 
                    recordBufferIter < RECORDSIZE; 
                    ++recordBufferIter)
            {
                recordBuffer[recordBufferIter] = inputBuffer[inputBufferIter];
                ++inputBufferIter;
            }
            Record r = new Record(recordBuffer, run);
            heap.insert(r);
            
            //add 1 to this runs block counter in heap
            runRecordsInHeap[run] = runRecordsInHeap[run] + 1;
        }
    }

    /**
     * Will merge the runs
     * @return true if sorted false otherwise
     * @throws IOException
     */
    public boolean merge() throws IOException
    {
        //clear file
        File outputFile;
        outputFile = new File("outputFile.txt");
        boolean createOutputFile = outputFile.createNewFile();
        if (!createOutputFile)
        {
            System.out.println("Could not create File");
        }
   
        outputFileOutput = new FileOutputStream("outputFile.txt");
        
        inputBuffer = new byte[BLOCKSIZE * RECORDSIZE];
        heap = new MinHeap(0, BLOCKSIZE * 8);
        
        File file = new File("runFile.txt");
        try 
        {
            randomFile = new RandomAccessFile(file, "r");
        } 
        catch (FileNotFoundException e) 
        {
            e.printStackTrace();
            System.exit(0);
        }     
        
        //Find the number of runs and passes needed
        int numRuns = runs.size();
        int numPasses = 0;
        //numRuns = 20;
        ArrayList<Integer> runsPerPass = new ArrayList<Integer>(); 
        
        while (numRuns > 0)
        {
            numRuns = numRuns - 8;
            if (numRuns < 0)
            {
                runsPerPass.add(numRuns + 8);
            }
            else
            {
                runsPerPass.add(8);
            }
            numPasses++;
        }
        numRuns = runs.size();
       
        //calculate and store all run pointers
        runRecordsInHeap = new int[runs.size()];
        runPointers = new int[runs.size()];
        originalRunPointers = new int[runs.size()];
        
        //newRuns ArrayList for multiple passes
        ArrayList<Integer> newRuns = new ArrayList<Integer>();
        
        int runPointerTotal = 0;
        for (int i = 0; i < runs.size(); ++i)
        {
            runPointerTotal += runs.get(i);
        }
        runRecordTotal = runPointerTotal;
        //adjust for correct pointer value
        for (int i = runs.size() - 1; i >= 0; i--)
        { 
            runPointerTotal = runPointerTotal - runs.get(i);
            runPointers[i] = runPointerTotal;
            originalRunPointers[i] = runPointerTotal;            
        }
        
        int recordsPerPass = 0;   
        //cycle through all of the passes
        for (int pass = 0; pass < numPasses; pass++)
        {
            //cycle through all the runsPerPass (runs)
            for (int i = 0; i < runsPerPass.get(pass); i++)
            {
                addBlockToHeap(i + (pass * 8));
            }

            //now the first block of each run is in the heap
            //now sort the heap and remove the minimum
            while (heap.heapsize() > 0)
            {         
                //move one block at a time to the outputBuffer 
                int outputIndex = 0;
                outputBuffer.clear();
                for (int outputBufferIter = 0; 
                        outputBufferIter < BLOCKSIZE; ++outputBufferIter)
                {
                    if (heap.heapsize() == 0)
                    {
                        break;
                    }
                    
                    Record r = heap.removemin();
                    outputBuffer.put(r.getCompleteRecord());
                    outputIndex++;
                    recordsPerPass++;
                    
                    //decrement which run the record came from
                    runRecordsInHeap[r.getRun()] = 
                            runRecordsInHeap[r.getRun()] - 1;
    
                    if (runRecordsInHeap[r.getRun()] == 0)
                    {
                        addBlockToHeap(r.getRun());
                    }
                }
                
                //now write the outputBuffer to the file
                writeOutputBuffer(outputIndex);
            }
            
            //adjust newRuns array list   
            newRuns.add(recordsPerPass);
            recordsPerPass = 0; 
        }
        
        outputFileOutput.close();
        
        if (numPasses > 1)
        {
            runs = newRuns;
            return false;
        }  
        return true;
    }
    

    /**
     * Writes the outputBuffer to a runFile
     * @param outputIndex the outputIndex
     * @throws IOException
     */
    public void writeOutputBuffer(int outputIndex) throws IOException
    {     
        //print in bytes
        outputFileOutput.write(outputBuffer.array());     
    }
}

import java.io.File;
import java.io.FileInputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

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
 * The class containing the main method.
 *
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */

public class Externalsort 
{
    //Record Size
    private static final int RECORDSIZE = 16;
    //Block Size
    private static final int BLOCKSIZE = 512;
    
    static private MinHeap heap;
    static private byte[] inputBuffer;
    static private ByteBuffer outputBuffer;
    
    /**
     * Default Constructor
     */
    public Externalsort()
    {
        //does nothing
    }
    
    /**
     * @param args
     *     Command line parameters
     * @throws IOException 
     */
    public static void main(String[] args) throws IOException
    {
        inputBuffer = new byte[BLOCKSIZE * RECORDSIZE];
        outputBuffer = ByteBuffer.allocate(BLOCKSIZE * RECORDSIZE);
        
        //initialize the heap
        heap = new MinHeap(0, BLOCKSIZE * 8);
        
        if (args.length != 1)
        {
            System.out.println("Incorrect Arguments");
            return;
        }
        File file = new File(args[0]);
        RandomAccessFile randomFile;
        try 
        {
            randomFile = new RandomAccessFile(file, "r");
        }
        catch (FileNotFoundException e)
        {
            System.out.println("Bad Input to RandomAccessFile");
            return;
        }
        
        //Create a ReplacementSort Object
        ReplacementSort rs;
        ArrayList<Integer> runs = new ArrayList<Integer>();
        try
        {
            rs = new ReplacementSort(
                    randomFile, heap, inputBuffer, outputBuffer);
            runs = rs.preRun();
        }
        catch (IOException e)
        {
            e.printStackTrace();
            System.exit(0);
        }
        
        //Start MultiWayMerge
        MultiWayMerge merger = new MultiWayMerge(
                heap, inputBuffer, outputBuffer, runs);

        File outputFile;
        File runFile;
        FileInputStream outputFileStream;
        FileOutputStream runFileStream;
        byte[] buffer = new byte[1024];
        int length;
        
        while (!merger.merge())
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
            
            //recordOutputFile.delete();
        }
        
        //copy data to outputFile
        outputFile = new File("outputFile.txt");
        boolean deleteFile = file.delete();
        boolean createFile = file.createNewFile();
        if (!deleteFile || !createFile)
        {
            System.out.println("Problem with file");
        }
        
        outputFileStream = new FileInputStream(outputFile);
        FileOutputStream fileStream = new FileOutputStream(file);
      
        while ((length = outputFileStream.read(buffer)) > 0)
        {
            fileStream.write(buffer, 0, length);
        }
      
        outputFileStream.close();
        fileStream.close();
        boolean deleteOutputFile = outputFile.delete();
        if (!deleteOutputFile)
        {
            System.out.println("Problem with file");
        }
        runFile = new File("runFile.txt");
        boolean deleteRunFile = runFile.delete();
        if (!deleteRunFile) 
        {
            System.out.println("Problem with file");
        }
        
        file = new File(args[0]);
        randomFile = new RandomAccessFile(file, "r");
        
        //Output
        int totalBytes = 0;
        //gets the total amount of blocks
        for (int i = 0; i < runs.size(); i++)
        {
            totalBytes = totalBytes + runs.get(i);
        }
        int blocks = totalBytes / BLOCKSIZE;
        
        int lineCounter = 0;
        for (int i = 0; i < blocks; i++)
        {
            randomFile.seek(BLOCKSIZE * i * RECORDSIZE);
            byte[] minBytes = new byte[RECORDSIZE];
            randomFile.read(minBytes);
            Record blockMin = new Record(minBytes);
            lineCounter++;
            if (lineCounter == 5)
            {
                lineCounter = 0;
                System.out.println(blockMin.getId() 
                        + " " + blockMin.getKey());
            }
            else
            {
                System.out.print(blockMin.getId() 
                        + " " + blockMin.getKey() + " ");
            }
            
        }
        randomFile.close();
    }
}

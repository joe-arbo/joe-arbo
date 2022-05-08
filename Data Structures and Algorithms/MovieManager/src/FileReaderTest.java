import java.io.FileNotFoundException;
import student.TestCase;

/**
 * Tests the methods of the FileReader class
 * @author Ryan Fasco 906093506
 * @version 2021-09-23
 *
 */
public class FileReaderTest extends TestCase {

    //The main FileReader object used for testing
    private FileReader reader;
    /**
     * Sets up the FileReader used for testing
     */
    public void setUp()
    {
        reader = new FileReader();
    }
    
    /**
     * Tests FileReader with an invalid file
     */
    public void testInvalidFile()
    {
        Exception e = null;
        try {
            reader.readFile("false");
        }
        catch (Exception exception) {
            e = exception;
        }
        assertTrue(e instanceof FileNotFoundException);
    }
    
    /**
     * Tests FileReader with a valid file. Tests each
     * line to make sure that it is read correctly
     * @throws FileNotFoundException when an invalid file
     *         is passed into the method
     */
    public void testFile() throws FileNotFoundException
    {
        String[] commands = reader.readFile("sampleInput.txt");
        System.out.println(commands.length);
        for (int i = 0; i < commands.length; i++)
        {
            System.out.println(commands[i]);
        }
        assertEquals(commands.length, 27);
        assertEquals(commands[0], "print           hashtable");
        assertEquals(commands[24], "add death note");
    }
    
    /**
     * Test FileReader with a null value
     * @throws FileNotFoundException when an invalid file
     *         is passed into the method
     */
    public void testNullFile() throws FileNotFoundException
    {
        assertNull(reader.readFile(null));
    }
    
    
}
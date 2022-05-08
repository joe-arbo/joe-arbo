import java.io.FileNotFoundException;

import student.TestCase;

/**
 * Test the main function (you should throw this away for your project and write
 * your own tests)
 * 
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */
public class MemManTest extends TestCase {
    
    /**
     * Sets up the tests that follow. In general, used for initialization
     */
    public void setUp() {
        // Nothing Here
    }


    /**
     * Get code coverage of the class declaration.
     * @throws FileNotFoundException 
     */
    public void testRInit() throws FileNotFoundException {
        MemMan manager = new MemMan();
        assertNotNull(manager);
//        String[] args = {"32", "100", "testInput.txt"};
        String[] args = {"32", "10", "sampleInput.txt"};
//        String[] args = {"512", "17", "simpleHash.txt"};
//        String[] args = {"64", "17", "simpleMemory.txt"};
        MemMan.main(args);
    }
}

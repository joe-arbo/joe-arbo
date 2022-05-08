import java.io.IOException;
import student.TestCase;

/**
 * @author {Your Name Here}
 * @version {Put Something Here}
 */
public class ExternalsortTest extends TestCase {
    
    
    /**
     * set up for tests
     */
    public void setUp() {
        //nothing to set up.
    }
    
    /**
     * Get code coverage of the class declaration.
     * @throws IOException 
     */
    public void testExternalsortInit() throws IOException {
        Externalsort sorter = new Externalsort();
        assertNotNull(sorter);
        String[] args = new String[1];
        args[0] = "testInput.txt";
        Externalsort.main(args);
    }
}

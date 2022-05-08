import student.TestCase;

/**
 * Tests the methods of the Record class
 * @author Ryan Fasco
 * @version 1.0
 *
 */
public class RecordTest extends TestCase {

    private Record record;
    /**
     * Sets up the methods to be tested
     */
    public void setUp()
    {
        record = new Record("Death Note");
        KVPair<String, String> format = 
            new KVPair<String, String>("Format", "Series");
        record.addField(format);
        KVPair<String, String> genre = 
            new KVPair<String, String>("Genre", "Anime");
        record.addField(genre);
    }
    
    /**
     * Tests toString
     */
    public void testToString()
    {
        assertEquals(record.toString(), "Death "
            + "Note<SEP>Format<SEP>Series<SEP>Genre<SEP>Anime");
        KVPair<String, String> tv = new KVPair<String, String>("TV", "");
        record.addField(tv);
        assertEquals(record.toString(), "Death "
            + "Note<SEP>Format<SEP>Series<SEP>Genre<SEP>Anime<SEP>TV");
    }
    
    /**
     * Tests getFields
     */
    public void testGetFields()
    {
        assertNotNull(record.getFields());
    }
    
    /**
     * Tests addField with a null value
     */
    public void testAddFieldNull()
    {
        record.addField(null);
        assertEquals(record.getFields().size(), 2);
    }
}

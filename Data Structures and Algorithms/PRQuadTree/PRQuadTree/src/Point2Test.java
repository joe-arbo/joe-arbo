/**
 * Test file for Point2
 */
import java.io.FileNotFoundException;

import student.TestCase;

/**
 * Will Test Point2
 * @author Joseph Arbolino
 * @version 1.0
 */
public class Point2Test extends TestCase
{
    //point2 object to be used for testing
    private Point2 p2;
    
    /**
     * Sets up p2 to be tested
     */
    public void setUp()
    {
        p2 = new Point2();
    }
    
    /**
     * Tests main
     * 
     * @author Ryan Fasco
     * @throws FileNotFoundException
     */
    public void testMain() throws FileNotFoundException 
    {
        System.out.println("\ntestMain():");
        
        String[] args = new String[1];
        args[0] = "P2test1.txt";
        Point2.main(args);
        assertNotNull(p2.dump());

        System.out.println();
        
        String[] args2 = new String[1];
        args2[0] = "P2test2.txt";
        Point2.main(args2);
        assertNotNull(p2.dump());
    }    
    
    /**
     * Tests the Insert method
     */
    public void testInsert()
    {
        System.out.println("\ntestInsert():");
        //accepted
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 10));
        assertTrue(p2.insert("p2", 100, 200));
        assertTrue(p2.insert("p3", 10, 10));
        
        //rejected
        assertFalse(p2.insert("p4", 2000, 100));
        assertFalse(p2.insert("p5", 10, 2000));
        assertFalse(p2.insert("p1", 0, 0));
    }
    
    /**
     * Tests both remove methods
     */
    public void testRemove()
    {
        System.out.println("\ntestRemove():");
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 100));
        assertTrue(p2.insert("p2", 100, 200));
        assertTrue(p2.insert("p3", 10, 10));
        
        //accepted
        assertTrue(p2.remove(10, 10));
        assertTrue(p2.remove("p2"));
        assertTrue(p2.remove("p1"));
        assertTrue(p2.remove("p2"));
        
        //rejected
        assertFalse(p2.remove("p5"));
        assertFalse(p2.remove(100, 100));
        assertFalse(p2.remove(100, 2000));
        assertFalse(p2.remove(2000, 10));
        
        Point2 p22 = new Point2();
        
        assertTrue(p22.insert("p2", 10, 10));
        assertTrue(p22.insert("p3", 10, 10));
        assertTrue(p22.insert("p4", 10, 10));
        assertTrue(p22.insert("p5", 10, 10));
        
        System.out.println();
        p22.dump();
        System.out.println();    
        
        assertTrue(p22.remove(10, 10));
        //assertTrue(p22.remove("p2"));
        
        System.out.println();
        p22.dump();
        System.out.println();     
    }
    
    /**
     * Tests regionsearch method
     */
    public void testRegionSearch()
    {
        System.out.println("\ntestRegionSearch():");
        
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 10));
        assertTrue(p2.insert("p2", 1000, 514));
        assertTrue(p2.insert("p3", 10, 10));
        
        System.out.println();
        p2.dump();
        System.out.println();
        
        //accepted
        assertTrue(p2.regionsearch(0, 0, 100, 100));
        assertTrue(p2.regionsearch(900, 300, 120, 300));
        
        //rejected
        assertFalse(p2.regionsearch(0, 0, -10, 100));
        assertFalse(p2.regionsearch(0, 0, 100, -10));
    }
    
    /**
     * Tests duplicates method
     */
    public void testDuplicates()
    {
        System.out.println("\ntestDuplicates():");
        
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 10));
        assertTrue(p2.insert("p2", 1000, 514));
        assertTrue(p2.insert("p3", 10, 10));
        assertTrue(p2.insert("p4", 10, 10));
        assertTrue(p2.insert("p5", 100, 100));
        assertTrue(p2.insert("p6", 100, 100));
        
        //accepted
        assertTrue(p2.duplicates());
    }
    
    /**
     * Tests search method
     */
    public void testSearch()
    {
        System.out.println("\ntestSearch():");
        
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 10));
        assertTrue(p2.insert("p2", 1000, 514));
        assertTrue(p2.insert("p3", 10, 10));
        
        //accepted
        assertTrue(p2.search("p1"));
        assertTrue(p2.search("p2"));
        assertTrue(p2.search("p3"));
        
        //rejected
        assertFalse(p2.search("p5"));
        assertFalse(p2.search("random"));
    }
    
    /**
     * Tests dump method
     */
    public void testDump()
    {
        System.out.println("\ntestDump():");
        assertFalse(p2.dump());
        
        assertTrue(p2.insert("p1", 0, 0));
        assertTrue(p2.insert("p2", 10, 10));
        assertTrue(p2.insert("p2", 1000, 514));
        assertTrue(p2.insert("p3", 10, 10));
        
        assertTrue(p2.dump());
    }
}

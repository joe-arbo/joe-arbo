import student.TestCase;
import java.util.ArrayList;
import java.util.Random;

/**
 * Tests the methods of PRQuadTree
 * @author Ryan Fasco 906093506
 * @version 1.0
 */
public class PRQuadTreeTest extends TestCase {

    private PRQuadTree tree;
    /**
     * Sets up the tree to be tested
     */
    public void setUp()
    {
        tree = new PRQuadTree(1024);
    }
    
    /**
     * Tests Insert with a group of points close together
     */
    public void testInsertSmall()
    {
        Point point = new Point(2, 4, "Name");
        Point point2 = new Point(3, 4, "Name");
        Point point3 = new Point(4, 4, "Name");
        Point point4 = new Point(5, 4, "Name");
        Point point5 = new Point(2, 7, "Name");
        Point point6 = new Point(2, 9, "Name");
        Point point7 = new Point(5, 4, "Name");
        Point point8 = new Point(2, 9, "Name");
        tree.insert(point);
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        assertTrue(tree.search(point));
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        assertTrue(tree.search(point));
        assertTrue(tree.search(point2));
        assertTrue(tree.search(point3));
        assertTrue(tree.search(point4));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point6));
        assertTrue(tree.search(point7));
        assertTrue(tree.search(point8));
    }
    
    /**
     * Tests Insert with 2 sets of points close together
     * in 2 separate quadrants
     */
    public void testInsert2Regions()
    {
        Point point = new Point(2, 4, "Name");
        Point point2 = new Point(3, 4, "Name");
        Point point3 = new Point(4, 4, "Name");
        Point point4 = new Point(5, 4, "Name");
        Point point5 = new Point(2, 7, "Name");
        Point point6 = new Point(2, 9, "Name");
        Point point7 = new Point(5, 4, "Name");
        Point point8 = new Point(2, 9, "Name");
        
        Point point9 = new Point(800, 7, "Name");
        Point point10 = new Point(800, 9, "Name");
        Point point11 = new Point(800, 4, "Name");
        Point point12 = new Point(800, 9, "Name");
        
        Point point13 = new Point(800, 127, "Name");
        
        Point point14 = new Point(800, 805, "Name");
        Point point15 = new Point(800, 810, "Name");
        Point point16 = new Point(800, 807, "Name");
        Point point17 = new Point(800, 809, "Name");
        
        tree.insert(point);
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        assertTrue(tree.search(point));
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        
        tree.insert(point9);
        tree.insert(point10);
        tree.insert(point11);
        tree.insert(point12);
        
        tree.insert(point13);
        
        tree.insert(point14);
        tree.insert(point15);
        tree.insert(point16);
        tree.insert(point17);
        
        assertTrue(tree.search(point));
        assertTrue(tree.search(point2));
        assertTrue(tree.search(point3));
        assertTrue(tree.search(point4));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point6));
        assertTrue(tree.search(point7));
        assertTrue(tree.search(point8));
    }
    
    /**
     * Tests Insert with points in 4 separate quadrants
     */
    public void testInsert()
    {
        Point point = new Point(2, 4, "Name");
        
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(2, 300, "Name");
        Point point6 = new Point(300, 7, "Name");
        Point point7 = new Point(300, 300, "Name");
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert( point6);
        tree.insert(point7);
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point4));
    }
    
    /**
     * Tests insert() with multiple levels and splits
     */
    public void testInsert3()
    {
        Point point = new Point(2, 4, "Name");
        
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(2, 300, "Name");
        Point point6 = new Point(300, 7, "Name");
        Point point7 = new Point(300, 300, "Name");
        Point point8 = new Point(300, 8, "Name");
        Point point9 = new Point(300, 9, "Name");
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        tree.insert(point9);
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point4));
    }
    
    
    
    /**
     * Tests insert() with duplicate values
     */
    public void testInsertDuplicates()
    {
        Point point = new Point(2, 4, "Name");
        
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(2, 300, "Name");
        Point point6 = new Point(300, 7, "Name");
        Point point7 = new Point(300, 7, "Name");
        Point point8 = new Point(300, 7, "Name");
        Point point9 = new Point(200, 8, "Name");
        Point point10 = new Point(200, 9, "Name");
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        tree.insert(point9);
        tree.insert(point10);
        assertTrue(tree.search(point4));
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point2));
        assertTrue(tree.search(point3));
        assertTrue(tree.search(point6));
        assertTrue(tree.search(point7));
        assertTrue(tree.search(point8));
        assertTrue(tree.search(point9));
        assertTrue(tree.search(point10));
    }
    
    /**
     * Tests insert() with cascading node
     * decomposition
     */
    public void testCascadingSplit()
    {
        Point point = new Point(125, 4, "Name");
        
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(150, 5, "Name");
        Point point6 = new Point(175, 7, "Name");
        Point point7 = new Point(180, 7, "Name");
        Point point8 = new Point(300, 7, "Name");
        Point point9 = new Point(200, 8, "Name");
        Point point10 = new Point(200, 9, "Name");
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        tree.insert(point9);
        tree.insert(point10);
        assertTrue(tree.search(point4));
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point2));
        assertTrue(tree.search(point3));
        assertTrue(tree.search(point6));
        assertTrue(tree.search(point7));
        assertTrue(tree.search(point8));
        assertTrue(tree.search(point9));
        assertTrue(tree.search(point10));
    }
    
    /**
     * Tests insert() with cascading decomposition
     * in multiple regions
     */
    public void testCascadingSplitRegion2()
    {
        Point point = new Point(125, 4, "Name");
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(150, 5, "Name");
        Point point6 = new Point(175, 7, "Name");
        Point point7 = new Point(180, 355, "Name");
        Point point8 = new Point(300, 7, "Name");
        Point point9 = new Point(200, 8, "Name");
        Point point10 = new Point(200, 9, "Name");
        Point point11 = new Point(800, 500, "Name");
        Point point12 = new Point(800, 550, "Name");
        Point point13 = new Point(800, 580, "Name");
        Point point14 = new Point(800, 600, "Name");
        Point point15 = new Point(800, 630, "Name");
        Point point16 = new Point(203, 780, "Name");
        Point point17 = new Point(158, 1000, "Name");
        Point point18 = new Point(780, 130, "Name");
        Point point19 = new Point(130, 780, "Name");
        Point point20 = new Point(75, 904, "Name");
        Point point21 = new Point(875, 255, "Name");
        Point point22 = new Point(456, 150, "Name");
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        tree.insert(point9);
        tree.insert(point11);
        tree.insert(point12);
        tree.insert(point13);
        tree.insert(point14);
        tree.insert(point15);
        tree.insert(point16);
        tree.insert(point17);
        tree.insert(point18);
        tree.insert(point19);
        tree.insert(point20);
        tree.insert(point21);
        tree.insert(point22);
        tree.insert(point10);
        assertTrue(tree.search(point4));
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertTrue(tree.search(point2));
        assertTrue(tree.search(point3));
        assertTrue(tree.search(point6));
        assertTrue(tree.search(point7));
        assertTrue(tree.search(point8));
        assertTrue(tree.search(point9));
        assertTrue(tree.search(point10));
        assertTrue(tree.search(point11));
        assertTrue(tree.search(point12));
        assertTrue(tree.search(point13));
        assertTrue(tree.search(point14));
        assertTrue(tree.search(point15));
        assertTrue(tree.search(point16));
        assertTrue(tree.search(point17));
        assertTrue(tree.search(point18));
        assertTrue(tree.search(point19));
        assertTrue(tree.search(point20));
        assertTrue(tree.search(point21));
        assertTrue(tree.search(point22));
    }
    
    /**
     * Tests insert() with several invalid points
     */
    public void testInvalidPoint()
    {
        Point point = new Point(-1, 4, "Name");
        Point point2 = new Point(600, -1, "Name");
        Point point3 = new Point(100000, 600, "Name");
        Point point4 = new Point(600, 100000, "Name");
        Point point5 = null;        
        assertFalse(tree.insert(point));
        assertFalse(tree.insert(point2));
        assertFalse(tree.insert(point3));
        assertFalse(tree.insert(point4));
        assertFalse(tree.insert(point5));
        assertFalse(tree.insert(point));
    }
    
    /**
     * Tests insert and remove with 200 random points
     */
    public void testBigInsert()
    {
        Random rand = new Random();
        ArrayList<Point> points = new ArrayList<Point>();
        for (int i = 0; i < 400; i++)
        {
            Point newPoint = new Point(rand.nextInt(1024), rand.nextInt(1024), 
                    String.valueOf(rand.nextInt(1000000)).toString());
            tree.insert(newPoint);
            points.add(newPoint);
        }
        
        for (int i = 0; i < points.size(); i++)
        {
            assertTrue(tree.search(points.get(i)));
        }
        tree.dump();
        
        for (int i = 0; i < points.size(); i++)
        {
            assertTrue(tree.search(points.get(i)));
        }
    }
    
    /**
     * Tests remove()
     */
    public void testRemove()
    {
        Point point = new Point(2, 4, "Name");
        Point point2 = new Point(600, 4, "Name");
        Point point3 = new Point(4, 600, "Name");
        Point point4 = new Point(600, 600, "Name");
        Point point5 = new Point(2, 300, "Name");
        Point point6 = new Point(300, 7, "Name");
        Point point7 = new Point(300, 300, "Name");
        Point point8 = new Point(300, 300, "Name2");
        
        tree.insert(point);
        assertTrue(tree.search(point));
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.dump();
        assertEquals(point4, tree.remove(point4));
        tree.dump();
        assertEquals(tree.remove(point3), point3); 
        
        tree.insert(point5);
        tree.insert(point6);
        tree.insert(point7);
        tree.insert(point8);
        assertEquals(tree.remove(point7).getName(), point7.getName());
        assertNull(tree.remove(point7));
        assertTrue(tree.search(point));
        assertTrue(tree.search(point5));
        assertFalse(tree.search(point4));
        assertFalse(tree.search(point3));
        assertEquals(tree.remove(point6), point6);
        assertFalse(tree.search(point6));
        assertTrue(tree.search(point));
    }
    
    /**
     * Tests remove() with several points close together
     * so that the points will have to merge
     */
    public void testMerge()
    {
        Point point = new Point(2, 4, "Name");
        Point point2 = new Point(3, 4, "Name");
        Point point3 = new Point(4, 4, "Name");
        Point point4 = new Point(5, 4, "Name");
        Point point5 = new Point(2, 7, "Name");
        Point point6 = new Point(2, 9, "Name");
        Point point7 = new Point(9, 4, "Name");
        Point point8 = new Point(2, 11, "Name");
        tree.insert(point);
        tree.insert(point2);
        tree.insert(point3);
        tree.insert(point4);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert( point7);
        tree.insert(point8);
        tree.dump();
        
        assertEquals(tree.remove(point), point);
        assertEquals(tree.remove(point2), point2);
        assertEquals(tree.remove(point3), point3);
        tree.dump();
        assertEquals(tree.remove(point4), point4);
        tree.dump();
        assertEquals(tree.remove(point5), point5);
        tree.dump();
        assertEquals(tree.remove(point6), point6);
        tree.dump();
        assertEquals(tree.remove(point7), point7);
        tree.dump();
        
        assertFalse(tree.search(point));
        assertFalse(tree.search(point2));
        assertFalse(tree.search(point3));
        assertFalse(tree.search(point4));
        assertFalse(tree.search(point5));
        assertFalse(tree.search(point7));
        assertFalse(tree.search(point6));
        assertTrue(tree.search(point8));
        
        assertEquals(tree.remove(point8), point8);
        tree.dump();
    }
    
    /**
     * Tests remove with invalid parameters
     */
    public void testRemoveError()
    {
        Point point = new Point(50, 50, "Name");
        Point point2 = new Point(-1, 50, "Name");
        Point point3 = new Point(50, -1, "Name");
        Point point4 = new Point(100000, 50, "Name");
        Point point5 = new Point(50, 100000, "Name");
        
        assertNull(tree.remove(point));
        assertNull(tree.remove(point2));
        assertNull(tree.remove(point3));
        assertNull(tree.remove(point4));
        assertNull(tree.remove(point5));
        assertNull(tree.remove(null));
        assertNull(tree.remove(point5));
        
        tree.insert(point);
        assertEquals(tree.remove(point), point);
        assertNull(tree.remove(point));
        assertFalse(tree.search(point));
    }
    
    /**
     * Test dump()
     */
    public void testDump()
    {
        tree.dump();
        Point point = new Point(2, 4, "Point1");
        Point point2 = new Point(3, 4, "Point2");
        Point point3 = new Point(4, 4, "Point3");
        Point point4 = new Point(5, 4, "Point4");
        Point point5 = new Point(2, 7, "Point5");
        Point point6 = new Point(2, 9, "Point6");
        Point point7 = new Point(9, 4, "Point7");
        Point point8 = new Point(2, 11, "Point8");
        assertTrue(tree.insert(point));
        assertTrue(tree.insert(point2));
        assertTrue(tree.insert(point3));
        assertTrue(tree.insert(point4));
        assertTrue(tree.insert(point5));
        assertTrue(tree.insert(point6));
        assertTrue(tree.insert(point7));
        assertTrue(tree.insert(point8));
        
        //tree.dump();
    }
    
    /**
     * Test Regionsearch
     */
    public void testRegionSearch()
    {
        Point point = new Point(2, 4, "Point1");
        Point point2 = new Point(3, 4, "Point2");
        Point point3 = new Point(4, 4, "Point3");
        Point point4 = new Point(5, 4, "Point4");
        Point point5 = new Point(2, 7, "Point5");
        Point point6 = new Point(2, 9, "Point6");
        Point point7 = new Point(9, 4, "Point7");
        Point point8 = new Point(2, 11, "Point8");
        assertTrue(tree.insert(point));
        assertTrue(tree.insert(point2));
        assertTrue(tree.insert(point3));
        tree.regionSearch(0, 0, 5, 5);
        tree.insert(point4);
        tree.regionSearch(0, 0, 5, 5);
        tree.insert(point5);
        tree.insert(point6);
        tree.insert( point7);
        tree.insert(point8);
        tree.regionSearch(0, 0, 5, 5);
        
    }
    
    /**
     * Test regionsearch with a rectangle
     * that doesn't include the current
     * points
     */
    public void testRegionSearchDifferentRegion()
    {
        Point point = new Point(2, 4, "Point1");
        Point point2 = new Point(3, 4, "Point2");
        Point point3 = new Point(4, 4, "Point3");
        Point point4 = new Point(5, 4, "Point4");
        Point point5 = new Point(2, 7, "Point5");
        Point point6 = new Point(2, 9, "Point6");
        Point point7 = new Point(9, 4, "Point7");
        Point point8 = new Point(2, 11, "Point8");
        assertTrue(tree.insert(point));
        assertTrue(tree.insert(point2));
        assertTrue(tree.insert(point3));
        assertTrue(tree.insert(point4));
        assertTrue(tree.insert(point5));
        assertTrue(tree.insert(point6));
        assertTrue(tree.insert(point7));
        assertTrue(tree.insert(point8));
        tree.regionSearch(600, 600, 10, 10);
    }
    
    /**
     * test remove duplicates
     */
    public void testRemoveDuplicates()
    {
        Point point = new Point(10, 10, "Point1");
        Point point2 = new Point(10, 10, "Point2");
        Point point3 = new Point(10, 10, "Point3");
        Point point4 = new Point(10, 10, "Point4");
        assertTrue(tree.insert(point));
        assertTrue(tree.insert(point2));
        assertTrue(tree.insert(point3));
        assertTrue(tree.insert(point4));
        tree.dump();
        
        tree.remove(point2);
        tree.dump();
        
    }
    
}

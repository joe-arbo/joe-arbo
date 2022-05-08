import student.TestCase;

/**
 * Will Test the Point Class
 * @author Joseph Arbolino
 * @version 1.0
 */
public class PointTest extends TestCase {

    private Point point;
    private Point pointX;
    private Point pointY;
    private Point nullPoint;
    private String string;
    private Point equalPoint;
    
    /**
     * Will Set up Point to be tested
     */
    public void setUp()
    {
        point = new Point(1, 1, "Name");
        pointX = new Point(2, 1, "Name");
        pointY = new Point(1, 2, "Name");
        nullPoint = null;
        string = "";
        equalPoint = new Point(1, 1, "Name");
    }
    
    /**
     * Tests Equals
     */
    public void testEquals()
    {
        assertFalse(point.equals(pointX));
        assertFalse(point.equals(pointY));
        assertFalse(point.equals(nullPoint));
        assertFalse(point.equals(string));
        assertTrue(point.equals(equalPoint));
    }
}

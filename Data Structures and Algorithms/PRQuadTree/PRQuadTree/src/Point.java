/**
 * Creates a point with an x and y coordinate to be stored in the SkipList
 */

/**
 * @author Joseph Arbolino
 * @version 1.0
 * 
 */
public class Point
{
    //x coordinate of the point
    private int x;
    //y coordinate of the point
    private int y;
    //name of the point
    private String name;
    
    /**
     * Creates a new Point object
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @param name name of the point
     */
    public Point(int x, int y, String name)
    {
        this.x = x;
        this.y = y;
        this.name = name;
    }
    
    /**
     * Returns the x coordinate of the point
     * @return the x coordinate of the point
     */
    public int getX()
    {
        return x;
    }
    
    /**
     * Returns the y coordinate of the point
     * @return the y coordinate of the point
     */
    public int getY()
    {
        return y;
    }
    
    /**
     * Sets the value of X
     * @param newX the new X coordinate
     */
    public void setX(int newX)
    {
        x = newX;
    }
    
    /**
     * Sets the value of Y
     * @param newY the new Y coordinate
     */
    public void setY(int newY)
    {
        y = newY;
    }
    
    /**
     * Gets the name of this point
     * @return the name of this point
     */
    public String getName()
    {
        return name;
    }
    
    /**
     * Sets the name of the point
     * @param newName the incoming name of the point
     */
    public void setName(String newName)
    {
        name = newName;
    }
    
    /**
     * Compares 2 Points and returns if they are equal
     * 
     * @param comp the object of the point to compare to
     * @return false if not equal, true otherwise
     */
    public boolean equals(Object comp)
    {
        if (comp == null)
        {
            return false;
        }
        
        if (comp.getClass() != this.getClass())
        {
            return false;
        }
        
        Point compPoint = (Point) comp;
        return compPoint.getX() == this.getX() &&
                compPoint.getY() == this.getY()
                && name.equals(compPoint.getName());
    }
    
    /**
     * Prints the point in the correct format
     * @return the formatted String with the name and coordinates
     */
    public String toString()
    {
        return "(" + name + ", " + x + ", " + y + ")";
    }
    
}

import java.util.ArrayList;

/**
 * Superclass for the LeafNode, InternalNode, and EmptyLeafNode
 * The superclass will help with having common methods among
 * the PRQuadTree
 * @author Ryan Fasco 906093506
 * @version 1.0
 * by the node
 */
public class PRQuadTreeNode {

    /**
     * Generic constructor
     */
    public PRQuadTreeNode()
    {
        //Does nothing
    }
    
    /**
     * Insert data into node
     * @param data the insert point
     * @return if the insertion was successful
     */
    public boolean insert(Point data)
    {
        return false;
    }
    
    /**
     * Return a String representation of the node
     * @return the toString
     */
    public String toString()
    {
        return ""; 
    }
    
    /**
     * Remove an element from the node
     * @param elem Element to be removed
     * @return the removed element
     */
    public Point remove(Point elem)
    {
        return null;
    }
    
    /**
     * get the elements of the tree
     * @return an arraylist of elements
     */
    public ArrayList<Point> getElements()
    {
        return null;
    }

    /**
     * Sets the children of this node
     * @param children the new set of children
     */
    public void setChildren(PRQuadTreeNode[] children) {
        //does nothing here
    }
    
}

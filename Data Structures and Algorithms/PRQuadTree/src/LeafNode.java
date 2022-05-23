import java.util.ArrayList;

/**
 * Leaf node containing at least 1 element and 
 * up to 3 unique elements
 * @author Ryan Fasco 906093506
 * @version 1.0
 */
public class LeafNode extends PRQuadTreeNode {

    private int uniqueCount; //Number of unique elements
    private ArrayList<Point> elements; //Dynamic list of elements
    /**
     * Creates a new Leaf Node
     */
    public LeafNode()
    {
        elements = new ArrayList<Point>();
        uniqueCount = 0;
    }
    
    /**
     * Returns the list of elements
     * @return the list of elements
     */
    public ArrayList<Point> getElements()
    {
        return elements;
    }
    
    /**
     * Returns number of unique elements
     * @return number of unique elements
     */
    public int getCount()
    {
        return uniqueCount;
    }
    
    /**
     * Sets the number of unique points in this node
     * @param newCount the new number of unique points
     */
    public void setCount(int newCount)
    {
        uniqueCount = newCount;
    }
    
    /**
     * Checks if the node meets the requirements for
     * decomposition
     * @return if the node can be split
     */
    public boolean split()
    {
        return elements.size() >= 4 && uniqueCount > 1;
    }
    
    /**
     * Inserts a new element into the node. Returns false if there are 
     * 4 unique nodes so that the tree can split the node
     * @param newElement
     * @return False if the node has reached capacity
     */
    @Override
    public boolean insert(Point newElement)
    {
        boolean newElem = true;
        for (int i = 0; i < elements.size(); i++)
        {
            if (newElement.getX() == elements.get(i).getX()
                && newElement.getY() == elements.get(i).getY())
            {
                if (newElement.getName().equals(elements.get(i).getName()))
                {
                    return false;
                }
                else
                {
                    newElem = false;
                }
            }
        }
        if (newElem)
        {
            uniqueCount++;
        }
        elements.add(newElement);
        return true;
    }
    
    /**
     * Removes the specified element from the node.
     * The tree will handle changing the node from a leaf
     * to an empty leaf if the element count is less than 0
     * @param elem the Point coordinates
     * @return the point removed
     */
    public Point remove(Point elem)
    {
        boolean unique = true;
        boolean found = false;
        //checks if remove was called from point2 with digits
        if (elem.getName().equals(""))
        {
            for (int i = 0; i < elements.size(); i++)
            {
                if (elem.getX() == elements.get(i).getX()
                    && elem.getY() == elements.get(i).getY())
                {
                    if (found)
                    {
                        unique = false;
                    }
                    else
                    {
                        elem.setName(elements.get(i).getName());
                        elements.remove(elem);
                        found = true;
                    }
                } 
            }
        }
        else
        {
            for (int i = 0; i < elements.size(); i++)
            {
                if (elem.getX() == elements.get(i).getX()
                    && elem.getY() == elements.get(i).getY())
                    
                {
                    if (elem.getName().equals(elements.get(i).getName()))
                    {
                        found = true;
                        elements.remove(elem);
                    }
                    else
                    {
                        unique = false;
                    }
                }
            }
        }
        if (unique)
        {
            uniqueCount--;
        }
        if (found)
        {
            return elem;
        }
        else
        {
            return null;
        }
    }
    
    /**
     * Checks if this node contains any points
     * @return if the point is empty
     */
    public boolean isEmpty()
    {
        return elements.isEmpty();
    }
    
    /**
     * Sets the elements contained by this node
     * @param newElements the new set of points contained
     *        in the node
     */
    public void setElements(ArrayList<Point> newElements)
    {
        elements = newElements;
        //elements.clear();
        //elements.addAll(newElements);
    }

}

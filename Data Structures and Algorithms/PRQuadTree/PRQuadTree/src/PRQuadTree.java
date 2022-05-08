import java.util.ArrayList;
//import java.lang.StringBuilder;

/**
 * A data structure implementation of the PRQuadTree.
 * Uses a system of Nodes and quadrants to organize
 * and store data.
 */

/**
 * @author Joseph Arbolino
 * @author Ryan Fasco
 * @version 1.0
 */
public class PRQuadTree
{
    private PRQuadTreeNode root;
    private EmptyLeafNode empty;
    private int area;
    private Point zero;
    //Global variables used to check if nodes need to be merged
    //Record the number of points in an internal node's children
    private int mergeCount;
    private int mergeCountUnique;
    private boolean inserted;
    private int nodesTraversed;
    private Point removedPointG;
    
    /**
     * Creates a new PRQuadTree with the defined area
     * @param area the area of the square that contains
     *        the points
     */
    public PRQuadTree(int area)
    {
        root = new LeafNode();
        empty = new EmptyLeafNode(); //Flyweight for EmptyLeafNodes
        this.area = area;
        zero = new Point(0, 0, "zero"); //Reference for top-left of active area
    }
    
    /**
     * Inserts a new point into the tree
     * @param point the point to be inserted
     * @return if the point was successfully inserted
     */
    public boolean insert(Point point)
    {
        //checks for valid parameters and coordinates
        if (point == null || point.getName() == null)
        {
            return false;
        }
        if (point.getX() < 0 || point.getX() > area
            || point.getY() < 0 || point.getY() > area)
        {
            return false;
        }
        inserted = false;
        //sets the top-left point to (0, 0)
        zero.setX(0);
        zero.setY(0);
        //call to recursive method
        insert(point, root, area, zero);
        return inserted;
    }
    
    /**
     * Recursive insert method. Goes down the tree until 
     * it finds the proper location for the point
     * @param x the x coordinate of the point being inserted
     * @param y the y coordinate of the point being inserted
     * @param curr the node used to traverse 
     *        the tree to find the spot for insertion
     * @param size the size of the current area
     * @param first the top-left point in the current area
     * @return the new location of the point, or null otherwise
     */
    private PRQuadTreeNode insert(
        Point point, PRQuadTreeNode curr, int size, Point first)
    {
        if (size < 1)
        {
            return null;
        }
        
        if (curr.getClass().getName().equals("LeafNode"))
        {
            //cast to leaf node to insert
            LeafNode leaf = (LeafNode) curr;
            inserted = leaf.insert(point);
            
            //checks if leaf node meets the conditions for decomposition
            if (leaf.split() && size > 1)
            {
                //converts leaf node into internal node
                PRQuadTreeNode temp = split(leaf, size, first);
                if (temp != null)
                {
                    curr = temp;
                }
                return curr;
            }
            return leaf;
        }
        else if (curr.getClass().getName().equals("InternalNode"))
        {
            //divide current area into 4 squares
            int midX = (first.getX() + (size / 2));
            int midY = (first.getY() + (size) / 2);
            int x = point.getX();
            int y = point.getY();
            
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            //iterates through each quadrant
            if (x < midX && y < midY)
            {
                if (!children[0].getClass().getName().equals("InternalNode"))
                {
                    //recursively updates child node with new value
                    PRQuadTreeNode temp = insert(point, 
                            children[0], size / 2, first);
                    if (temp != null)
                    {
                        children[0] = temp;
                    }
                    return children[0];
                }
                else
                {
                    //no need to update children if they are internal nodes
                    return insert(point, children[0], size / 2, first);
                }
            }
            else if (x >= midX && y < midY)
            {
                //change top-left point to match quadrant
                first.setX(first.getX() + (size / 2));
                if (!children[1].getClass().getName().equals("InternalNode"))
                {
                    PRQuadTreeNode temp = insert(point, 
                            children[1], size / 2, first);
                    if (temp != null)
                    {
                        children[1] = temp;
                    }
                    return children[1];
                }
                else
                {
                    return insert(point, children[1], size / 2, first);
                }
            }
            else if (x < midX && y >= midY)
            {
                first.setY(first.getY() + (size / 2));
                if (!children[2].getClass().getName().equals("InternalNode"))
                {
                    PRQuadTreeNode temp = insert(point, 
                            children[2], size / 2, first);
                    if (temp != null)
                    {
                        children[2] = temp;
                    }
                    return children[2];
                }
                else
                {
                    return insert(point, children[2], size / 2, first);
                }
            }
            else if (x >= midX && y >= midY)
            {
                first.setX(first.getX() + (size / 2));
                first.setY(first.getY() + (size / 2));
                if (!children[3].getClass().getName().equals("InternalNode"))
                {
                    PRQuadTreeNode temp = insert(point, 
                            children[3], size / 2, first);
                    if (temp != null)
                    {
                        children[3] = temp;
                    }
                    return children[3];
                }
                else
                {
                    return insert(point, children[3], size / 2, first);
                }
            }
        }
        else if (curr.getClass().getName().equals("EmptyLeafNode"))
        {
            //change empty leaf node to leaf node
            curr = new LeafNode();
            inserted = curr.insert(point);
            return curr;
        }
        return null;
    }
    
    /**
     * Handles node decomposition when a leaf node has at least 4 values and
     * at least 2 unique values
     * @param node the node to be split
     * @param size the size of the current area
     * @param first the top-left point of the current area
     * @return the new internal node created from the leaf node
     */
    private InternalNode split(PRQuadTreeNode node, int size, Point first)
    {
        if (size < 1)
        {
            return null;
        }
        
        PRQuadTreeNode[] children = new PRQuadTreeNode[4];
        ArrayList<Point> points = node.getElements();
        
        //divide current area into 4 squares
        int midX = (first.getX() + (size / 2));
        int midY = (first.getY() + (size) / 2);
        
        
        //assign all of the points from the leaf into the appropriate
        //children nodes
        while (!points.isEmpty())
        {
            //sort points one at a time
            Point temp = points.get(points.size() - 1);
            int x = temp.getX();
            int y = temp.getY();
            //sorts points into proper quadrants
            if (x < midX && y < midY)
            {
                if (children[0] == null)
                {
                    //sets nodes that contain points as leaf nodes
                    children[0] = new LeafNode();
                }
                //removes duplicate points
                while (points.contains(temp))
                {
                    children[0].insert(temp);
                    points.remove(temp);
                }
                
            }
            else if (x >= midX && y < midY)
            {
                if (children[1] == null)
                {
                    children[1] = new LeafNode();
                }
                while (points.contains(temp))
                {
                    children[1].insert(temp);
                    points.remove(temp);
                }
            }
            else if (x < midX && y >= midY)
            {
                if (children[2] == null)
                {
                    children[2] = new LeafNode();
                }
                while (points.contains(temp))
                {
                    children[2].insert(temp);
                    points.remove(temp);
                }
            }
            else if (x >= midX && y >= midY)
            {
                if (children[3] == null)
                {
                    children[3] = new LeafNode();
                }
                while (points.contains(temp))
                {
                    children[3].insert(temp);
                    points.remove(temp);
                }
            }
            
        }
        //sets value of all remaining children and handles
        //cascading decomposition
        for (int i = 0; i < 4; i++)
        {
            if (children[i] == null)
            {
                //sets empty children as empty nodes
                children[i] = empty;
            }
            else if (children[i].getClass().getName().equals("LeafNode"))
            {
                LeafNode leaf = (LeafNode) children[i];
                //checks if children meet requirements for decomposition
                if (leaf.split() && size > 1)
                {
                    //handles cascading decomposition based on the quadrant
                    if (i == 0)
                    {
                        PRQuadTreeNode temp = split(leaf, size / 2, first);
                        if (temp != null)
                        {
                            children[i] = temp;
                        }
                    }
                    else if (i == 1)
                    {
                        first.setX(first.getX() + (size / 2));
                        PRQuadTreeNode temp = split(leaf, size / 2, first);
                        if (temp != null)
                        {
                            children[i] = temp;
                        }
                    }
                    else if (i == 2)
                    {
                        first.setY(first.getY() + (size / 2));
                        PRQuadTreeNode temp = split(leaf, size / 2, first);
                        if (temp != null)
                        {
                            children[i] = temp;
                        }
                    }
                    else
                    {
                        first.setX(first.getX() + (size / 2));
                        first.setY(first.getY() + (size / 2));
                        PRQuadTreeNode temp = split(leaf, size / 2, first);
                        if (temp != null)
                        {
                            children[i] = temp;
                        }
                    }
                    
                }
            }
        }
        
        //returns current node as internal node
        if (node.equals(root))
        {
            root = new InternalNode();
            root.setChildren(children);
            return (InternalNode)root;
        }
        else
        {
            node = new InternalNode();
            node.setChildren(children);
            return (InternalNode)node;
        }
        
    }
    
    /**
     * Removes the given point from the tree
     * @param point the point being removed
     * @return the point that was removed
     */
    public Point remove(Point point)
    {
        //checks for valid parameters and coordinates
        if (point == null || point.getName() == null)
        {
            return null;
        }
        if (point.getX() < 0 || point.getX() > area
            || point.getY() < 0 || point.getY() > area)
        {
            return null;
        }
        //sets the top-left point to (0, 0)
        zero.setX(0);
        zero.setY(0);
        //sets the count used for merging to 0
        mergeCountUnique = 0;
        mergeCount = 0;
        //global variable used to record removed
        //point
        removedPointG = null;
        //Recursive call to remove point
        remove(root, point, area, zero);
        return removedPointG;
    }
    
    /**
     * Recursive function to remove points. Travels down the tree
     * until it finds the point to be removed. Handles calls for
     * merging nodes after removal
     * @param curr the node used to traverse the tree
     * @param data the point to be removed from the tree
     * @param size the size of the current area
     * @param first the top-left point in the current area
     * @return the removed point, or null if it was not found
     */
    private PRQuadTreeNode remove(
        PRQuadTreeNode curr, Point data, int size, Point first)
    {
        if (curr.getClass().getName().equals("InternalNode"))
        {
            int midX = (first.getX() + (size / 2));
            int midY = (first.getY() + (size / 2));
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            int x = data.getX();
            int y = data.getY();
            //recursive call on the proper quadrant of the current area
            if (x < midX && y < midY)
            {
                //recursive call on child node
                children[0] = remove(children[0], data, size / 2, first);
                //check if this node could merge
                if (mergeCount < 4 || mergeCountUnique <= 1)
                {
                    mergeCountUnique = 0;
                    mergeCount = 0;
                    return checkMerge((InternalNode) curr);
                }
            }
            else if (x >= midX && y < midY)
            {
                first.setX(first.getX() + (size / 2));
                children[1] = remove(children[1], data, size / 2, first);
                if (mergeCount < 4 || mergeCountUnique <= 1)
                {
                    mergeCountUnique = 0;
                    mergeCount = 0;
                    return checkMerge((InternalNode) curr);
                }
            }
            else if (x < midX && y >= midY)
            {
                first.setY(first.getY() + (size / 2));
                children[2] = remove(children[2], data, size / 2, first);
                if (mergeCount < 4 || mergeCountUnique <= 1)
                {
                    mergeCountUnique = 0;
                    mergeCount = 0;
                    return checkMerge((InternalNode) curr);
                }
            }
            else if (x >= midX && y >= midY)
            {
                first.setX(first.getX() + (size / 2));
                first.setY(first.getY() + (size / 2));
                children[3] = remove(children[3], data, size / 2, first);
                if (mergeCount < 4 || mergeCountUnique <= 1)
                {
                    mergeCountUnique = 0;
                    mergeCount = 0;
                    return checkMerge((InternalNode) curr);
                }
            }
        }
        else if (curr.getClass().getName().equals("LeafNode"))
        {
            removedPointG = curr.remove(data);
            if (((LeafNode)curr).isEmpty())
            {
                curr = empty;
            }
            return curr;
        }           
        
        return curr;
    }
    
    /**
     * Checks if a node should merge. A node should merge if 
     * it is internal and the sum of all of the points contained
     * in its children is less than 4, or if the number of unique points
     * contained in its children is less than 2
     * @param parent the node to be merged
     * @return the newly merged node, or the current node if the
     *         conditions to merge were not met
     */
    private PRQuadTreeNode checkMerge(PRQuadTreeNode parent)
    {
        //returns current node if it is not internal
        if (!parent.getClass().getName().equals("InternalNode"))
        {
            return parent;
        }
        //the number of points contained in this node's children
        int thisCount = 0;
        //the number of unique points contained in this node's children
        int thisCountUnique = 0;
        PRQuadTreeNode[] children = ((InternalNode)parent).getChildren();
        //iterates through each child and adds up unique and total points
        for (int i = 0; i < 4; i++)
        {
            //if a child is internal, checks if 
            //that node should merge. The point total
            //from that child will be added to the global mergeCount variables
            if (children[i].getClass().getName().equals("InternalNode"))
            {
                children[i] = checkMerge((InternalNode)children[i]);
            }
            else if (children[i].getClass().getName().equals("LeafNode"))
            {
                LeafNode leaf = (LeafNode) children[i];
                thisCountUnique += leaf.getCount();
                thisCount += leaf.getElements().size();
            }
        }
        //adds the sum of points and the unique points from this node's 
        //children to the global count
        mergeCountUnique += thisCountUnique;
        mergeCount += thisCount;
        //If this node's children contain less 
        //than 4 total nodes or no more than
        //1 unique node, then this node will be merged into a single leaf node
        if (mergeCount < 4 || mergeCountUnique <= 1)
        {
            return merge(parent, mergeCountUnique);
        }
        return parent;
    }
    
    /**
     * Merges an internal node's children into one leaf node
     * @param parent
     * @return
     */
    private PRQuadTreeNode merge(PRQuadTreeNode parent, int unique)
    {
        PRQuadTreeNode[] children = ((InternalNode)parent).getChildren();
        ArrayList<Point> points = new ArrayList<Point>();
        //iterates through this node's children and adds them to a list
        for (int i = 0; i < 4; i++)
        {
            if (children[i].getClass().getName().equals("LeafNode"))
            {
                points.addAll(((LeafNode)children[i]).getElements());
                //children[i] = null;
            }
        }
        //turns the internal node into the proper node type
        if (parent.equals(root))
        {
            root = new LeafNode();
            ((LeafNode)root).setElements(points);
            ((LeafNode)root).setCount(unique);
        }
        else
        {
            if (points.isEmpty())
            {
                parent = empty;
            }
            else
            {
                //parent.setChildren(null);
                parent = new LeafNode();
                ((LeafNode)parent).setElements(points);
                ((LeafNode)parent).setCount(unique);
            }
        }
        
        return parent;
    }
    
    /**
     * Performs a regionsearch for all points located inside
     * the specified region
     * @param x the x coordinate of the region
     * @param y the y coordinate of the region
     * @param w the width of the region
     * @param h the height of the region
     * @return true
     */
    public boolean regionSearch(int x, int y, int w, int h)
    {
     // parameters for rejection
        if (w <= 0 || h <= 0) 
        {
            String rejMessage = "Rectangle rejected: (" 
                    + x + ", " + y + ", " + w + ", " + h + ")";
            System.out.println(rejMessage);
            return false;
        }
        
        System.out.println("Points intersecting region ( " 
                + x + ", " + y + ", " + w + ", " + h + "):");
        nodesTraversed = 1;
        regionSearch(root, x, y, w, h, area, 0, 0);
        System.out.println(nodesTraversed + " quadtree nodes visited");
        return true;
    }
    
    /**
     * Searches each node of the tree for points that intersect with
     * the given rectangle
     * @param curr the node used to traverse the tree
     * @param x the x coordinate of the top-left point of the rectangle
     * @param y the x coordinate of the top-left point of the rectangle
     * @param w the width of the rectangle
     * @param h the height of the rectangle
     * @param size the size of the current area
     * @param thisX the x coordinate of the top-left point of the current area
     * @param thisY the y coordinate of the top-left point of the current area
     */
    private void regionSearch(PRQuadTreeNode curr, int x, int y, 
        int w, int h, int size, int thisX, int thisY)
    {
        if (curr.getClass().getName().equals("InternalNode"))
        {
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            
            if (regionsIntersect(thisX, thisY, size / 2, x, y, w, h))
            {
                nodesTraversed++;
                regionSearch(children[0], x, y, w, h, size / 2, thisX, thisY);
            }
            if (regionsIntersect(thisX + (size / 2), 
                    thisY, size / 2, x, y, w, h))
            {
                nodesTraversed++;
                regionSearch(children[1], x, y, w, h, size / 2, 
                        thisX + (size / 2), thisY);
            }
            if (regionsIntersect(thisX, thisY + (size / 2), 
                    size / 2, x, y, w, h))
            {
                nodesTraversed++;
                regionSearch(children[2], x, y, w, h, size / 2, 
                        thisX, thisY + (size / 2));
            }
            if (regionsIntersect(thisX + (size / 2), thisY + 
                    (size / 2), size / 2, x, y, w, h))
            {
                nodesTraversed++;
                regionSearch(children[3], x, y, w, h, size / 2, 
                        thisX + (size / 2), thisY + (size / 2));
            }
        }
        else if (curr.getClass().getName().equals("LeafNode"))
        {
            ArrayList<Point> points = curr.getElements();
            for (int i = 0; i < points.size(); i++)
            {
                Point temp = points.get(i);
                int tempX = temp.getX();
                int tempY = temp.getY();
                if (!(tempX <= x || tempX >= x + w 
                        || tempY <= y || tempY >= y + h))
                {
                    System.out.println("Point found: " + temp.toString());
                }
            }
        }
    }
    
    /**
     * Checks if the rectangle intersects with the current area
     * @param x the x coordinate of the top-left point of the rectangle
     * @param y the x coordinate of the top-left point of the rectangle
     * @param w the width of the rectangle
     * @param h the height of the rectangle
     * @param size the size of the current area
     * @param thisX the x coordinate of the top-left point of the current area
     * @param thisY the y coordinate of the top-left point of the current area
     * @return if the rectangle intersects with the area
     */
    private boolean regionsIntersect(
        int thisX, int thisY, int size, int x, int y, int w, int h)
    {
        return !(thisX + size <= x
            || thisY + size <= y
            || thisX >= x + w
            || thisY >= y + h);
    }
    
    /**
     * Calls the Recursive function duplicates
     */
    public void duplicates()
    {
        System.out.println("Duplicate points: ");
        duplicates(root);
    }
    
    /**
     * Will print all duplicates in the tree
     * @param curr the root node
     * @return true
     */
    private boolean duplicates(PRQuadTreeNode curr)
    {
        if (curr.getClass().getName().equals("InternalNode"))
        {
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            duplicates(children[0]);
            duplicates(children[1]);
            duplicates(children[2]);
            duplicates(children[3]);
        }
        else if (curr.getClass().getName().equals("LeafNode"))
        {
            ArrayList<Point> points = curr.getElements();
            ArrayList<Point> dupPoints = new ArrayList<Point>();
            
            for (int i = 0; i < points.size(); i++)
            {
                for (int j = i + 1; j < points.size(); ++j)
                {
                    //do not compare points to themselves
                    if (((points.get(i).getX() == points.get(j).getX()) 
                        && (points.get(i).getY() == points.get(j).getY())))
                    {
                        //duplicate found
                        if (dupPoints.isEmpty())
                        {
                            dupPoints.add(points.get(i));
                            String dupMessage = "(" + points.get(i).getX() 
                                    + ", " + points.get(i).getY() + ")";
                            System.out.println(dupMessage);
                        }
                        else
                        {
                            for (int k = 0; k < dupPoints.size(); ++k)
                            {
                                //if dupPoint does not already exist
                                if (dupPoints.get(k).getX() != 
                                        points.get(i).getX()
                                        && dupPoints.get(k).getY() != 
                                        dupPoints.get(k).getY())
                                {
                                    dupPoints.add(points.get(i));  
                                    String dupMessage = "(" 
                                            + points.get(i).getX() 
                                            + ", " + points.get(i).getY() + ")";
                                    System.out.println(dupMessage);
                                } 
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
    
    /**
     * Searches for the given node in the tree
     * @param data the node to be searched for
     * @return if the node was found
     */
    public boolean search(Point data)
    {
        //set the upper-left corner to (0, 0)
        zero.setX(0);
        zero.setY(0);
        //call to recursive search method
        return search(root, data, area, zero) != null;
    }
    
    /**
     * Recursively searches through the tree for the given node
     * @param curr the node used to traverse the tree
     * @param data the node that is being searched for
     * @param size the size of the current area
     * @param first the upper-left point of the current area
     * @return the node if found, or null otherwise
     */
    private PRQuadTreeNode search(
        PRQuadTreeNode curr, Point data, int size, Point first)
    {
        if (curr.getClass().getName().equals("InternalNode"))
        {
            int midX = (first.getX() + (size / 2));
            int midY = (first.getY() + (size / 2));
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            int x = data.getX();
            int y = data.getY();
            //searches through the proper child node
            if (x < midX && y < midY)
            {
                return search(children[0], data, size / 2, first);
            }
            else if (x >= midX && y < midY)
            {
                first.setX(first.getX() + (size / 2));
                return search(children[1], data, size / 2, first);
            }
            else if (x < midX && y >= midY)
            {
                first.setY(first.getY() + (size / 2));
                return search(children[2], data, size / 2, first);
            }
            else if (x >= midX && y >= midY)
            {
                first.setX(first.getX() + (size / 2));
                first.setY(first.getY() + (size / 2));
                return search(children[3], data, size / 2, first);
            }
        }
        else if (curr.getClass().getName().equals("LeafNode"))
        {
            if (curr.getElements().contains(data))
            {
                return curr;
            }
        }
        return null;
    }
    
    /**
     * Prints a visual representation of the tree, with every
     * child node indented under its parent, and with every point
     * under its node. Covers the tree in preorder traversal
     */
    public void dump()
    {
        nodesTraversed = 0;
        System.out.println("QuadTree Dump:");
        dump(root, area, 0, 0, 0);
        String dmpMessage = nodesTraversed + " quadTree nodes printed";
        System.out.println(dmpMessage);
    }
    
    /**
     * Recursive call for dump. Prints a line for each node and for
     * each point
     * @param curr the node used to traverse the tree
     * @param size the size of the current area
     * @param x the x coordinate of the upper-left point of the current area
     * @param y the y coordinate of the upper-left point of the current area
     * @param indentCount how much each line will be indented
     */
    private void dump(
        PRQuadTreeNode curr, int size, int x, int y, int indentCount)
    {
        if (root.getClass().getName().equals("LeafNode"))
        {
            if (((LeafNode)root).isEmpty())
            {
                System.out.println("Node at " + x + 
                        ", " + y + ", " + size + ": Empty");
                ++nodesTraversed;
                return;
            }
        }
        String indent = "";
        StringBuilder builder = new StringBuilder();
        //add the appropriate whitespace before each line
        for (int i = 0; i < indentCount; i++)
        {
            builder.append(' ');
        }
        indent = builder.toString();
        if (curr.getClass().getName().equals("InternalNode"))
        {
            System.out.println(indent + "Node at " + x + 
                    ", " + y + ", " + size + ": Internal");
            ++nodesTraversed;
            InternalNode internal = (InternalNode) curr;
            PRQuadTreeNode[] children = internal.getChildren();
            dump(children[0], size / 2, x, y, indentCount + 2);
            dump(children[1], size / 2, x + (size / 2), y, indentCount + 2);
            dump(children[2], size / 2, x, y + (size / 2), indentCount + 2);
            dump(children[3], size / 2, x + (size / 2), 
                    y + (size / 2), indentCount + 2);
        }
        else if (curr.getClass().getName().equals("LeafNode"))
        {
            System.out.println(indent + "Node at " + x + 
                    ", " + y + ", " + size + ":");
            ++nodesTraversed;
            ArrayList<Point> points = curr.getElements();
            for (int i = 0; i < points.size(); i++)
            {
                System.out.println(indent + points.get(i).toString());
            }
        }
        else
        {
            System.out.println(indent + "Node at " + x + 
                    ", " + y + ", " + size + ": Empty");
            ++nodesTraversed;
        }
    }
}

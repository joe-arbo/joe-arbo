
/**
 * Internal node within the tree which has 4 leaf node children
 * @author Ryan Fasco 906093506
 * @version 1.0
 */
public class InternalNode extends PRQuadTreeNode {

    //the children of the Node
    private PRQuadTreeNode[] children;
    /**
     * Internal node in the tree. The children will be set by
     * the tree since the tree will know the size and coordinates
     */
    public InternalNode()
    {
        children = new PRQuadTreeNode[4];
    }
    
    /**
     * Returns the children
     * @return the array of children
     */
    public PRQuadTreeNode[] getChildren()
    {
        return children;
    }
    
    /**
     * Sets the array of child nodes
     * @param newChildren the new array of child nodes
     */
    public void setChildren(PRQuadTreeNode[] newChildren)
    {
        children = newChildren;
    }
}

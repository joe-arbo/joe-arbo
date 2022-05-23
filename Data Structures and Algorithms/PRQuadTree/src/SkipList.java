import java.util.Random;

/**
 * Provides a basic Skip List structure to store key-value pairs and retrieve
 * them efficiently.
 * Most methods taken from Open DSA
 * @author Ryan Fasco 906093506
 * @version 2021-09-23
 * @param <K> Generic key for each KVPair object within the SkipList
 * @param <E> Generic element for each KVPair object within the SkipList
 */
public class SkipList<K extends Comparable<K>, E> {
    //Suppresses warnings for the type E in the
    //SkipNode declaration
    //@SuppressWarnings("hiding")
    
    /**
     * Implements the nodes that will be stored in the SkipList
     * @author OpenDSA
     * @version 2021-09-23
     * @param <K> Generic key for each KVPair object within the SkipList
     * @param <E> Generic element for each KVPair object within the SkipList
     */
    class SkipNode<K extends Comparable<K>, E> {
        private KVPair<K, E> rec;
        private SkipNode<K, E>[] forward;
        private int level;


        /**
         * Creates a new SkipNode object which holds a key-value pair
         * @author Taken from Open DSA
         * @param key
         *            the index by which the node will be sorted
         * @param elem
         *            the value stored in the node
         * @param level
         *            the pointer level on the SkipList where this node resides
         */
        @SuppressWarnings("unchecked")
        public SkipNode(K key, E elem, int level) {
            rec = new KVPair<K, E>(key, elem);
            forward = new SkipNode[level + 1];
            for (int i = 0; i < level; i++)
            {
                forward[i] = null;
            }
            this.level = level;
        }


        /**
         * Get method for the element
         * Taken from Open DSA
         * 
         * @return the value stored in the node
         */
        public E element() {
            return rec.value();
        }


        /**
         * Get method for the key
         * Taken from Open DSA
         * 
         * @return the node's key
         */
        public K key() {
            return rec.key();
        }


        /**
         * Gets all of the nodes in each level in front of this node
         * Taken from Open DSA
         * 
         * @return an array of nodes on each level which this node points to
         */
        public SkipNode<K, E>[] forward() {
            return forward;
        }
        
        /**
         * Returns the level of the node
         * @return the level of the node
         */
        public int level()
        {
            return level;
        }

        /**
         * Returns the node's element in String form
         * Taken from Open DSA
         * 
         * @return the node's element in String form
         */
        public String toString() {
            return rec.toString();
        }
    }

    private SkipNode<K, E> head;
    private int level;
    private int size;
    static private Random ran = new Random(); // Hold the Random class object


    /**
     * Creates a new SkipList with a head node and a size of 0
     * Taken from Open DSA
     */
    public SkipList() {
        head = new SkipNode<K, E>(null, null, 0);
        level = -1;
        size = 0;
    }

    /**
     * Returns the level of the Head node
     * @return the head level
     */
    public int getHeadLevel()
    {
        return head.level();
    }

    /**
     * Return the (first) matching matching element if one
     * exists, null otherwise
     * Taken from Open DSA
     * 
     * @param key
     *            the key associated with the SkipNode to be found
     * @return the SkipNode with the corresponding key, or null 
     *         if it's not found
     */
    public SkipNode<K, E> find(K key) {
        if (key == null) 
        {
            return null;
        }
        SkipNode<K, E> x = head; // Dummy header node
        for (int i = level; i >= 0; i--) // For each level...
        {
            while ((x.forward[i] != null) && (x.forward[i].key().compareTo(
                key) < 0)) // go forward
            {
                x = x.forward[i]; // Go one last step
            }
        }
        
        x = x.forward[0]; // Move to actual record, if it exists
        if ((x != null) && (x.key().compareTo(key) == 0))
        {
            return x; // Got it
        }
        else
        {
            return null; // Its not there
        }
    }


    /**
     * Return the (first) matching matching 
     * element if one exists, null otherwise
     * 
     * @param element
     *            the element associated with the SkipNode to be found
     * @return the SkipNode with the corresponding element, 
     *         or null if it's not found
     */
    public SkipNode<K, E> find(E element) 
    {
        if (element == null) 
        {
            return null;
        }
        SkipNode<K, E> x = head; // Dummy header node
        while ((x.forward[0] != null) && 
            !(x.forward[0].element().equals(element))) 
        { // go forward
            x = x.forward[0]; // Go one last step
        }
        if (x.forward[0] != null && 
            x.forward[0].element().equals(element)) 
        {
            return x.forward[0];
        }
        else
        {
            return null; // Its not there
        }   
    }


    /**
     * Returns a random valid level for a new node
     * Taken from Open DSA
     * 
     * @return random level within the limit
     */
    int randomLevel() {
        int lev;
        for (lev = 0; Math.abs(ran.nextInt()) % 2 == 0; lev++) 
        {
            // ran is random generator
            // Do nothing
        }    
        return lev;
    }


    /**
     * Insert a key, element pair into the skip list
     * Taken from Open DSA
     * 
     * @param key
     *            the key of the new node
     * @param elem
     *            the value stored within the new node
     */
    public void insert(K key, E elem) {
        int newLevel = randomLevel(); // New node's level
        if (newLevel > level) // If new node is deeper
        {
            adjustHead(newLevel); // adjust the header
        }
        // Track end of level
        @SuppressWarnings("unchecked")
        SkipNode<K, E>[] update = new SkipNode[level + 1];
        SkipNode<K, E> x = head; // Start at header node
        for (int i = level; i >= 0; i--) 
        { // Find insert position
            while ((x.forward[i] != null) && 
                (x.forward[i].key().compareTo(key) < 0))
            {
                x = x.forward[i];
            }
            update[i] = x; // Track end at level i
        }

        x = new SkipNode<K, E>(key, elem, newLevel);
        for (int i = 0; i <= newLevel; i++) 
        { // Splice into list
            x.forward[i] = update[i].forward[i]; // Who x points to
            update[i].forward[i] = x; // Who points to x
        }
        size++; // Increment dictionary size
    }


    /**
     * Sets the head node to a new level
     * Taken from Open DSA
     * 
     * @param newLevel The new level for the head node
     */
    private void adjustHead(int newLevel) {
        SkipNode<K, E> temp = head;
        head = new SkipNode<K, E>(null, null, newLevel);
        for (int i = 0; i <= level; i++)
        {
            head.forward[i] = temp.forward[i];
        }
        level = newLevel;
    }


    /**
     * Removes the node with the corresponding key
     * 
     * @param key the key of the node to be removed
     * @return the removed node, or null otherwise
     */
    @SuppressWarnings("unchecked")
    public SkipNode<K, E> remove(K key) {
        if (key == null || size == 0) { //Checks for a null key or empty list
            return null;
        }
        //The node which will take on the value of the removed node
        SkipNode<K, E> rem = null;
        //An array which will contain each node 
        //pointing to or past the node to be removed
        SkipNode<K, E>[] update = new SkipNode[level + 1];
        //Iterates through each level of the SkipList
        for (int i = level; i >= 0; i--) 
        {
            //Sets the corresponding index of the array to head
            update[i] = head.forward[i];
            if (update[i] != null)
            {
                //Checks if the head points to the node to be removed
                if (update[i].key().compareTo(key) == 0)
                {
                    //Removes the head's pointer to the node at 
                    //the current level
                    head.forward[i] = update[i].forward[i];
                    update[i].forward[i] = null;
                    rem = update[i]; //Stores the removed node
                }
                else
                {
                    //Finds the pointer at the current level 
                    //to the node to be removed
                    while ((update[i].forward[i] != null) && 
                        (update[i].forward[i].key().compareTo(key) < 0)) 
                    {
                        //Moves the pointer forward
                        update[i] = update[i].forward[i];
                    }  
                    //Checks if there is a pointer to the 
                    //node at the current level
                    if ((update[i].forward[i] != null) && 
                        (update[i].forward[i].key().compareTo(key) == 0))
                    {
                        //Removes the pointer to the node at the current level
                        rem = update[i].forward[i]; 
                        update[i].forward[i] = update[i].forward[i].forward[i];
                        rem.forward[i] = null; //Stores the removed node
                    }
                }
                
            }
            
        }
        if (rem != null) //Decrements the size if the node was found
        {
            size--;
        }
        //Returns the removed node, or null otherwise
        return rem;
    }
     
    /**
     * Removes the node with the corresponding value
     * @param value the value of the node to be removed
     * @return the removed node, or null otherwise
     */
    public SkipNode<K, E> remove(E value) {
        //Checks for a null value or an empty list
        if (value == null || size == 0) { 
            return null;
        }
        //Finds the node with the corresponding value
        SkipNode<K, E> foundNode = this.find(value);
        if (foundNode == null) //Checks for a null node
        {
            System.out.println("Node not found in skiplist");
            return null;
        }
        else
        {
            //Calls remove(K) with the node's key
            return remove(foundNode.key()); 
        }
    }

    /**
     * Returns an array of every node in the SkipList
     * @return an array containing every node in the SkipList
     */
    @SuppressWarnings("unchecked")
    public SkipNode<K, E>[] toArray() {
        if (head.forward[0] == null) { //Checks for an empty list
            return null;
        }
        SkipNode<K, E>[] array = new SkipNode[size];
        SkipNode<K, E> arrayNode = head.forward[0]; //Iterative node
        int count = 0;
        while (arrayNode != null) { //Iterates through each node
            array[count] = arrayNode;
            arrayNode = arrayNode.forward[0]; //Moves the iterative node forward
            count++;
        }
        return array;
    }

}
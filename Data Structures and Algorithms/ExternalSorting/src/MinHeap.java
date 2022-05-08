/**
 * Stores an array of records where each record is either a leaf, or
 * has 1 or 2 children. Every parent record will have a key that is
 * less than the key of either of their children. The smallest key
 * will be at the top of the heap
 * @author Joseph Arbolino Ryan Fasco
 * @version 1.0
 *
 */
public class MinHeap 
{
    private Record[] heap; // Pointer to the heap array
    private int size; // Maximum size of the heap
    private int n; // Number of things now in heap
    private int inactiveCount;

    /**
     * Constructor for MinHeap
     * @param num items in heap
     * @param maxSize max size of the heap
     */
    public MinHeap(int num, int maxSize) 
    {
        heap = new Record[maxSize];
        n = num;
        size = maxSize;
        inactiveCount = 0;
        buildheap();
    }


    /**
     * returns the current size of the heap
     * @return n size of heap
     */
    int heapsize() 
    {
        return n;
    }
    
    /**
     * Gets the number of inactive records in the heap
     * @return number of inactive records
     */
    public int inactiveCount()
    {
        return inactiveCount;
    }


    /**
     * tells if a pos is a leaf or not
     * @param pos position in heap
     * @return true if leaf, false if not
     */
    boolean isLeaf(int pos) 
    {
        return (pos >= n / 2) && (pos < n);
    }


    /**
     * returns the position of leftchild of current position
     * @param pos position in question
     * @return position of left child
     */
    int leftchild(int pos) 
    {
        if (pos >= n / 2) {
            return -1;
        }
        return 2 * pos + 1;
    }


    /**
     * returns the position of rightchild of current position
     * @param pos position in question
     * @return position of right child
     */
    int rightchild(int pos) 
    {
        if (pos >= (n - 1) / 2) {
            return -1;
        }
        return 2 * pos + 2;
    }


    /**
     * returns the position of parent of current position
     * @param pos position in question
     * @return position of parent
     */
    int parent(int pos) 
    {
        if ((pos <= 0) || (pos >= n)) {
            return -1;
        }
        return (pos - 1) / 2;
    }


    /**
     * Inserts key into heap
     * @param key the value to be inserted
     */
    void insert(Record key)
    {
        if (key == null)
        {
            return;
        }
        if (n >= size) {
            return;
        }
        int curr = n++;
        heap[curr] = key; // Start at end of heap
        // Now sift up until curr's parent's key < curr's key
        while ((curr != 0) && (heap[parent(curr)].compareTo(heap[curr]) > 0)) 
        {
            swap(heap, curr, parent(curr));
            curr = parent(curr);
        }
    }

    /**
     * Heapify contents of heap
     */
    void buildheap() 
    {
        if (n <= 2) {
            siftdown(0);
            return;
        }
        for (int i = n / 2 - 1; i >= 0; i--) {
            siftdown(i);
        }
    }


    /**
     * Put element in its correct place
     * @param pos starting point from which to siftdown        
     */
    void siftdown(int pos) 
    {
        if ((pos < 0) || (pos >= n)) {
            return; // Illegal position
        }
        while (!isLeaf(pos)) {
            int j = leftchild(pos);
            if ((j < (n - 1)) && (heap[j].compareTo(heap[j + 1]) > 0)) {
                j++; // j is now index of child with lesser value
            }
            if (heap[pos].compareTo(heap[j]) <= 0) {
                return;
            }
            swap(heap, pos, j);
            pos = j; // Move down
        }
    }

    
    /**
     * Remove and return min value
     * @return the min value in the heap
     */
    public Record removemin() 
    {
        if (n == 0) {
            return null; // Removing from empty heap
        }
        swap(heap, 0, --n); // Swap min with last value
        siftdown(0); // Put new heap root val in correct place
        return heap[n];
    }
    
    /**
     * Returns the minimum value in the heap
     * @return the top of the heap
     */
    public Record getMin()
    {
        if (n == 0)
        {
            return null;
        }
        return heap[0];
    }


    /**
     * Remove and return element at specified position
     * @param pos to be removed     
     * @return the item removed or -1 if illegal heap position
     */
    public Record remove(int pos) 
    {
        if ((pos < 0) || (pos >= n)) {
            return null; // Illegal heap position
        }
        if (pos == (n - 1)) {
            n--; // Last element, no work to be done
        }
        else {
            this.swap(heap, pos, --n); // Swap with last value
            update(pos);
        }
        return heap[n];
    }

    /**
     * The value at pos has been changed, restore the heap property
     * @param pos where to begin the update process
     */
    void update(int pos) 
    {
        // If it is a big value, push it up
        while ((pos > 0) && (heap[parent(pos)].compareTo(heap[pos]) > 0)) {
            this.swap(heap, pos, parent(pos));
            pos = parent(pos);
        }
        siftdown(pos); // If it is little, push down
    }


    /**
     * Swaps positions of two values
     * @param heap where the values are located
     * @param i first value to be swapped
     * @param j second value to be swapped
     */
    private void swap(Record[] heap1, int i, int j) 
    {
        Record temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    /**
     * Inserts an inactive value to the end of the
     * heap which will not be counted in any operations
     * @param key the record to be inserted
     * @return if the record was successfully inserted
     */
    public boolean insertInactive(Record key) 
    {
        heap[n] = key;
        inactiveCount++;
        return true;
    }
    
    /**
     * Rebuilds the heap when it is completely filled
     * with inactive values
     * @return if the heap was successfully rebuilt
     */
    public boolean rebuildHeap()
    {
        if (n != 0 || inactiveCount != size)
        {
            return false;
        }
        
        for (int i = 0; i < inactiveCount; i++)
        {
            this.insert(heap[(size - inactiveCount) + i]);
        }
        
        /*n = inactiveCount;
        this.buildheap();*/
        inactiveCount = 0;
        return true;
    }
    
    /**
     * Rebuilds the heap when it only contains
     * inactive values, but is not full
     * @return if the heap was successfully rebuilt
     */
    public boolean partialRebuildHeap()
    {
        //inactive count is left at the end of the array, 
        //there's a gap between n and inactive count
        if (n != 0 || inactiveCount == 0)
        {
            return false;
        }
        int count = 0;
        Record[] inactives = new Record[inactiveCount];
        for (int i = heap.length - 1; i > heap.length - 1 - inactiveCount; i--)
        {
            inactives[count] = heap[i];
            count++;
        }
        
        for (int i = 0; i < inactives.length; i++)
        {
            heap[i] = inactives[i];
        }
        
        
        n = inactiveCount;
        this.buildheap();
        inactiveCount = 0;
        return true;
    }
    
    /**
     * Returns true if the amount of active and inactive records
     * in the heap are equal to the total capacity of the tree
     * @return if the tree has no more space for additional records
     */
    public boolean isAtCapacity()
    {
        return n + inactiveCount == size;
    }
    
    /**
     * Returns the key of the record at the
     * given index
     * @param index the index of the record
     *        to be returned
     * @return the key of the returned 
     *         record, or null if the
     *         index is invalid
     */
    public Double getAtIndex(int index)
    {
        if (index < 0 || index >= heap.length)
        {
            return null;
        }
        return heap[index].getKey();
    }
}

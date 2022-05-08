/**
 * Allows a way to sort through non-comparable values by assigning them a key
 * 
 * @author Ryan Fasco 906093506
 *
 * @param <K> The key by which KVPair is sorted
 * @param <E> The value stored within each pair
 * @version 1.0
 */
public class KVPair<K extends Comparable<K>, E> {
    // The key by which KVPair is sorted
    private K key;
    // The value stored within each pair
    private E value;

    /**
     * Creates a new KVPair object
     * 
     * @param key   the key by which KVPair is sorted
     * @param value the value stored within each pair
     */
    public KVPair(K key, E value) {
        this.key = key;
        this.value = value;
    }

    /**
     * Returns the key
     * 
     * @return the key field
     */
    public K key() {
        return key;
    }

    /**
     * Returns the value
     * 
     * @return the value field
     */
    public E value() {
        return value;
    }

    /**
     * Returns the value in String form
     * @return the toString String
     */
    public String toString() {
        return value.toString();
    }
}
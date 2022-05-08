import java.nio.ByteBuffer;

/**
 * Holds a single record
 * 
 * @author CS Staff
 * @author Ryan Fasco
 * @author Joseph Arbolino
 * @version 2021-11-20
 */
public class Record implements Comparable<Record> {

    private byte[] completeRecord;
    private int run;

    /**
     * The constructor for the Record class
     * 
     * @param record
     *            The byte for this object
     */
    public Record(byte[] record) {
        completeRecord = record;
    }

    /**
     * Constructor to set run
     * @param record the bytes for this record
     * @param r the run this record belongs to
     */
    public Record(byte[] record, int r)
    {
        completeRecord = record;
        run = r;
    }
    
    /**
     * returns run
     * @return run
     */
    public int getRun()
    {
        return run;
    }

    /**
     * returns the complete record
     * 
     * @return complete record
     */
    public byte[] getCompleteRecord() {
        return completeRecord;
    }

    /**
     * Returns the object's key
     * 
     * @return the key
     */
    public double getKey() {
        ByteBuffer buff = ByteBuffer.wrap(completeRecord);
        return buff.getDouble(8);
    }
    
    /**
     * Returns the record's id
     * @return the id
     */
    public long getId() {
        ByteBuffer buff = ByteBuffer.wrap(completeRecord);
        return buff.getLong(0);
    }
    
    


    /**
     * Compare Two Records based on their keys
     * 
     * @param toBeCompared The Record to be compared.
     * @return A negative integer, zero, or a positive integer as this employee
     *         is less than, equal to, or greater than the supplied record
     *         object.
     */
    @Override
    public int compareTo(Record toBeCompared) {
        return Double.compare(this.getKey(), toBeCompared.getKey());
    }


    /**
     * Outputs the record as a String
     * 
     * @return a string of what the record contains
     */
    public String toString() {
        return "" + this.getKey();
    }

}

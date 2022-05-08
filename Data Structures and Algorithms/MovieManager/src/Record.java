import java.util.ArrayList;
/**
 * Holds a single record
 * 
 * @author Ryan Fasco
 * @author Joseph Arbolino
 * @version 2021-11-20
 */
public class Record implements Comparable<Record> {

    private ArrayList<KVPair<String, String>> fields;
    private String name;
    private int byteIndex;
    private int blockIndex;

    /**
     * The constructor for the Record class
     * 
     * @param name the name of this record
     */
    public Record(String name) {
        this.name = name;
        fields = new ArrayList<KVPair<String, String>>();
        byteIndex = -1;
        blockIndex = -1;
    }
    
    /**
     * sets the index value
     * @param i the index
     */
    public void setByteIndex(int i)
    {
        byteIndex = i;
    }
    
    /**
     * returns index
     * @return index
     */
    public int getByteIndex()
    {
        return byteIndex;
    }
    
    /**
     * sets the index value
     * @param i the index
     */
    public void setBlockIndex(int i)
    {
        blockIndex = i;
    }
    
    /**
     * returns index
     * @return index
     */
    public int getBlockIndex()
    {
        return blockIndex;
    }
    
    /**
     * Returns the name of the current record
     * @return the name of the movie/series
     */
    public String getName()
    {
        return name;
    }
    
    /**
     * Returns the list of fields
     * @return the fields list
     */
    public ArrayList<KVPair<String, String>> getFields()
    {
        return fields;
    }
    
    /**
     * Adds a new field/value pair to the fields list.
     * Checks if the field is already in the list, and
     * updates that field if it is found
     * @param newField the field to be added
     */
    public void addField(KVPair<String, String> newField)
    {
        if (newField == null)
        {
            return;
        }
        for (int i = 0; i < fields.size(); i++)
        {
            if (newField.key().equals(fields.get(i).key()))
            {
                fields.remove(i);
                fields.add(newField);
                return;
            }
        }
        fields.add(newField);
    }
    
    /**
     * Removes the given field from the list of fields,
     * if it exists
     * @param field the field to be removed
     * @return if the field was successfully removed
     */
    public boolean removeField(String field)
    {
        if (field == null)
        {
            return false;
        }
        for (int i = 0; i < fields.size(); i++)
        {
            if (field.equals(fields.get(i).key()))
            {
                fields.remove(i);
                return true;
            }
        }
        return false;
    }

    /**
     * Compare Two Records based on their names
     * 
     * @param toBeCompared The Record to be compared.
     * @return A negative integer, zero, or positive integer as this employee
     *         is less than, equal to, or greater than the supplied record
     *         object.
     */
    @Override
    public int compareTo(Record toBeCompared) {
        return name.compareTo(toBeCompared.getName());
    }


    /**
     * Outputs the record as a String
     * 
     * @return a string of what the record contains
     */
    public String toString() {
        String string = name;
        for (int i = 0; i < fields.size(); i++)
        {
            string += ("<SEP>" + fields.get(i).key());
            if (!fields.get(i).value().contentEquals(""))
            {
                string += ("<SEP>" + fields.get(i).value());
            }
        }
        return string;
    }

}

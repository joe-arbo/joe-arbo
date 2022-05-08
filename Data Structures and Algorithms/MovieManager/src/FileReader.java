import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;
/**
 * Reads a file and returns the lines of the file in a String
 * array. Ignores any empty lines
 * @author Ryan Fasco 906093506
 * @version 2021-09-23
 */
public class FileReader {

    /**
     * Creates a new FileReader object
     */
    public FileReader()
    {
        //Empty constructor since the point of
        //the FileReader is to return a string array
    }
    
    /**
     * Creates an array of strings based on the given file
     * @param fileName the name of the file being read
     * @return an array containing each non-empty line
     *         in the file
     * @throws FileNotFoundException when an invalid file
     *         is passed into the method
     */
    public String[] readFile(String fileName) throws FileNotFoundException
    {
        if (fileName == null)//Checks for null values
        {
            return null;
        }
        //ArrayList is used for dynamic length
        ArrayList<String> commands = new ArrayList<String>();
        Scanner scan = new Scanner(new File(fileName));
        while (scan.hasNextLine())
        {
            String word = scan.nextLine();
            //Removes excess spaces from lines
            word = word.trim();
            if (!word.isEmpty())//Adds valid lines to ArrayList
            {
                commands.add(word);
            }
        }
        scan.close();
        //Converts ArrayList to array
        String[] commandArray = new String[commands.size()];
        commandArray = commands.toArray(commandArray);
        return commandArray;
    }
}
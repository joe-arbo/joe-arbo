import java.io.FileNotFoundException;
import java.util.ArrayList;

// On my honor:
//
// - I have not used source code obtained from another student,
// or any other unauthorized source, either modified or
// unmodified.
//
// - All source code and documentation used in my program is
// either my original work, or was derived by me from the
// source code published in the textbook for this course.
//
// - I have not discussed coding details about this project with
// anyone other than the instructor, ACM/UPE tutors, programming 
// partner (if allowed in this class), or the TAs assigned to 
// this course. I understand that I may discuss the concepts
// of this program with other students, and that another student
// may help me debug my program so long as neither of us writes
// anything during the discussion or modifies any computer file
// during the discussion. I have violated neither the spirit nor
// letter of this restriction.

/**
 * @author Joseph Arbolino 906186037
 * @author Ryan Fasco 906093506
 * @version 1.0
 *
 */
public class Point2 
{
    //SkipList and PRQuadTree holding points
    private SkipList<String, Point> pointList;
    private PRQuadTree pointTree;
    
    /**
     * Default Constructor
     */
    public Point2()
    {
        pointList = new SkipList<String, Point>();
        pointTree = new PRQuadTree(1024);
    }
    
    /**
     * This program was written in Windows 10 and compiled using JDT Core.
     * When invoked, this program takes in a text file containing a list of
     * commands. These commands involve manipulating Points on a 2D plane.
     * Points are added and removed from this plane based on their name and
     * coordinates. This program will also print a list of all Points in the
     * plane and all Points in a specific region. At the end of the text file, 
     * the program will terminate.
     * Date completed: 10/20/2021
     * @param args the input arguments following the program on the command line
     * @throws FileNotFoundException thrown if the argument is not a valid file
     */
    public static void main(String[] args) throws FileNotFoundException
    {
        Point2 p2 = new Point2();
        FileReader reader = new FileReader();
        
        String[] commands = reader.readFile(args[0]);
        
      //Parses through each line of the file
        for (int i = 0; i < commands.length; i++) 
        {
            //Checks for null lines
            if (commands[i] != null) 
            {
                //The string array which will contain 
                //each individual element of the given line
                String[] split; 
                
                //Splits the string if it contains spaces
                if (commands[i].contains(" ")) 
                {
                    split = commands[i].split("[ \t]");  
                }
                else
                {
                    //Creates an array for a single string such as dump
                    split = new String[1]; 
                    split[0] = commands[i];
                }
                
                if (split[0].contentEquals("insert")) //Handles insert commands
                {
                    String name = ""; //The name of the rectangle
                    
                    //The array for the measurements
                    int[] insertInt = new int[2];
                    int count = 0;
                    
                    //Loops through each string from the given line
                    for (int k = 1; k < split.length; k++)
                    {
                         //Index for the integer array
                         //Ignores strings with tabs and empty strings
                        if (!split[k].contains("\t") && !split[k].isEmpty()) 
                        {
                            //Stores the name of the rectangle
                            if (name.isEmpty())
                            {
                                name = split[k];
                            }
                            else //Stores the integers in the array
                            {    
                                insertInt[count] = Integer.parseInt(split[k]);
                                count++;
                            }  
                        }
                    }
                    p2.insert(name, insertInt[0], insertInt[1]);
                    
                }
                //Handles remove commands
                else if (split[0].contentEquals("remove")) 
                {
                    //Checks if the command is to 
                    //remove by name or by measurements
                    if (split.length < 3)
                    {
                        //Calls remove with the rectangle's name
                        p2.remove(split[1]);
                    }
                    else
                    {
                        int[] removeInt = new int[4];
                        //Calls parseInt to put the measurements in an array
                        removeInt = parseInt(split, 2);
                        p2.remove(removeInt[0], removeInt[1]);
                    }
                    
                }
                //Handles regionsearch commands
                else if (split[0].contentEquals("regionsearch"))
                {
                    //Calls parseInt to put the measurements in an array
                    int[] searchInt = new int[4];
                    searchInt = parseInt(split, 4); 
                    p2.regionsearch(searchInt[0], searchInt[1], 
                            searchInt[2], searchInt[3]);
                }
                //Handles duplicates command
                else if (split[0].contentEquals("duplicates"))
                {
                    p2.duplicates();
                }
                //Handles search commands
                else if (split[0].contentEquals("search"))
                {
                    for (int k = 1; k < split.length; k++)
                    {
                        if (!split[k].contains("\t") && !split[k].isEmpty())
                        {
                            //Removes any spaces from the valid string
                            if (split[k].contains(" "))
                            {
                                split[k] = split[k].trim();
                            }
                            p2.search(split[k]);
                            break;
                        }
                    }
                }
                //Handles dump commands
                else if (split[0].contentEquals("dump"))
                {
                    p2.dump();
                }
            }       
        }
    }
    
    /**
     * Helper method to read 4 integers from a String array
     * 
     * @author Ryan Fasco
     * @param split a string array containing 4 integers
     * @return an array of integers read from the string array
     */
    private static int[] parseInt(String[] split, int len) 
    {
        int[] intArray = new int[len];
        
        // because the rectangle name is stored in split[0]
        int count = 0;
        
        // Iterates through each String. Starts at 1
        for (int k = 1; k < split.length; k++)
        {   
            // Ignores any tabs or empty strings
            if (!split[k].contains("\t") && !split[k].isEmpty())
            {
                // Checks for and removes any spaces from the integer
                if (split[k].contains(" "))
                {
                    split[k] = split[k].trim();
                }

                // Parses the int from the string
                intArray[count] = Integer.parseInt(split[k]);
                count++; // Index of the integer array
            }
        }
        return intArray;
    }
    
    /**
     * Insert a point into both data structures
     * @param name the name of the point
     * @param x the x coordinate of the point
     * @param y the y coordinate of the point
     * @return true if inserted, false otherwise
     */
    public boolean insert(String name, int x, int y)
    {
        if (x < 0 || y < 0 || x > 1024 || y > 1024)
        {
            String errMessage = "Point rejected: (" 
                    + name + ", " + x + ", " + y + ")";
            System.out.println(errMessage);
            return false;
        }
        
        Point p = new Point(x, y, name);
        pointList.insert(name, p);  
        
        System.out.print("Point inserted: ");
        System.out.println(p.toString());
        
        return pointTree.insert(p);
    }
    
    /**
     * Removes a point from both data structures
     * @param name the name of the point
     * @return true if removed, false otherwise
     */
    public boolean remove(String name)
    {

        // create SkipNode for remove
        SkipList<String, Point>.SkipNode<String, Point> rem 
            = pointList.remove(name);

        if (rem == null) // Point does not exist
        {
            String errMessage = "Point not removed: " + name;
            System.out.println(errMessage);
            return false;
        } 
        else // Rectangle exists and has been removed
        {
            //remove from Tree
            Point p = rem.element();
            p.setName(name);
            
            Point remPoint = pointTree.remove(p);
            
            if (remPoint == null)
            {
                String errMessage = "Point not removed: (" + name + ")";
                System.out.println(errMessage);
                return false;
            }
            
            System.out.print("Point removed: ");
            System.out.println(rem.element().toString());
            return true;
        }
    }
    
    /**
     * Removes a point form both data Structures
     * @param x the x coordinate of the point
     * @param y the y coordinate of the point
     * @return true if removed, false otherwise
     */
    public boolean remove(int x, int y)
    {
        if (x < 0 || y < 0 || x > 1024 || y > 1024)
        {
            String errMessage = "Point rejected: (" + x + ", " + y + ")";
            System.out.println(errMessage);
            return false;
        }
        
        Point p = new Point(x, y, "");
        Point rem = pointTree.remove(p);
        
        if (rem == null) // Point does not exist
        {
            String errMessage = "Point not found: "
                    + "(" + x + ", " + y + ")";
            System.out.println(errMessage);
            return false;
        } 
        else // Point exists and has been removed
        {
            //remove from tree
            SkipList<String, Point>.SkipNode<String, Point> remPoint 
                = pointList.remove(rem);
            if (remPoint == null)
            {
                String errMessage = "Point not found: "
                        + "(" + x + ", " + y + ")";
                System.out.println(errMessage);
                return false;
            }
            
            String remMessage = "Point removed: (" + rem.getName() + ", " 
                    + x + ", " + y + ")";
            System.out.println(remMessage);
            return true;
        }
    }
    
    /**
     * Reports all points inside the region being searched
     * formed by the parameters passed in
     * @param x x origin coordinate of the rectangle
     * @param y y origin coordinate of the rectangle
     * @param w width of the rectangle
     * @param h height of the rectangle
     * @return true if regionsearch is valid, false otherwise
     */
    public boolean regionsearch(int x, int y, int w, int h)
    {
        return pointTree.regionSearch(x, y, w, h);
    }
    
    /**
     * Will check for any duplicate points in the list
     * @return true if duplicates found, false otherwise
     */
    public boolean duplicates()
    {
        pointTree.duplicates();
        return true;
    }
    
    /**
     * Will search the lists for a point with name
     * @param name the name of the point
     * @return true if found, false otherwise
     */
    public boolean search(String name)
    {
     // temporary variables to store found points
        ArrayList<Point> foundPointList = new ArrayList<Point>();
        SkipList<String, Point>.SkipNode<String, Point>[] pointListArray 
                = pointList.toArray();

        // no points exist
        if (pointListArray == null) 
        {
            String notFoundMessage = "Point not found: " + name;
            System.out.println(notFoundMessage);
            return false;
        }

        // points exist in the list
        for (int i = 0; i < pointListArray.length; ++i) 
        {
            if (pointListArray[i].key().equals(name)) 
            {
                // point found
                foundPointList.add(pointListArray[i].element());
            }
        }

        // foundPointList is now full of all found points
        // no points found
        if (foundPointList.isEmpty()) 
        {
            String notFoundMessage = "Point not found: " + name;
            System.out.println(notFoundMessage);
            return false;
        }
        // points(s) have been found
        else 
        {
            for (int i = 0; i < foundPointList.size(); ++i) 
            {
                // print out the ArrayList in the correct format
                System.out.print("Found ");
                System.out.println(foundPointList.get(i).toString());
            }

            return true;
        }
    }
    
    /**
     * Will dump the contents of both data structures
     * @return true
     */
    public boolean dump()
    {
        System.out.println("SkipList dump:");

        // get array of SkipList contents
        SkipList<String, Point>.SkipNode<String, Point>[] pointArray 
            = pointList.toArray();
        
        if (pointArray == null)
        {
            System.out.println("Node has depth 1, Value (null)");
            System.out.println("SkipList size is: 0");
        }
        else
        {
         // formulate dumbMessage
            String dumpMessage = "Node has depth " 
                    + pointList.getHeadLevel() + ", Value (null)";
            System.out.println(dumpMessage);

            // iterate through the array and print
            for (int i = 0; i < pointArray.length; ++i) 
            {
                dumpMessage = "Node has depth " 
                        + pointArray[i].level() + ", Value "
                        + pointArray[i].element().toString();
                System.out.println(dumpMessage);
            }

            // print the size of the SkipList
            String sizeMessage = "SkipList size is: " + (pointArray.length);
            System.out.println(sizeMessage);
        }
        
        
        //now call tree dump
        pointTree.dump();

        return pointArray != null;
    }
}

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Project: LifeNet
Developers: Hrushikesh Mehendale, Santosh Vempala
Georgia Institute of Technology, Atlanta, USA
 */
package lifenetftp;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

public class FileHandler {

    private String name;
    private int mode;
    private File file;
    private FileReader fileReader;
    private FileWriter fileWriter;
    private BufferedReader reader;
    private BufferedWriter writer;
    private static int WRITE = 1;
    private static int READ = 0;

    public FileHandler(String fileName) {
        name = fileName;
    }

    public String[] readIntoArray() {

        String[] strArr;
        String text = null;
        String str = null;
        StringBuffer contents = new StringBuffer();

        try {
            if (reader != null) {
                while ((text = reader.readLine()) != null) {
                    contents.append(text).append(System.getProperty("line.separator"));
                }
            }
        } catch (IOException ex) {
            if (Main.DEBUG) {
                Logger.getLogger(FileHandler.class.getName()).log(Level.SEVERE, null, ex);
                System.out.println("File not found in readintoarray");
                ex.printStackTrace();
            }
        } catch (NullPointerException ex) {
            if (Main.DEBUG) {
                System.out.println("Null ptr exception caught in readintoarray");
                ex.printStackTrace();
            }
        }

        str = contents.toString();
        strArr = str.split("\\n");
        return strArr;
    }

    public String getSecondValueFromName(String name) {
        String[] strArr;
        String text = null;
        String str = null;
        StringBuffer contents = new StringBuffer();

        try {
            while ((text = reader.readLine()) != null) {
                contents.append(text).append(System.getProperty("line.separator"));
            }
        } catch (IOException ex) {
            Logger.getLogger(FileHandler.class.getName()).log(Level.SEVERE, null, ex);
        }

        str = contents.toString();
        strArr = str.split("\\n");

        for (int i = 0; i < strArr.length; i++) {
            String[] tempStr;
            tempStr = strArr[i].split(" ");
            if (tempStr[0].trim().equals(name.trim())) {
                return tempStr[1];
            }
        }

        return null;
    }

    public boolean openFile(int fileMode) {
        mode = fileMode;

        try {
            if (mode == READ) {
                file = new File(name);
                fileReader = new FileReader(file);
                reader = new BufferedReader(fileReader);
            } else {
                file = new File(name);
                fileWriter = new FileWriter(file);
                writer = new BufferedWriter(fileWriter);
            }

            return true;

        } catch (FileNotFoundException ex) {
            if (Main.DEBUG) {
                System.out.println("File not found exception in openfile");
                ex.printStackTrace();
            }
            return false;


        } catch (IOException ex) {
            if (Main.DEBUG) {
                System.out.println("IO Exception in openfile");
                Logger.getLogger(FileHandler.class.getName()).log(Level.SEVERE, null, ex);
                ex.printStackTrace();
            }
            return false;
        }
    }

    public void closeFile() {
        try {
            if (mode == READ) {
                reader.close();
                fileReader.close();
                file = null;
            } else {
                writer.close();
                fileWriter.close();
                file = null;
            }

        } catch (IOException ex) {
            if (Main.DEBUG) {
                System.out.println("inside closefile");
                Logger.getLogger(FileHandler.class.getName()).log(Level.SEVERE, null, ex);
                ex.printStackTrace();
            }
        } catch (NullPointerException exc) {
            if (Main.DEBUG) {
                System.out.println("Null ptr. exception in closefile");
                exc.printStackTrace();
            }
        }
    }
}

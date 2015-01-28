/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package passwordlistsorter;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeSet;

/**
 *
 * @author ertai
 */
public class PasswordListSorter {

    static String hashAlphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static class Mapping {

        public Mapping(char c, int val) {
            this.c = c;
            this.val = val;
        }

        char c;
        int val;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        String[] hashNames = new String[]{"sha1", "sha256", "SHA256_SPH", "MD5", "MD5_SPH"};
        String fileNameIn = "/home/ertai/Development/ParalleleSysteme/Code/Parallel-Bruteforce/ParallelBruteforce/dist/Debug/OpenMPI-Linux-x86/tbsha1";
        String fileNameOut = "/home/ertai/Development/ParalleleSysteme/Code/Parallel-Bruteforce/ParallelBruteforce/dist/Debug/OpenMPI-Linux-x86/tbsha1_sorted";

        Mapping[] map = new Mapping[hashAlphabet.length()];
        for (int i = 0; i < hashAlphabet.length(); i++) {
            map[i] = new Mapping(hashAlphabet.charAt(i), i);
        }
        HashMap<String, List<String>> hashListResult = new HashMap<>();
        String currentHashType = null;
        List<String> currentHashesList = null;
        FileInputStream fis = new FileInputStream(fileNameIn);
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(fis));
        String line;
        
        while ((line = bufferedReader.readLine()) != null) {
            if (line.trim().isEmpty()) {
                continue;
            }
            boolean isHashType=false;
            for (String s : hashNames) {
                if ((line.equalsIgnoreCase(s))) {
                    if (currentHashType != null && currentHashesList != null) {
                        hashListResult.put(currentHashType, currentHashesList);
                    }
                    currentHashType = line;
                    currentHashesList = new ArrayList<>();
                    isHashType=true;
                    break;
                }

            }
            if(isHashType)
                continue;
            if (currentHashesList != null) {
                currentHashesList.add(line);
            }
        }
        if (currentHashType != null && currentHashesList != null) {
            hashListResult.put(currentHashType, currentHashesList);
        }
        
        bufferedReader.close();
        fis.close();
        File f = new File(fileNameOut);
        
        f.createNewFile();
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(fileNameOut)));
        String currentLine;
        Set<Entry<String, List<String>>> entrySet = hashListResult.entrySet();
        for (Entry<String, List<String>> entry : entrySet) {
            Set<String> setOfList=new TreeSet<>(entry.getValue());
            List<String> val = new ArrayList<>(setOfList);
            Collections.sort(val);
            writer.write(entry.getKey() + "\n");
            for (String hash : val) {
//                final BigInteger bigInt = new BigInteger(hash, 16);
                
//                writer.write(hash +";"+ bigInt.toString(10) +"\n");
                writer.write(hash +"\n");
            }
        }
        writer.close();
    }

}

/* 
 * File:   server.h
 *      Defines a set of server (mpi master) side functions.
 *      
 * Author: Nils Frenking
 *
 * 
 * Created on 13. Januar 2015, 11:29
 */

#ifndef SERVER_H
#define	SERVER_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Simple mapping from a character to an int value. 
     */
    typedef struct AlphaValueMapping{
        /**
         * Mapping key
         */
        char alpha;
        /**
         * Value mapped to <code>alpha</code>
         */
        short value;
    };
    /**
     * Calculates the next offset password for generation.
     * The resulting password consists of elements of <code>alphabet</code>
     * @param currentPassword The initial passwort from which the next 'iteration' will be calculated from
     * @param alphabet The alphabeth of the input and output password
     * @param count The count of elements between input and output.
     * @return <code>result = currentPassword + count</code>
     */
    char* nextGenerationPassword(char* currentPassword, char* alphabet, int count);
    
    /**
     * Calculates the absolute difference between password1 and password2.
     * @param password1 
     * @param password2
     * @param alphabet The alphabet of p1 and p2.
     * @return <code>result = | password1 - password2 |</code>
     */
    long passwordDifference(char* password1, char* password2, char* alphabet);
    
    /**
     * Creates an <code>AlphaValueMapping</code> for each element in <code>alphabet</code>. 
     * The values of these mappings are in range of 0 to #alphabet-1 (first element is mapped to 0 and so on).
     * @param alphabet The input alphabet
     * @return An array of <code>AlphaValueMappings</code> with the same size as <code>alphabet</code>.
     */
    AlphaValueMapping* assignValues(char* alphabet);


#ifdef	__cplusplus
}
#endif

#endif	/* SERVER_H */


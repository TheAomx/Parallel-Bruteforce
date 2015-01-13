/* 
 * File:   plc_common.h
 *      Defines functions used on both sides client and server (mpi task and master).
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:17
 */

#ifndef PLC_COMMON_H
#define	PLC_COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Simple mapping from a character to an int value. 
     */
    typedef struct AlphaValueMapping {
        /**
         * Mapping key
         */
        char alpha;
        /**
         * Value mapped to <code>alpha</code>
         */
        unsigned short value;
    };
    /**
     * Calculates the next offset password for generation.
     * The resulting password consists of elements of <code>alphabet</code>
     * @param currentPassword The initial passwort from which the next 'iteration' will be calculated from
     * @param alphabet The alphabet of the input and output password
     * @param count The count of elements between input and output.
     * @return <code>result = currentPassword + count</code>
     */
    char* nextGenerationPassword(char* currentPassword, char* alphabet, unsigned int count);

    /**
     * Calculates the absolute difference between password1 and password2.
     * @param password1 
     * @param password2
     * @param alphabet The alphabet of p1 and p2.
     * @return <code>result = | password1 - password2 |</code>
     */
    unsigned long passwordDifference(char* password1, char* password2, char* alphabet);

    /**
     * <p>Creates an <code>AlphaValueMapping</code> for each element in <code>alphabet</code></p>. 
     * The values of these mappings are in range of 0 to #alphabet-1 (first element is mapped to 0 and so on).
     * @param alphabet The input alphabet
     * @return An array of <code>AlphaValueMappings</code> with the same size as <code>alphabet</code>.
     */
    AlphaValueMapping* assignValues(char* alphabet);

    /**
     * Calculates the number of passwords to be checked when calculating any password from length <code>1</code> to <code>maxPasswordLength</code>
     * @param alphabetLength The alphabet of the passwords.
     * @param maxPasswordLength The maximum password length.
     * @return The number of words over the alphabet with the length <code>1</code> to <code>maxPasswordLength</code>
     */
    unsigned long calcNumPasswords(unsigned long alphabetLength, unsigned long maxPasswordLength);
    

#ifdef	__cplusplus
}
#endif

#endif	/* PLC_COMMON_H */

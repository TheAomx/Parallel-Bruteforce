/* 
 * File:   password_algo.h
 * Author: TDB
 *
 * Created on 22. Januar 2015, 21:11
 */

#ifndef PASSWORD_ALGO_H
#define	PASSWORD_ALGO_H
#include "core_headers.h"
#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_PASSWORD 100

    struct mapping {
        char c;
        int val;
    };
    typedef struct mapping Mapping;

    enum PwGenAlgoType {
        DEFAULT
    };

    /*
     * From code sample:
    #define COLOR_STR(color)                            \
    (RED       == color ? "red"    :                \
     (BLUE     == color ? "blue"   :                \
      (GREEN   == color ? "green"  :                \
       (YELLOW == color ? "yellow" : "unknown"))))    
     */
#define PW_GEN_TYPE_TO_STRING(type) (DEFAULT== type ? "DEFAULT": "UNKNOWN_TYPE")



    typedef enum PwGenAlgoType PwGenAlgoType;

    typedef void (*PasswordCountCallback)(char* in, char* out);
    typedef ulong(*PasswordAbsDiffCallback)(char* pw1, char* pw2);
    typedef ulong(*PasswordValueCallback)(char* pass);
    typedef void (*PasswordAtIndexCallback)(ulong passIndex, char* result);
    typedef void (*PasswordAtRelativeIndexCallback)(char* in, ulong passIndex, char* result);
    typedef void (*PasswordDataInitCallback)();
    typedef void (*PasswordDataFreeCallback)();

    /**
     * Structure holding required data and function for executing generic kind of Password generation and calculation.
     */
    struct passwordGenerationContext {
        int type;
        char* alphabet;
        PasswordCountCallback nextPassword;
        PasswordAbsDiffCallback passwordDiff;
        PasswordValueCallback valueOf;
        PasswordAtIndexCallback passwordAt;
        PasswordAtRelativeIndexCallback passwordAtRelative;
        PasswordDataInitCallback initData;
        PasswordDataFreeCallback clearData;

    };
    typedef struct passwordGenerationContext PasswordGenerationContext;

    /**
     * Client side structure used for password generation.
     */
    struct PasswordGenTask {
        char* startPassword;
        char* endPassword;
        PasswordGenerationContext* generationContext;
    };
    typedef struct PasswordGenTask PasswordGenTask;

    /**
     * Counts up a given password using the default alphabet.
     * @param in The input password.
     * @param outPass The output password. No allocation!
     */
    void nextPass(char* in, char* outPass);

    /**
     * Get the index number for a given <code>string</code> using the default <code>alphabet</code>.
     * @param string The input to convert to a number.
     * @return The counting index number of the given <code>string</code>. 
     * It can be considered as an index value of the <code>string</code> within the password generation algorithm's generation index.
     * (That is, the input of 'aaaa' and 'aa' produce different results.)
     */
    ulong toNumberIndefaultAlphabet(char* string);

    /**
     * Calculate the number of elements between <code>pw1</code> and <code>pw2</code>
     * @param pw1
     * @param pw2
     * @return 
     */
    ulong getPasswordDiff(char* pw1, char* pw2);


    /**
     * Calculates the password which has <code>targetDifference</code> elements from <code>offsetPW</code>.
     * Or <code>result - offsetPW = targetDifference</code>
     * @param offsetPw Input. 
     * @param targetDifference The number of elements between in and out.
     * @param result Output. No allocation!
     */
    void getPasswordAtRelativeOffset(char* offsetPw, ulong targetDifference, char* result);

    /**
     * Retrieve the password located at the given index
     * @param passwordIndex The index of the password to calculate.
     * @param result The resulting password. No allocation!
     */
    void getPasswordAt(ulong passwordIndex, char* result);

    /**
     * Creates a new Password generation context of the given type.
     * @param type The type of algorithm (collection) to retrieve.
     * @return An allocated and initialized PasswordGenerationContext of the supplied type.
     */
    PasswordGenerationContext* createPasswordGenerationContextByType(int type);
    /**
     * Creates a new Password generation context of the given type name.
     * @param type The name of the type of algorithm (collection) to retrieve.
     * @return An allocated and initialized PasswordGenerationContext of the supplied type name.
     */
    PasswordGenerationContext* createPasswordGenerationContextByName(char* typeName);


    PasswordGenTask* createClientTask(int pwGenAlgoType, char* start, char* end);
#ifdef	__cplusplus
}
#endif

#endif	/* PASSWORD_ALGO_H */


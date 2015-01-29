/* 
 * File:   password_algo.c
 * Author: TDB
 *
 * Created on 22. Januar 2015, 21:11
 */
#include "core_headers.h"

/**
 * The default alphabet used.
 */
static char defaultAlphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"};
/**
 * The length of the default alphabet.
 */
static int alphabetLength = 0;
/**
 * An array of pre calculated offset values used for conversion.
 */
static ulong* pregeneratedOffsets = NULL;
/**
 * The length of predefined offset values.
 */
static int pregeneratedLen = 0;
/**
 * An array of mappings of int to char.
 */
static Mapping* alphabetMapping = NULL;

/**
 * The key which is mapped to zero.
 */
static char nullChar;

/**
 * Inverts the given string while assuming the string length == <code>len</code>.
 * @param s The string to be inverted.
 * @param len The length of the string.
 */
static inline void reverse(char s[], int len) {
    int c, i, j;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/**
 * Creates the default mapping of alphabet chars to integer values.
 * @param alphabet The alphabet to create the mapping for.
 * @param outPtr The resulting alphabet.(Double pointer, since we allocating the result)
 */
static Mapping* getMapping(const char* alphabet) {
    int alphabetLen = alphabetLength;
    Mapping *mapping = (Mapping*) malloc(sizeof (Mapping) * alphabetLen);
    if (mapping == NULL) {
        DBG_ERR("Memory allocation did not complete successfully!");
    }
    for (int i = 0; i < alphabetLen; i++) {
        mapping[i].c = alphabet[i];
        mapping[i].val = i;
    }
    return mapping;
}

/**
 * Retrieve the key mapped to a given <code>value</code> from the specified mapping.
 * @param mappings The mapping to search for <code>value</code>.
 * @param value The value, which mapped key to receive.
 * @return The key from <code>mappings</code> mapped to <code>value</code>.
 */
static char getKey(Mapping* mappings, int val) {
    char result; /* Default value... */
    int len = alphabetLength;
    result = mappings[val].c;
    return result;
}

void initializeGlobals(char* alphabet) {
    alphabetLength = strlen(alphabet);
    DBG_OK("Initializing password generation with alphabet: %s, %d", alphabet, alphabetLength);
    alphabetMapping = getMapping(defaultAlphabet);
    nullChar = getKey(alphabetMapping, 0);
    pregeneratedOffsets = (ulong*) malloc(sizeof (ulong) * MAX_PASSWORD);
    ulong result = 0;
    pregeneratedOffsets[0] = 0;
    pregeneratedOffsets[1] = 0;
    for (int i = 0; i < 20; i++) {
        result += pow_ul(alphabetLength, (ulong) (i + 1));
        pregeneratedOffsets[i + 2] = result;
    }
    pregeneratedLen = 22;
}

void freeGlobals() {
    if (alphabetMapping != NULL) {
        free(alphabetMapping);
    }
    if (pregeneratedOffsets != NULL) {
        free(pregeneratedOffsets);
    }
}

/**
 * Retrieve the value mapped to a given <code>key</code> from the specified mapping.
 * @param mappings The mapping to search for <code>key</code>.
 * @param key The key, which mapped value to receive.
 * @return The value from <code>mappings</code> mapped to <code>key</code>.
 */
static int getValue(Mapping* mappings, char key) {
    int len = alphabetLength;
    for (int j = 0; j < len; j++) {
        if (mappings[j].c == key) {
            return mappings[j].val;
        }
    }
    return -1;
}

/**
 * Retrieve the mapping element with the highest value from <code>mappings</code>.
 * @param mappings The array of mappings
 * @return The mapping element with the highest value.
 */
static Mapping getMax(Mapping* mappings) {
    Mapping result;
    result.c = '\0';
    result.val = 0;
    int currentMax = 0;
    int len = alphabetLength;
    for (int j = 0; j < len; j++) {
        if (mappings[j].val >= currentMax) {
            result = mappings[j];
        }
    }
    return result;
}

/**
 * Converts the given <code>number</code> to a string.
 * @param number The number to convert.
 * @param alphabetLen The length of the alphabet (and the supplied <code>mapping</code>) used.
 * @param mapping The mapping array of the alphabet.
 * @param outLen The length of the result.
 * @param out The resulting string.
 */
static inline void convert(ulong number, int alphabetLen, Mapping* mapping, int* outLen, char* out) {
    char rem;
    int i = 0;
    ulong numTmp = number;
    int remNum;
    
    while (numTmp != 0) {
        remNum = numTmp % alphabetLen;
        rem = mapping[remNum].c;
        numTmp /= alphabetLen;
        out[i] = rem;
        i++;
        *outLen = i;
    }
    out[*outLen] = '\0';
    reverse(out, *outLen);
}

/**
 * Converts the given <code>number</code> to a string.
 * @param number The number to convert.
 * @param mapping The mapping array of the alphabet.
 * @param out The resulting string.
 * @param nullChar The character mapped to the value zero.
 * @param outLen The length of the result.
 */
static inline void toString(ulong number, Mapping* mapping, char* out, char nullChar, int *outLen) {

    Mapping *map = mapping;
    if (number == 0) {
        out[0] = nullChar;
    } else {
        convert(number, alphabetLength, map, outLen, out);
    }

}

/**
 * Converts the given <code>number</code> to a string with the provided minimum length.
 * @param number
 * @param mapping
 * @param minLen
 * @param out
 */
static inline void toStringWithMinLen(ulong number, Mapping* mapping, int minLen, char* out) {
    Mapping *map = mapping;
    if (number == 0) {
        memset(out, nullChar, sizeof (char)*minLen);
        return;
    }
    int outlen = 0;
    toString(number, map, out, nullChar, &outlen);

    if (outlen < minLen) {
        int lenDif = minLen - outlen;
        char* tmp = (char*) malloc(sizeof (char)*outlen);

        memcpy(tmp, out, outlen);
        //            strncpy(tmp, out, outlen);
        memset(out, '\0', sizeof (char)*minLen);
        memset(out, nullChar, sizeof (char)*minLen);
        memcpy((out + (lenDif)), tmp, outlen);
        //            strncpy((out + (lenDif)), tmp, outlen);
        free(tmp);
    }
    
}


/**
 * Calculates a number from the given string. The result of this function is the 
 * representation of the given <code>string</code> consisting of elements the given <code>alphabet</code> expressed as a number with base 10.
 * 
 * This method is used for internal password counting. 
 * @param alphabet The alphabet of the password
 * @param string The password string
 * @return The value represented by the given <code>string</code>. 
 * <strong>Note:</strong> ('a' is mapped to 0) the return value for 'aaabc' and 'bc' are equal
 */
static ulong toNumber(char* string, Mapping* mapping) {
    int alphabetLen = alphabetLength;
    int inputLen = strlen(string);
    Mapping *map = mapping;


    ulong result = 0;
    int currentPosition = 0;
    ulong currentValue = 0;
    for (int i = inputLen - 1; i >= 0; i--, currentPosition++) {

        char currentLetter;
        currentLetter = string[i];
        currentValue = ((ulong) getValue(map, currentLetter)) * pow_ul(alphabetLen, currentPosition);
        result += currentValue;
    }

    return result;
}

ulong toNumberIndefaultAlphabet(char* string) {
    ulong result = toNumber(string, alphabetMapping) + pregeneratedOffsets[strlen(string)];

    return result;
}

/**
 * Determines if the given number is the highest value for a password of this length.
 * This is done by checking that each digit of <code>in</code> is the one element 
 * of <code>map</code>, which is mapped to the highest value.
 * @param in 
 * @param len Length value to check against. Must be lower than in.length.
 * @param map
 * @return <code>1</code> if <code>in</code> denotes the highest value for the given length. Otherwise <code>0</code> is returned.
 */
static char isMaxPwForLen(char* in, int len, Mapping* map) {
    char maxKey = getMax(map).c;
    int i = 0;
    while (i < len) {
        if (in[i] != maxKey)

            return 0;
        i++;
    }
    return 1;
}

/**
 * Counts up the given input by one and return the result in <code>outPtr</code>.
 * @param in
 * @param outPtr
 * @param inputLen
 */
static void countUp(char* in, char* outPtr, int inputLen) {
    Mapping *map = alphabetMapping;
    ulong inputValue = toNumber(in, map);
    if (isMaxPwForLen(in, inputLen, map) == 1) {
        /* 
         * If the current input represents the maximum number for a digit of this length,
         * set the value to 0 and length+=1
         */
        toStringWithMinLen(0, map, inputLen + 1, outPtr);
        DBG_OK("Max input value for len detected: %s -> %s", in, outPtr);
    } else {

        toStringWithMinLen(inputValue + 1, map, inputLen, outPtr);
    }
    free(map);
}

void nextPass(char* in, char* outPass) {

    int inlen = strlen(in);
    countUp(in, outPass, inlen);

    int outlen = strlen(outPass);
    if (inlen - outlen < 0) {

        DBG_OK("Overflow detected from %d -> %d digits", inlen, outlen);
        DBG_OK("in -> %s, out -> %s", in, outPass);
    }
}

ulong getPasswordDiff(char* beginPW, char *endPW) {
    ulong beginPWAsNumber = toNumberIndefaultAlphabet(beginPW);
    ulong endPWAsNumber = toNumberIndefaultAlphabet(endPW);
    
    DBG_OK("beginPW as number: %lu, endPW as number: %lu", beginPWAsNumber, endPWAsNumber);

    return (beginPWAsNumber < endPWAsNumber) ? endPWAsNumber - beginPWAsNumber : beginPWAsNumber - endPWAsNumber;
}

void getPasswordAtRelativeOffset(char* offsetPw, ulong targetDifference, char* result) {

    long offsetPwIndex = toNumberIndefaultAlphabet(offsetPw);
    long resultingIndex = targetDifference + offsetPwIndex;
    getPasswordAt(resultingIndex, result);


}

#define offsetFromPregenerated(offt) (offt<=1)?0:pregeneratedOffsets[offt-2]
#define getOffset(pwLen) pregeneratedOffsets[pwLen] 

static inline int getPasswordLength(ulong passwordIndex) {
    int passwordLength = 1;
    
    while (getOffset(passwordLength) <= passwordIndex) {
        passwordLength++;
    }
    
    passwordLength--;
    return passwordLength;
}

void getPasswordAt(ulong passwordIndex, char* result) {
    Mapping *map = alphabetMapping;
    int i = 0;
    ulong relativeIndex = 0;
    
    int passwordLength = getPasswordLength(passwordIndex);
    relativeIndex = passwordIndex - pregeneratedOffsets[passwordLength];

    toStringWithMinLen(relativeIndex, map, passwordLength, result);
}

/**
 * Create the default type of <code>PasswordGenerationContext</code> used by the brute force algorithm.
 * @return 
 */
static PasswordGenerationContext* createDefaultContext() {
    PasswordGenerationContext* result = NULL;
    alphabetLength = strlen(defaultAlphabet);
    result = (PasswordGenerationContext*) malloc(sizeof (PasswordGenerationContext));
    result->alphabet = (char*) malloc(sizeof (char)*alphabetLength);
    memset(result->alphabet, '\0', sizeof (char)*alphabetLength);
    strncpy(result->alphabet, defaultAlphabet, sizeof (char)*alphabetLength);
    result->nextPassword = nextPass;
    result->passwordDiff = getPasswordDiff;
    result->passwordAt = getPasswordAt;
    result->passwordAtRelative = getPasswordAtRelativeOffset;
    result->valueOf = toNumberIndefaultAlphabet;
    result->type = DEFAULT;
    result->initData = initializeGlobals;
    result->clearData = freeGlobals;

    return result;
}

/**
 * Create the default type of <code>PasswordGenerationContext</code> used by the brute force algorithm.
 * @return 
 */
PasswordGenerationContext* createDefaultContextWithAlphabet(char* alphabet) {
    PasswordGenerationContext* result = NULL;
    int alphaLen = strlen(alphabet);
    result = (PasswordGenerationContext*) malloc(sizeof (PasswordGenerationContext));
    result->alphabet = (char*) malloc(sizeof (char)*alphaLen);
    memset(result->alphabet, '\0', sizeof (char)*alphaLen);
    strncpy(result->alphabet, alphabet, sizeof (char)*alphaLen);
    result->nextPassword = nextPass;
    result->passwordDiff = getPasswordDiff;
    result->passwordAt = getPasswordAt;
    result->passwordAtRelative = getPasswordAtRelativeOffset;
    result->valueOf = toNumberIndefaultAlphabet;
    result->type = DEFAULT;
    result->initData = initializeGlobals;
    result->clearData = freeGlobals;

    return result;
}

static PasswordGenerationContext* createContext(char* alphabet,
    PasswordDataInitCallback dataInitFunction, PasswordDataFreeCallback dataFreeFunction,
    PasswordCountCallback nextPasswordFunction, PasswordAtIndexCallback passwordAtFunction,
    PasswordAtRelativeIndexCallback passwordAtRelativeFunction, PasswordValueCallback passwordValueOfFunction, PasswordAbsDiffCallback passwordAbsDiffFunction) {
    PasswordGenerationContext* result = NULL;
    int alphaLen = strlen(alphabet);
    result = (PasswordGenerationContext*) malloc(sizeof (PasswordGenerationContext));
    result->alphabet = (char*) malloc(sizeof (char)*alphaLen);
    memset(result->alphabet, '\0', sizeof (char)*alphaLen);
    strncpy(result->alphabet, alphabet, sizeof (char)*alphaLen);
    result->nextPassword = nextPasswordFunction;
    result->passwordDiff = passwordAbsDiffFunction;
    result->passwordAt = passwordAtFunction;
    result->passwordAtRelative = passwordAtRelativeFunction;
    result->valueOf = passwordValueOfFunction;
    result->type = 1000;
    result->initData = dataInitFunction;
    result->clearData = dataFreeFunction;

    return result;
}

PasswordGenerationContext* createPasswordGenerationContextByType(int type) {
    PasswordGenerationContext* result = NULL;
    switch (type) {
        case DEFAULT:
            result = createDefaultContext();

            break;
        default:
            DBG_OK("Error unknown type of password generation algorithm defined.");
            exit(EXIT_FAILURE);
    }
    return result;
}

PasswordGenerationContext* createCustomPasswordGenerationContext(char* alphabet,
    PasswordDataInitCallback dataInitFunction,
    PasswordDataFreeCallback dataFreeFunction,
    PasswordCountCallback nextPasswordFunction,
    PasswordAtIndexCallback passwordAtFunction,
    PasswordAtRelativeIndexCallback passwordAtRelativeFunction,
    PasswordValueCallback passwordValueOfFunction,
    PasswordAbsDiffCallback passwordAbsDiffFunction) {
    PasswordGenerationContext* result = NULL;

    result = createContext(alphabet, dataInitFunction, dataFreeFunction,
        nextPasswordFunction, passwordAtFunction, passwordAtRelativeFunction,
        passwordValueOfFunction, passwordAbsDiffFunction);

    return result;
}

PasswordGenerationContext* createPasswordGenerationContextByName(char* typeName) {
    PasswordGenerationContext* result = NULL;
    if (strcmp(typeName, "DEFAULT") == 0 || strcmp(typeName, "default") == 0) {
        result = createDefaultContext();
    } else {
        DBG_OK("Error unknown type ('%s') of password generation algorithm defined.", typeName);
        exit(EXIT_FAILURE);
    }
    return result;
}
/* 
 * File:   password_algo.c
 * Author: TDB
 *
 * Created on 22. Januar 2015, 21:11
 */
#include "core_headers.h"


static char defaultAlphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"};
static int alphabetLength = 0;
static ulong* pregeneratedOffsets = NULL;
static int pregeneratedLen = 0;
static Mapping* alphabetMapping = NULL;

static void reverse(char s[], int len) {
    int c, i, j;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

static void getMapping(const char* alphabet, Mapping** outPtr) {
    int alphabetLen = alphabetLength;
    *outPtr = NULL;
    *outPtr = (Mapping*) malloc(sizeof (Mapping) * alphabetLen);
    if (*outPtr == NULL) {
        free(*outPtr);
        DBG_OK("Memory allocation did not complete successfully!");
        *outPtr = NULL;
        return;
    }
    for (int i = 0; i < alphabetLen; i++) {
        (*outPtr)[i].c = alphabet[i];
        (*outPtr)[i].val = i;
    }
}

void initializeGlobals() {

    alphabetLength = strlen(defaultAlphabet);
    getMapping(defaultAlphabet, &alphabetMapping);

    pregeneratedOffsets = (ulong*) malloc(sizeof (ulong) * MAX_PASSWORD);
    ulong result = 0;
    for (int i = 0; i < 20; i++) {
        result += pow_ul(alphabetLength, (ulong) (i + 1));
        pregeneratedOffsets[i] = result;
    }
    pregeneratedLen = 20;
}

void freeGlobals() {
    if (alphabetMapping != NULL)
        free(alphabetMapping);
    if (pregeneratedOffsets != NULL);
    free(pregeneratedOffsets);
}

static int getValue(Mapping* mappings, char key) {
    int result = -1; /* Default value... */
    int len = alphabetLength;
    for (int j = 0; j < len; j++) {
        if (mappings[j].c == key) {
            result = mappings[j].val;
            break;
        }
    }
    return result;
}

static char getKey(Mapping* mappings, int val) {
    char result; /* Default value... */
    int len = alphabetLength;
    result = mappings[val].c;
    return result;
}

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

static void convert(ulong number, int alphabetLen, Mapping* mapping, int* outLen, char* out) {
    char rem;
    int i = 0;
    ulong numTmp = number;
    int remNum;
    while (numTmp != 0) {
        remNum = numTmp % alphabetLen;
        rem = mapping[remNum].c; //getKey(mapping, remNum);
        numTmp /= alphabetLen;
        out[i] = rem;
        i++;
        *outLen = i;
    }
    out[*outLen] = '\0';
    reverse(out, *outLen);
}

static void toString(ulong number, Mapping* mapping, char* out, char nullChar) {
    int alphabetLen = alphabetLength;
    Mapping *map = mapping;
    if (number == 0) {
        out[0] = nullChar;
    } else {
        int outLen = 0;
        convert(number, alphabetLen, map, &outLen, out);
    }

}

static void toStringWithMinLen(ulong number, Mapping* mapping, int minLen, char* out) {
    Mapping *map = mapping;
    char nullChar = '\0';

    nullChar = getKey(map, 0);
    if (number == 0) {
        memset(out,nullChar,sizeof(char)*minLen);
    } else {

        toString(number, map, out, nullChar);

        int outlen = strlen(out);
        if (outlen < minLen) {
            int lenDif = minLen - outlen;
            char* tmp = (char*) malloc(sizeof (char)*outlen);
            
            strncpy(tmp, out, outlen);
            memset(out, '\0', sizeof (char)*minLen);
            memset(out,nullChar,sizeof(char)*minLen);
            strncpy((out + (lenDif)), tmp, outlen);
            free(tmp);
        }
    }


}

/**
 * Calculates the offset for a password with a given length. (That is because 'aaba' != 'ba' but val('aaba')==val('ba'))
 * @param numberLen Length of the password
 * @return 
 */
static ulong numberOffset(int numberLen) {
    if (numberLen <= 1)
        return 0;

    if (pregeneratedLen >= numberLen) {
        return pregeneratedOffsets[numberLen - 2];
    }
    DBG_OK("regeneratedOffsets not containing value for len %d", numberLen);
    ulong alphabetLen = alphabetLength;
    ulong result = 0;
    for (int i = 1; i < numberLen; i++) {
        result += pow_ul(alphabetLen, i);
        pregeneratedOffsets[i - 1] = result;
    }
    pregeneratedLen = numberLen;
    return result;
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

    initializeGlobals();
    ulong result = toNumber(string, alphabetMapping) + numberOffset(strlen(string));
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

static void countUp(char* in, char* outPtr, int inputLen) {
    Mapping *map = NULL;
    getMapping(defaultAlphabet, &map);
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
    if (alphabetLength == 0) {
        alphabetLength = strlen(defaultAlphabet);
    }
    int inlen = strlen(in);
    countUp(in, outPass, inlen);

    int outlen = strlen(outPass);
    if (inlen - outlen < 0) {
        DBG_OK("Overflow detected from %d -> %d digits", inlen, outlen);
        DBG_OK("in -> %s, out -> %s", in, outPass);
    }
}

ulong getPasswordDiff(char* pw1, char* pw2) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(defaultAlphabet);
    }
    ulong pw1Idx = toNumberIndefaultAlphabet(pw1);
    ulong pw2Idx = toNumberIndefaultAlphabet(pw2);
    return (pw1Idx < pw2Idx) ? pw2Idx - pw1Idx : pw1Idx - pw2Idx;
}

void getPasswordAtRelativeOffset(char* offsetPw, ulong targetDifference, char* result) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(defaultAlphabet);
    }
    long offsetPwIndex = toNumberIndefaultAlphabet(offsetPw);
    long resultingIndex = targetDifference + offsetPwIndex;
    getPasswordAt(resultingIndex, result);


}

static ulong getOffsetForLen(int pwLength) {
    if (pwLength <= 1)
        return 0;
    if (pregeneratedLen >= pwLength) {
        return pregeneratedOffsets[pwLength - 2];
    }

    int alphabetLen = alphabetLength;
    ulong result = 0;
    for (int i = 1; i < pwLength; i++) {
        result += pow_ul((ulong) alphabetLen, (ulong) i);
        pregeneratedOffsets[i - 1] = result;
    }
    pregeneratedLen = pwLength;
    return result;
}

void getPasswordAt(ulong passwordIndex, char* result) {
    Mapping *map = alphabetMapping;

    long resultingIndex = passwordIndex;
    if (passwordIndex < 0) {
        toStringWithMinLen(0, map, 1, result);
        return;
    }
    if (resultingIndex > 0) {
        int i = 0;
        ulong tmp = 0;
        int pos = 1;

        while (1) {
            if (getOffsetForLen(pos) > resultingIndex) {
                if (pos > 0 && getOffsetForLen(pos - 1) <= resultingIndex)
                    pos--;
                break;
            }
            pos++;
        }
        ulong offForPos = getOffsetForLen(pos);
        tmp = resultingIndex - offForPos;
        if (resultingIndex > 0 && tmp <= 0 && pos > 0) {
            if (resultingIndex == offForPos) {
                toStringWithMinLen(0, map, pos, result);
            } else {
                pos--;
                tmp = (getOffsetForLen(pos + 1) - offForPos) - 1;
                toStringWithMinLen(tmp, map, pos, result);
            }
        } else {
            toStringWithMinLen(tmp, map, pos, result);
        }
    } else {
        result[0] = getKey(map, 0);
    }

}

static PasswordGenerationContext* createDefaultContext() {
    PasswordGenerationContext* result = NULL;
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
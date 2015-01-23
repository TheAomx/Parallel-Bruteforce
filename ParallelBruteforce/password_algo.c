/* 
 * File:   password_algo.c
 * Author: TDB
 *
 * Created on 22. Januar 2015, 21:11
 */
#include "core_headers.h"


static char defaultAlphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\0"};
static int alphabetLength = 0;

static void reverse(char s[]) {
    int length = strlen(s);
    int c, i, j;

    for (i = 0, j = length - 1; i < j; i++, j--) {
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
    for (int j = 0; j < len; j++) {
        if (mappings[j].val == val) {
            //            DBG_OK("Found mapping of value: %d to key: %c",val,mappings[j].c);
            result = mappings[j].c;
            break;
        }
    }
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
    uint alphaLen = alphabetLen;
    ulong numTmp = number;
    uint remNum;
    while (numTmp != (ulong) 0) {
        remNum = numTmp % alphaLen;

        rem = getKey(mapping, remNum);

        numTmp /= alphaLen;
        out[i] = rem;
        i++;
        *outLen = i;
    }

    out[*outLen] = '\0';

    reverse(out);
}

static void toString(ulong number, Mapping* mapping, char* out) {
    int alphabetLen = alphabetLength;

    Mapping *map = mapping;
    char nullChar = '\0';
    if (map == NULL) {
        DBG_OK("Unable to create alphabet mapping");
        exit(EXIT_FAILURE);
    }
    nullChar = getKey(map, 0);
    if (number == 0) {
        out[0] = nullChar;
        out[1] = '\0';
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

        for (int i = 0; i < minLen; i++) {
            out[i] = nullChar;
        }
        out[minLen] = '\0';

    } else {

        toString(number, map, out);
        //        DBG_OK("Converted num %ld to %s", number, out);
        int outlen = strlen(out);
        if (outlen < minLen) {
            int lenDif = minLen - outlen;
            char* tmp = (char*) malloc(sizeof (char)*outlen);
            strncpy(tmp, out, outlen);

            memset(out, '\0', sizeof (char)*minLen);
            for (int i = 0; i < minLen; i++) {
                out[i] = nullChar;
            }
            strncpy((out + (lenDif)), tmp, outlen);
            out[minLen] = '\0';
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
    int len = alphabetLength;
    ulong result = 0;
    if (numberLen == 1)
        return result;


    for (int i = numberLen - 1; i > 0; i--) {
        result += pow_ul(((ulong) len), ((ulong) i));
    }
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
    Mapping *map = NULL;
    getMapping(defaultAlphabet, &map);
    ulong result = toNumber(string, map) + numberOffset(strlen(string));
    free(map);
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
    if (pwLength == 1)
        return 0;
    ulong alphabetLen = alphabetLength;
    ulong result = 0;
    for (ulong i = 1; i < pwLength; i++) {
        result += pow_ul(alphabetLen, i);
    }
    return result;
}

void getPasswordAt(ulong passwordIndex, char* result) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(defaultAlphabet);
    }
    Mapping *map = NULL;
    getMapping(defaultAlphabet, &map);
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
        tmp = resultingIndex - getOffsetForLen(pos);
        if (resultingIndex > 0 && tmp <= 0 && pos > 0) {
            if (resultingIndex == getOffsetForLen(pos)) {
                toStringWithMinLen(0, map, pos, result);
            } else {
                pos--;
                tmp = (getOffsetForLen(pos + 1) - getOffsetForLen(pos)) - 1;
                toStringWithMinLen(tmp, map, pos, result);
            }
        } else {
            toStringWithMinLen(tmp, map, pos, result);
        }
    } else {
        result[0] = getKey(map, 0);
    }
    free(map);
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
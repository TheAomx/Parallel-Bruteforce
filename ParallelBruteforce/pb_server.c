
#include "core_headers.h"

struct mapping {
    char c;
    int val;
};
typedef struct mapping Mapping;
static char alphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\0"};
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
    int len = strlen(alphabet);
    for (int j = 0; j < len; j++) {
        if (mappings[j].c == key) {
            result = mappings[j].val;
            break;
        }
    }
    return result;
}

static char getKey(Mapping* mappings, int val) {
    char result = '\0'; /* Default value... */
    int len = strlen(alphabet);
    for (int j = 0; j < len; j++) {
        if (mappings[j].val == val) {
            result = mappings[j].c;
            break;
        }
    }
    return result;
}

static Mapping getMin(Mapping* mappings) {
    Mapping result;
    result.c = '\0';
    result.val = 0;
    int currentMin = -1;
    int len = alphabetLength;
    for (int j = 0; j < len; j++) {
        if (currentMin == -1 || mappings[j].val <= currentMin) {
            result = mappings[j];
            currentMin = mappings[j].val;
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
    int remNum;
    while (number != 0) {
        remNum = number % alphabetLen;

        rem = getKey(mapping, remNum);

        number /= alphabetLen;

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
//FIXME: The toString methods don't give the correct strings...
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
            out[minLen + 1] = '\0';
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
    int alphabetLen = strlen(alphabet);
    ulong result = 0;
    if (numberLen == 1)
        return result;


    for (int i = numberLen - 1; i > 0; i--) {
        result += pow_ul(((ulong) alphabetLen), ((ulong) i));
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
    getMapping(alphabet, &map);
    ulong result = toNumber(string, map) + numberOffset(strlen(string));
    free(map);
    return result;


}

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
    getMapping(alphabet, &map);
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

static long long getNumPasswords(char* start, char* end) {
    Mapping *map = NULL;
    getMapping(alphabet, &map);


    ulong startValue = toNumber(start, map);
    ulong endValue = toNumber(end, map);
    ulong result;
    DBG_OK("Start: %ld, End: %ld", startValue, endValue);

    if (endValue > startValue)
        result = endValue - startValue;
    else
        result = startValue - endValue;
    free(map);
    return result;
}

static void createClientTasks(int numTasks, long numPasswords, char* startPW, char* endPw, ClientTask* out, PasswordGenerationContext* pwdGenCopySource) {
    int isDivider = ((numPasswords % numTasks) == 0) ? 1 : 0;
    ulong passwordsPerTask = numPasswords / numTasks;
    DBG_OK("Calculated: Each one of the %d mpi-tasks has to generate %ld to search all %ld passwords from %s to %s", numTasks, passwordsPerTask, numPasswords, startPW, endPw);
    if (isDivider == 1) {
        DBG_OK("Is Divider!!");
    }

    ulong currentOffset = toNumberIndefaultAlphabet(startPW);

    int fullLengthTasks = (isDivider == 1) ? (numTasks) : (numTasks - 1);
    
    DBG_OK("Full tasks: %d", fullLengthTasks);
    for (int i = 0; i < fullLengthTasks; i++) {
        char* tempPwd;
    char* tempPwd2;
        tempPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        tempPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(tempPwd, '\0', sizeof (char)*MAX_PASSWORD);
        memset(tempPwd2, '\0', sizeof (char)*MAX_PASSWORD);
        out[i].pwAlgoType = DEFAULT;
        
        getPasswordAt(currentOffset, tempPwd);
        DBG_OK("Start is: %s", tempPwd);
        getPasswordAt(passwordsPerTask + currentOffset, tempPwd2);

        currentOffset += passwordsPerTask;
        out[i].startPass = tempPwd;

        out[i].endPass = tempPwd2;
        out[i].numPass = passwordsPerTask;
        out[i].passwordGenerationContext = (PasswordGenerationContext*) malloc(sizeof (pwdGenCopySource));
        memcpy(out[i].passwordGenerationContext, pwdGenCopySource, sizeof (pwdGenCopySource));


    }
    if (isDivider == 0) {
        char* tempPwd;
    char* tempPwd2;
        tempPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        tempPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(tempPwd, '\0', sizeof (char)*MAX_PASSWORD);
        memset(tempPwd2, '\0', sizeof (char)*MAX_PASSWORD);
        int index = (numTasks-1);
        out[index].pwAlgoType = DEFAULT;
        
        getPasswordAt(currentOffset, tempPwd);
        
        getPasswordAt(passwordsPerTask + currentOffset, tempPwd2);
DBG_OK("Start is: %s, end: %s", tempPwd, tempPwd2);
        currentOffset += passwordsPerTask;
        out[index].startPass = tempPwd;

        out[index].endPass = tempPwd2;
        out[index].numPass = passwordsPerTask;
        out[index].passwordGenerationContext = (PasswordGenerationContext*) malloc(sizeof (pwdGenCopySource));
        memcpy(out[index].passwordGenerationContext, pwdGenCopySource, sizeof (pwdGenCopySource));
    }

}

ServerContext* initializeWithPW(int numWorkers, char* startPW, char* endPW) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(alphabet);
    }
    ulong numPWD = getPasswordDiff(startPW, endPW);

    DBG_OK("Number of passwords to be calculated: %ld", numPWD);
    ServerContext* result = (ServerContext*) malloc(sizeof (ServerContext));
    result->startPassword = startPW;
    result->endPassword = endPW;
    result->numClients = numWorkers;
    result->numPasswords = numPWD;


    /*Initialize the password generation context which holds the alphabet and handles to methods for counting, converting and subtraction */
    PasswordGenerationContext* pwgenCtx = (PasswordGenerationContext*) malloc(sizeof (PasswordGenerationContext));
    pwgenCtx->alphabet = (char*) malloc(sizeof (char)*alphabetLength);
    memset(pwgenCtx->alphabet, '\0', sizeof (char)*alphabetLength);
    strncpy(pwgenCtx->alphabet, alphabet, sizeof (char)*alphabetLength);
    pwgenCtx->nextPassword = nextPass;
    pwgenCtx->passwordDiff = getPasswordDiff;
    pwgenCtx->valueOf = toNumberIndefaultAlphabet;
    result->passwordGenerationContext = pwgenCtx;

    result->tasks = (ClientTask*) malloc(sizeof (ClientTask) * numWorkers);
    createClientTasks(numWorkers, numPWD, startPW, endPW, result->tasks, pwgenCtx);


    return result;

}

ServerContext* initializeWithLenght(int numWorkers, char* startPW, ulong numPasswords) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(alphabet);
    }
    ulong startValue = toNumberIndefaultAlphabet(startPW);
    char* tmp = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    memset(tmp, '\0', sizeof (char)*MAX_PASSWORD);
    getPasswordAt(startValue + numPasswords, tmp);
    ulong numPWD = numPasswords;

    DBG_OK("Number of passwords to be calculated: %ld", numPWD);
    ServerContext* result = (ServerContext*) malloc(sizeof (ServerContext));
    result->startPassword = startPW;

    result->endPassword = tmp;


    result->numClients = numWorkers;
    result->numPasswords = numPWD;


    /*Initialize the password generation context which holds the alphabet and handles to methods for counting, converting and subtraction */
    PasswordGenerationContext* pwgenCtx = (PasswordGenerationContext*) malloc(sizeof (PasswordGenerationContext));
    pwgenCtx->alphabet = (char*) malloc(sizeof (char)*alphabetLength);
    memset(pwgenCtx->alphabet, '\0', sizeof (char)*alphabetLength);
    strncpy(pwgenCtx->alphabet, alphabet, sizeof (char)*alphabetLength);
    pwgenCtx->nextPassword = nextPass;
    pwgenCtx->passwordDiff = getPasswordDiff;
    pwgenCtx->valueOf = toNumberIndefaultAlphabet;
    result->passwordGenerationContext = pwgenCtx;

    result->tasks = (ClientTask*) malloc(sizeof (ClientTask) * numWorkers);
    createClientTasks(numWorkers, numPWD, startPW, tmp, result->tasks, pwgenCtx);




    return result;


}

void nextPass(char* in, char* outPass) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(alphabet);
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
        alphabetLength = strlen(alphabet);
    }
    ulong pw1Idx = toNumberIndefaultAlphabet(pw1);
    ulong pw2Idx = toNumberIndefaultAlphabet(pw2);
    return (pw1Idx < pw2Idx) ? pw2Idx - pw1Idx : pw1Idx - pw2Idx;
}

void getPasswordAtRelativeOffset(char* offsetPw, ulong targetDifference, char* result) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(alphabet);
    }

    long offsetPwIndex = toNumberIndefaultAlphabet(offsetPw);

    long resultingIndex = targetDifference + offsetPwIndex;



    getPasswordAt(resultingIndex, result);


}

void getPasswordAt(ulong passwordIndex, char* result) {
    if (alphabetLength == 0) {
        alphabetLength = strlen(alphabet);
    }
    Mapping *map = NULL;
    getMapping(alphabet, &map);
    long resultingIndex = passwordIndex;
    if (resultingIndex > 0) {
        int i = 0;
        long tmp = 0;
        int pos = 1;
        while ((resultingIndex - tmp) > 0) {
            tmp += pow_ul(alphabetLength, pos);
            pos++;
        }
        pos--;
        if (pos > 0) {
            tmp -= pow_ul(alphabetLength, pos);
        }

        tmp = resultingIndex - tmp;
        char gotresult = 0;
        while (numberOffset(i) <= resultingIndex) {
            if (numberOffset(i) == resultingIndex) {
                toStringWithMinLen(0, map, pos + 1, result);
                 DBG_OK("PW at %ld -> %s", passwordIndex, result);
                gotresult = 1;
                break;
            } else
                i++;

        }
        if (gotresult == 0) {
            toStringWithMinLen(tmp, map, pos, result);
             DBG_OK("PW at %ld -> %s", passwordIndex, result);
        }
    } else {
        result[0] = getKey(map, 0);
         DBG_OK("PW at %ld -> %s", passwordIndex, result);
    }

    free(map);

}

static void printClientTask(ClientTask task) {
    DBG_OK("Client task configuration:\r\nStart: %s, end: %s, count: %ld", task.startPass, task.endPass, task.numPass);
}

void printServerContext(ServerContext* ctx) {
    DBG_OK("Server context with %d clients", ctx->numClients);
    DBG_OK("Operation is to check %ld passwords. ->min %ld passwords per client", ctx->numPasswords, ctx->tasks[0].numPass);
    DBG_OK("Start password is: %s, end is: %s", ctx->startPassword, ctx->endPassword);

    DBG_OK("Client configurations:");
    for (int i = 0; i < ctx->numClients; i++) {
        printClientTask(ctx->tasks[i]);
    }
    DBG_OK("The used alphabet is: %s", ctx->passwordGenerationContext->alphabet);
}
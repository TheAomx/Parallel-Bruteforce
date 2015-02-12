/* 
 * File:   debug_macros.h
 * Author: julian
 *
 * Created on 10. Februar 2015, 20:09
 */

#ifndef DEBUG_MACROS_H
#define	DEBUG_MACROS_H

#ifdef	__cplusplus
extern "C" {
#endif

/* ansi color codes used at the dbg macros for coloured output. */

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* define DBG-macros for debugging purposes if DEBUG is defined...*/

#ifdef DEBUG
    #define DBG_MSG(color, stream, format, ...) do {\
                    fprintf(stream, "%sDBG:%s \"", color, KNRM);\
                    fprintf(stream, format, ##__VA_ARGS__);\
                    fprintf(stream, "\" function: %s file: %s line: %d\n",\
                    (char*) __func__, (char*)__FILE__, __LINE__);} while(0)

    #define DBG_OK(format, ...) do {\
                                DBG_MSG(KGRN, stdout, format, ##__VA_ARGS__);\
                    } while(0)
    #define DBG_WARN(format, ...) do {\
                                DBG_MSG(KYEL, stderr, format, ##__VA_ARGS__);\
                    } while(0)
    #define DBG_ERR(format, ...) do {\
                        DBG_MSG(KRED, stderr, format, ##__VA_ARGS__);\
                        exit(EXIT_FAILURE);\
                        } while(0)
#else
    #define DBG_MSG(color, stream, format, ...) do {} while(0)
    #define DBG_OK(format, ...) do {} while(0)
    #define DBG_WARN(format, ...) do {} while(0)
    #define DBG_ERR(format, ...) do {} while(0)
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_MACROS_H */


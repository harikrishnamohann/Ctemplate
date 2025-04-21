#pragma once

#include <stdlib.h>

#define ERR_BUF_SIZE 256
_Thread_local char err_buf[ERR_BUF_SIZE];

#ifndef OK
#define OK 0
#endif
#ifndef BAD
#define BAD -1
#endif
#ifndef HALT
#define HALT -2
#endif

// Clears the error buffer and returns val.
#define return_ok(buf, val) do {\
                              *buf = '\0';\
                              return val;\
                            } while(0)
//  Writes a formatted error message to buf and returns val.
#define return_err(buf, val, fmt, ...) do {\
                                         snprintf(buf, ERR_BUF_SIZE, fmt, __VA_ARGS__);\
                                         return val;\
                                       } while (0)
//  Prints the error message if present, or "OK" otherwise.
#define err_debug_print(buf) do { \
                         if (*buf) { \
                           printf("%s::{ \e[31m%s\e[0m }\n", #buf, buf); \
                         } else {\
                           printf("%s::{ \e[32mOK\e[0m }\n", #buf); \
                         } \
                       } while(0)

//  Prints the error message and exits the program with code -1 if an error exists.
#define err_debug_exit(buf) do { \
                         if (*buf) { \
                           printf("%s::{ \e[31m%s\e[0m }\nTerminated.\n", #buf, buf); \
                           exit(EXIT_FAILURE); \
                         } \
                       } while(0)

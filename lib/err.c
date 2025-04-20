#pragma once

#define ERR_BUF_SIZE 256

char err_buf[ERR_BUF_SIZE];

typedef enum {
    OK = 0,
    BAD = -1,
    HALT = -2,
} Status;

#define RETURN_OK(buf, val) do {*buf = '\0'; return val;} while(0)
#define RETURN_ERR(buf, val, fmt, ...) do { snprintf(buf, ERR_BUF_SIZE, fmt, __VA_ARGS__); return val; } while (0)
#define ERR_DEBUG(buf) do { if (*buf) printf("%s::{ \e[31m%s\e[0m }\n", #buf, buf); else printf("%s::{ \e[32mOK\e[0m }\n", #buf); } while(0)

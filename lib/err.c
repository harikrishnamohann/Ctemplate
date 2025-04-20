#pragma once

#define ERR_BUF_SIZE 256

char err_buf[ERR_BUF_SIZE];

#ifndef OK
#define OK 0
#endif
#ifndef BAD
#define BAD -1
#endif
#ifndef HALT
#define HALT -2
#endif

#define RETURN_OK(buf, val) do {*buf = '\0'; return val;} while(0)
#define RETURN_ERR(buf, val, fmt, ...) do { snprintf(buf, ERR_BUF_SIZE, fmt, __VA_ARGS__); return val; } while (0)
#define ERR_DEBUG(buf) do { if (*buf) printf("%s::{ \e[31m%s\e[0m }\n", #buf, buf); else printf("%s::{ \e[32mOK\e[0m }\n", #buf); } while(0)

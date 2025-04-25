#pragma once

#include "strings.c"
#include <unistd.h>

#include "err.c"
_Thread_local char utils_err[ERR_BUF_SIZE];

bool is_whitespace(char ch) { return (ch == ' ') || ch == '\t' || ch =='\r' || ch == '\v'; }
bool is_digit(char ch) { return (ch >= '0' && ch <= '9'); }
bool is_end_of_line(char ch) { return (ch == '\n' || ch == '\0'); }
bool is_alpha(char ch) { return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')); }

String file_to_str(char* filename) {
  if (access(filename, F_OK | R_OK) != 0) {
    return_halt(utils_err, STR_EMPTY, "file does not exist or not readable");
  }
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    fclose(fp);
    return_halt(utils_err, STR_EMPTY, "failed to open file");
  }
  String file = str_declare(STR_DYNAMIC);
  int ch;
  while ((ch = fgetc(fp)) != EOF)
    err_expect(str_err, str_insert(&file, STR_END, ch));

  fclose(fp);
  return_ok(utils_err, file);
}

int8_t str_to_file(char* filename, String content) {
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fclose(fp);
    return_halt(utils_err, HALT, "failed to open file");
  }
  size_t bytes_written = fwrite(content.str, sizeof(char), content.length, fp);
  if (bytes_written != content.length) {
    fclose(fp);
    return_bad(utils_err, BAD, "couldn't finish writing to file");
  }
  fclose(fp);
  return_ok(utils_err, OK);
}

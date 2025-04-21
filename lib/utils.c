#pragma once

#include "strings.c"
#include <unistd.h>

#include "err.c"
char utils_err_buf[ERR_BUF_SIZE];

bool is_whitespace(char ch) { return (ch == ' ') || ch == '\t' || ch =='\r' || ch == '\v'; }
bool is_digit(char ch) { return (ch >= '0' && ch <= '9'); }
bool is_end_of_line(char ch) { return (ch == '\n' || ch == '\0'); }

String file_to_str(char* filename) {
  if (access(filename, F_OK | R_OK) != 0) {
    return_err(utils_err_buf, STR_EMPTY, "%s(): %s does not exist or not readable", __FUNCTION__, filename);
  }
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    fclose(fp);
    return_err(utils_err_buf, STR_EMPTY, "%s(): failed to open %s", __FUNCTION__, filename);
  }
  String file = str_declare(STR_DYNAMIC);
  int ch;
  while ((ch = fgetc(fp)) != EOF) {
    if (str_insert(&file, STR_END, ch) != OK) {
      fclose(fp);
      return_err(utils_err_buf, STR_EMPTY, "%s(): string insertion error %s\n%s", __FUNCTION__, filename, str_err_buf);
    }
  }
  fclose(fp);
  return_ok(utils_err_buf, file);
}

int8_t str_to_file(char* filename, String content) {
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fclose(fp);
    return_err(utils_err_buf, BAD, "%s(): failed to open %s", __FUNCTION__, filename);
  }
  size_t bytes_written = fwrite(content.str, sizeof(char), content.length, fp);
  if (bytes_written != content.length) {
    fclose(fp);
    return_err(utils_err_buf, BAD, "%s(): couldn't finish writing to %s", __FUNCTION__, filename);
  }
  fclose(fp);
  return_ok(utils_err_buf, OK);
}

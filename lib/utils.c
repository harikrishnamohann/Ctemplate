#pragma once

#include "strings.c"
#include "err.c"

String file_to_str(char* filename) {
  String file = str_declare(STR_DYNAMIC);
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return file;
  }
  int ch;

  while ((ch = fgetc(fp)) != EOF) str_insert(&file, STR_END, ch);
  fclose(fp);
  return file;
}

void str_to_file(char* filename, String content) {
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    return;
  }

  fwrite(content.str, sizeof(char), content.length, fp);

  fclose(fp);
}

bool is_whitespace(char ch) { return (ch == ' ') || ch == '\t' || ch =='\r' || ch == '\v'; }
bool is_digit(char ch) { return (ch >= '0' && ch <= '9'); }
bool is_end_of_line(char ch) { return (ch == '\n' || ch == '\0'); }

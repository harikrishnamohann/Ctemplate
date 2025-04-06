#ifndef STRLIB_H
#define STRLIB_H

#include <stdint.h>
#include <stdarg.h>

typedef struct {
  char* str;
  uint64_t capacity;
  uint64_t length;
} String;

// Basic utilities
uint64_t str_len(const char* str);
String str_declare(uint64_t capacity);
String str_init(const char* s);
void str_free(String* s);

// Access and modification
char str_access(String str, int64_t index);
int str_modify(String* str, int64_t index, char ch);

// Copy and slicing
String str_dup(String s);
const String str_slice(String s, int start, int end);

// Joining and concatenation
String str_join(String a, String b);
int str_concat(String* dest, const String* src);

// Comparison
int str_cmp(const String a, const String b);

// Formatted string
String str_compose(const char* fmt, ...);

// Search
int str_contains(const String src, const String key);

// Conversions
int64_t str_to_int64(const String s);
double str_to_double(const String s);

// Replacement
int str_replace_first(String* s, int start, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length);
void str_replace_all(String* s, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length);

#define str_replace_first_using_str(src_ptr, start, search_key_str, replace_str) \
  (str_replace_first((src_ptr), (start), (search_key_str).str, (search_key_str).length, (replace_str).str, (replace_str).length))

#define str_replace_all_using_str(src_ptr, search_key_str, replace_str) \
  (str_replace_all((src_ptr), (search_key_str).str, (search_key_str).length, (replace_str).str, (replace_str).length))

#endif

/* 
This is a custom string library designed to simulate
dynamic string behavior in C. It supports a variety
of string operations including initialization, modification,
slicing, joining, comparison, replacement, and type conversions.

This implementation depends on `debug_raise_err()` for error
reporting (defined in "err.c"). Memory for dynamic strings must 
be explicitly freed by the user using `str_free()`.

Author: Harikrishna Mohan
Date: April-11-2025

## HOW TO USE ##
String str_declare(uint64_t capacity)
  -- Initializes an empty string with a specified capacity.
     Caller must free it using str_free().

String str_init(const char* s)
  -- Initializes a String from a C-style string.

char str_access(String str, int64_t index)
  -- Safely access a character using positive or negative index.

int str_modify(String* str, int64_t index, char ch)
  -- Modify character at given index. Adjusts length if needed.

String str_dup(String s)
  -- Returns a deep copy of the string. Caller must free it.

const String str_slice(String s, int start, int end)
  -- Returns a slice (view) of the string. No new memory allocated.

String str_join(String a, String b)
  -- Returns a new string as a concatenation of a and b.
     Must be freed by caller.

int str_concat(String *dest, const String src)
  -- Appends src to dest. Resizes buffer if needed.

int str_copy(String *dest, const String src)
  -- Overwrites dest with contents of src.

int str_cmp(const String a, const String b)
  -- Lexicographical comparison: 0 if equal, positive if a > b.

String str_compose(const char* fmt, ...)
  -- Like sprintf, returns a formatted String. Caller must free.

int str_contains(const String src, const String key)
  -- Returns the index of the first match of key in src or RECONSIDER.

int64_t str_to_int64(const String s)
  -- Parses and converts the string into a 64-bit integer.

double str_to_double(const String s)
  -- Parses and converts the string into a double.

int str_replace_first(String* s, int start, const char* search_key, uint32_t key_length, const char* target, uint32_t target_length)
  -- Replaces first occurrence of `search_key` starting at index `start`.

void str_replace_all(String* s, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length)
  -- Replaces all occurrences of `search_key` with `replace_with`.

void str_free(String* s)
  -- Frees memory allocated by any String constructor or mutator.

Macro APIs:
str_replace_first_using_str(str_ptr, start, key_str, target_str)
  -- Shorthand for str_replace_first() using String types.

str_replace_all_using_str(str_ptr, key_str, target_str)
  -- Shorthand for str_replace_all() using String types.
*/

#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>

#include "err.c"
#define DEBUG_ACTION WARN

typedef struct {
  char* str;
  uint64_t capacity;
  uint64_t length;
} String;

// returns the length of null terminated char*.
uint64_t str_len(const char* str) {
  uint64_t len = 0;
  while (str[len] != '\0') len++;
  return len;
}

// create an empty string with given capacity.
// memory must be freed using str_free()
String str_declare(uint64_t capacity) {
  if (capacity <= 0) {
    debug_raise_err(HALT, "invalid string capacity");
  }
  String s;
  s.length = 0;
  s.capacity = capacity;
  s.str = (char*)malloc(sizeof(char) * capacity);
  if (s.str == NULL) {
    debug_raise_err(MALLOC_FAILURE, NULL);
  }
  return s;
}

// create a new string using char* with its length as capacity.
String str_init(const char* s) {
  String str = str_declare(str_len(s));
  str.length = str.capacity;
  for (int i = 0; i < str.length; i++) str.str[i] = s[i];
  return str;
}

// access character at a specified position from both positive
// and negative indices with boundry checking.
// return RECONSIDER if out of bound index is given.
char str_access(String str, int64_t index) {
  if (index < 0) {
    index = str.length + index;
  }
  if (index < 0 || index >= str.length) {
    printf("length: %lu index: %ld\n", str.length, index);
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
    return RECONSIDER;
  }
  return str.str[index];
}

// alter character at a specified position using negative or
// positive indices. return PROCEED on success and RECONSIDER on
// index out of bounds failure.
int str_modify(String* str, int64_t index, char ch) {
  if (index < 0) {
    index = str->length + index;
  }
  if (index < 0 || index >= str->capacity) { 
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
    printf("access: %lu, capacity: %lu, length: %ld\n", index, str->capacity, str->length);
    return RECONSIDER;
  }
  if (index >= str->length) {
    str->length = index + 1;
  }
  str->str[index] = ch;
  return PROCEED;
}

// create duplicate of the given string,
// user of this function is deligated to free the memory.
String str_dup(String s) {
  String dup = str_declare(s.capacity);
  if (dup.capacity == 0) {
    debug_raise_err(NULL_REFERENCE, "failed to create duplicate");
    return dup;
  }
  dup.length = s.length;
  for (int i = 0; i < s.length; i++) dup.str[i] = s.str[i];
  return dup;
}

// slices string 's' from start to end - 1 (excluding end index).
// no memory is allocated. it returns a reference of the original
// string with str pointing to the start index and length is
// set to end - start.
const String str_slice(String s, int start, int end) {
  if (end < 0) {
    end = s.length + end + 1;
  }
  if (start < 0 || end > s.length || start >= end) {
    printf("length: %lu, start: %d, end: %d\n", s.length, start, end);
    debug_raise_err(INDEX_OUT_OF_BOUNDS, "incorrect slice length");
    return (String) {NULL, 0, 0};
  }
  return (String) {
    .length = end - start,
    .str = s.str + start,
    .capacity = s.capacity,
  }; 
}

// join string a and string b and return a new string
// memory must be freed after use using str_free()
String str_join(String a, String b) {
  String result = str_declare(a.capacity + b.capacity); 
  result.length = a.length + b.length;
  int j = 0;
  for (int i = 0; i < a.length; i++, j++) result.str[j] = a.str[i];
  for (int i = 0; i < b.length; i++, j++) result.str[j] = b.str[i];
  return result;
}

// concatinate src to dest
int str_concat(String *dest, const String src) {
  dest->capacity += src.capacity;
  dest->str  = realloc(dest->str, sizeof(char) * dest->capacity);
  if(dest->str == NULL) {
    debug_raise_err(MALLOC_FAILURE, NULL);
    return -1;
  }
  for (int i = dest->length, j = 0; j < src.length; i++, j++) dest->str[i] = src.str[j];
  dest->length += src.length;
  return 0;
}

// copy src to dest
int str_copy(String *dest, const String src) {
  if (dest->capacity < src.length) {
    dest->capacity += src.length - dest->capacity;
    dest->str = realloc(dest->str, dest->capacity + 1);
    if (dest->str == NULL) {
      debug_raise_err(MALLOC_FAILURE, "realloc() failed.");
      return -1;
    }
  }
  for (int i = 0; i < src.length; i++) {
    dest->str[i] = src.str[i];
  }
  dest->length = src.length;
  return 0;
}

// lexically compares two strings
// returns 0 if both strings are same; positive if a > b,
// negative otherwise
int str_cmp(const String a, const String b) {
  int64_t diff = 0, i = 0;
  for (; i < a.length && i < b.length; i++) diff += a.str[i] - b.str[i];
  for (; i < a.length; i++) diff += a.str[i];
  for (; i < b.length; i++) diff -= b.str[i];
  return diff;
}

// this encapuslate the snprintf() function.
// it takes a formated string and its arguments as input and
// returns the composed string. Memory should be freed after use.
String str_compose(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int req_length = vsnprintf(NULL, 0, fmt, args); // calculate length of the string
    va_end(args);

    if (req_length < 0) {
        debug_raise_err(MALLOC_FAILURE, "Error in vsnprintf format");
    }

    String composed = str_declare(req_length + 1);

    va_start(args, fmt);
    vsnprintf(composed.str, composed.capacity, fmt, args);
    va_end(args);

    composed.length = req_length;
    return composed;
}

// returns the starting index of key if key is present in src.
// otherwise returns RECONSIDER
int str_contains(const String src, const String key) {
  int pos;
  for (int i = 0; i < src.length; i++) {
    for (pos = 0; pos < key.length && src.str[i + pos] == key.str[pos]; pos++);
    if (pos == key.length) {
      return i;
    }
  }
  return RECONSIDER;
}

static void print_invalid_number_err_msg(const String s, int i) {
    for (int j = 0; j < s.length; j++) {
      if (j == i) {
        printf("\033[0;31m%c\033[0m", s.str[i]);
      } else {
        printf("%c", s.str[j]);
      }
    }
    printf("\n");
    for (int j = 0; j < i; j++) printf(" ");
    printf("^ invalid character found.\n");
}

// convert the given string to int64_t
// returns converted number on success
// returns RECONSIDER on if given string contains a non-numeric
// character.
int64_t str_to_int64(const String s) {
  int8_t sign = 1;
  int i = 0, n = 1;
  int64_t result = 0;
  if (s.length == 0) return 0;

  if (s.str[0] == '-') {
    sign = -1;
    i++;
  } else if (s.str[0] == '+') {
    i++;
  }
  while (i < s.length && s.str[i] >= '0' && s.str[i] <= '9') {
    result = result * 10 + (s.str[i] - '0');
    i++;
  }
  if (i != s.length){
    debug_raise_err(ARITHMETIC_ERR, "error converting to double: invalid character found.");
    print_invalid_number_err_msg(s, i);
    return RECONSIDER;
  }
  return result * sign;
}

// convert given string to double
// returns converted number on success.
// returns RECONSIDER if a non-numeric character is found.
double str_to_double(const String s) {
  double result = 0.0;
  int8_t sign = 1;
  uint32_t i = 0;
  if (s.length == 0) return 0.0;

  if (s.str[0] == '-') {
    sign = -1;
    i++;
  } else if (s.str[0] == '+') {
    i++;
  }

  while (i < s.length && s.str[i] >= '0' && s.str[i] <= '9') {
    result = result * 10.0 + (s.str[i] - '0');
    i++;
  }
  if (i < s.length && s.str[i] == '.') {
    i++;
    double fraction = 0.1;
    while (i < s.length && s.str[i] >= '0' && s.str[i] <= '9') {
      result += (s.str[i] - '0') * fraction;
      fraction /= 10.0;
      i++;
    }
  }

  if (i != s.length){
    debug_raise_err(ARITHMETIC_ERR, "error converting to double: invalid character found.");
    print_invalid_number_err_msg(s, i);
    return RECONSIDER;
  }
  return result * sign;
}
// Replaces the first occurrence of a search_key with a replacement
// string, starting from a specified position
// returns ending position of replaced target in string s.
// returns RECONSIDER if search_key is not found or an invalid
// start index is given.
int str_replace_first(String* s, int start, const char* search_key, uint32_t key_length, const char* target, uint32_t target_length) {
  if (start < 0 || start >= s->length) {
    debug_raise_err(INDEX_OUT_OF_BOUNDS, "invalid start index");    
    return RECONSIDER;
  }

  uint32_t span_start; // to store begin index of key in s
  int8_t contains = 0;
  for (int i = start; i < s->length; i++) {
    for (span_start = 0; span_start < key_length && s->str[i + span_start] == search_key[span_start]; span_start++);
    if (span_start == key_length) {
      span_start = i;
      contains = 1;
      break;
    }
  }

  if (!contains) { // return if key is not present in s
    return RECONSIDER;
  }

  if (str_cmp((String){(char*)search_key, key_length, key_length}, (String){(char*)target, target_length, target_length}) == 0) {
    goto ret; // no need of replacement if key and value are same. just return.
  }

  uint32_t span_end = span_start + key_length - 1;
  int32_t diff = key_length - target_length;
  if (diff > 0) { // target string is shorter than search_key
    s->length -= diff;
    for (int j = span_start + target_length; j < s->length; j++) {
      s->str[j] = s->str[j + diff]; // shift characters to the left
    }
  } else if (diff < 0) { // target string is longer than search_key
    diff *= -1;
    if (s->length + diff > s->capacity) {
      s->str = realloc(s->str, sizeof(char) * (s->capacity + diff) + 1);
      if (s->str == NULL) {
        debug_raise_err(MALLOC_FAILURE, "failed to reallocate memory for replacement");
      }
      s->capacity += diff;
      s->length += diff;
      for (int j = s->length - 1; j >= span_end; j--) {
        s->str[j + diff] = s->str[j];
      } 
    }
  }

  for (int i = span_start, j = 0; j < target_length; i++, j++) {
    s->str[i] = target[j];
  }

  ret:
    return span_start + target_length - 1;
}

// macro for giving string type inputs for str_replace_first()
#define str_replace_first_using_str(str_ptr, start, key_str, target_str) \
  str_replace_first(str_ptr, start, key_str.str, key_str.length, target_str.str, target_str.length)

#define str_replace_all_using_str(str_ptr, key_str, target_str) str_replace_all(str_ptr, key_str.str, key_str.length, target_str.str, target_str.length)
void str_replace_all(String* s, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length) {
  int pos = 0;
  while ((pos = str_replace_first(s, pos, search_key, key_length, replace_with, val_length)) != -1);
}

void str_free(String* s) {
  s->capacity = 0;
  s->length = 0;
  free(s->str);
  s->str = NULL;
}

#endif

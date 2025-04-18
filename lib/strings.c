/*
 * This is a string library that I implemented for having a better time
 * messing with C. I tried to implement most of the basic String operations
 * I can think of. I also implemented some features for the love of writing
 * string parsers. To be clear, all the operations related to the offset
 * parameter of String type is built up on my limited knolowed about building
 * simple parsers.
 *
 * In this implementation, strings are not meant to be null-terminated.
 * String's length is available in the type itself for concluding the end.
 * There are two kinds of strings:
 *
 **************************************************************************
 * [IMPORTANT NOTE!] Memory management:
 * Memory allocated using any of the function that returns a String type
 * must be released by calling str_free(). 
 **************************************************************************
 *
 * Modify this library as per the requirements. For instance, if you wanna use
 * an arena allocator for handling memory, you want may edit the methods that 
 * calls malloc() and realloc().
 *
 * Author: Harikrishna Mohan
 * Date: April-11-2025
 */

#pragma once

#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "err.c"

#define STR_DYNAMIC 0
#define STR_BEGIN 0
#define STR_END -1

// scale factor for scalable strings
#define _STR_SCALE_FACTOR 2.0
#define STR_EMPTY ((String) { NULL, 0, 0, 0, 0 })

#define str_debug_print(str) (_str_debug_print(#str, &str))

typedef struct {
  char* str;
  uint64_t capacity;
  uint64_t length;
  int64_t offset;
  bool mutable;
} String;

// Calculates the length of a null-terminated C string.
// Returns the length of the string (excluding the null terminator).
uint64_t str_len(const char* str) {
  uint64_t len = 0;
  while (str[len] != '\0') len++;
  return len;
}

// Moves the internal pointer of the string by the specified relative offset.
// Returns OK on success, BAD on invalid offset.
int8_t str_offset(String* s, int64_t offset) {
  if (offset == 0) return OK;
  if (offset > (signed)s->length || offset * -1 > (signed)s->offset) {
    printf("%s(): invalid offset position\n", __FUNCTION__);
    return BAD;
  }
  s->length -= offset;
  s->offset += offset;
  s->str += offset;
  return OK;
}

// Rewinds the internal pointer of an offset string back to the beginning (offset 0).
// Returns the original offset on success, OK if the offset was already 0.
int64_t str_rewind(String* s) {
  if (s->offset == 0) return OK;
  int64_t current_offset = s->offset;
  s->length += current_offset;
  s->str -= current_offset;
  s->offset = 0;
  return current_offset;
}

// Resizes the underlying buffer of the string by multiplying the capacity with the scale factor.
// This function is primarily used internally.
// Returns OK on success, BAD if the string is a non-mutable slice, HALT on memory allocation failure.
int8_t str_scale(String* s, float scale_factor) {
  if (!s->mutable) {
    printf("%s(): can't modify a slice\n", __FUNCTION__);
    return BAD;
  }

  int64_t offset = str_rewind(s);
  s->str = realloc(s->str, (uint64_t)(s->capacity * scale_factor));
  if (s->str == NULL) {
    printf("%s(): Failed to scale string!\n", __FUNCTION__);
    return HALT;
  }
  str_offset(s, offset);

  s->capacity *= scale_factor;
  return OK;
}

// Prints detailed debug information about a String object. This is primarily for internal debugging.
void _str_debug_print(const char* var, const String* s) {
  printf("%s{ capacity:%lu, length:%lu, offset:%lu, mutable:%s, str:%p };\n", 
         var, s->capacity, s->length, s->offset, (s->mutable) ?"yes":"no", s->str);
  if (s->str == NULL) return;
  printf("%s => \"", var);
  if (s->length <= 75) {
    for (size_t i = 0; i < s->length; i++) {
      printf("%c", s->str[i]);
    }
  } else {
    for (size_t i = 0; i <= 20; i++) {
      printf("%c", s->str[i]);
    }
    printf("... ...");
    for (size_t i = s->length - 20; i < s->length; i++) {
      printf("%c", s->str[i]);
    }
  }
  printf("\"\n");
}

// Initializes an empty string with the specified initial capacity.
// `capacity = STR_DYNAMIC` enables automatic resizing.
// Returns STR_EMPTY on failure.
String str_declare(uint64_t capacity) {
  if (capacity < 0) {
    printf("%s(): invalid string capacity\n", __FUNCTION__);
    return STR_EMPTY;
  }
  String s;
  s.length = 0;
  s.offset = 0;
  if (capacity == STR_DYNAMIC) {
    capacity = 1;
  }
  s.mutable = true;
  s.capacity = capacity;
  s.str = (char*)malloc(sizeof(char) * capacity);
  if (s.str == NULL) {
    printf("%s(): malloc() failed\n", __FUNCTION__);
    return STR_EMPTY;
  }
  return s;
}

// Initializes a String from a null-terminated C string.
// Returns STR_EMPTY on failure.
String str_init(const char* s) {
  String str = str_declare(str_len(s));
  if (str.capacity == STR_EMPTY.capacity)
    return STR_EMPTY;
  str.length = str.capacity;
  for (int i = 0; i < str.length; i++) str.str[i] = s[i];
  return str;
}

// Inserts a character at the specified index (supports negative indexing).
// Automatically resizes the string if necessary.
// Returns OK on success, BAD on invalid index, HALT on memory allocation failure.
 int8_t str_insert(String* s, int64_t pos, char ch) {
  // validate pos
  if (pos < 0) { // for negative index access
    pos = s->length + pos + 1;
  }
  if (pos > s->length || pos < 0) { // check for invalid pos
    printf("%s(): invalid access positon\n", __FUNCTION__);
    return BAD;
  }
  // deal with string capacity
  if (s->length == s->capacity) { // string is full
    int8_t ret = str_scale(s, _STR_SCALE_FACTOR);
    if (ret == BAD || ret == HALT) return ret;
  }
  // assign ch to required pos
  for (int64_t i = s->length - 1; i >= pos; i--) {
    s->str[i + 1] = s->str[i];
  }  

  s->str[pos] = ch;
  s->length++;
  return OK;
}

// Removes and returns the character at the specified index (supports negative indexing).
// Returns the removed character on success, BAD on error (empty string or invalid index).
char str_remove(String* s, int64_t pos) {
  if (s->length <= 0) {
    printf("%s(): cannot perform remove from an empty string.\n", __FUNCTION__);
    return BAD;
  }
  if (pos < 0) { // normalize index
    pos = s->length + pos;
  }
  if (pos > s->length || pos < 0) { // check for invalid pos
    printf("%s(): invalid access positon\n", __FUNCTION__);
    return BAD;
  }

  char ch = s->str[pos];
  s->length--;
  for (int64_t i = pos; i < s->length; i++) {
    s->str[i] = s->str[i + 1];
  }
  return ch;
}

// Returns a deep copy of the given string. The caller is responsible for freeing the memory.
// Returns STR_EMPTY on failure.
String str_dup(const String* s) {
  String dup = str_declare(s->capacity);
  if (dup.capacity == STR_EMPTY.capacity) {
    printf("%s(): failed to create duplicate\n", __FUNCTION__);
    return STR_EMPTY;
  }
  dup.length = s->length;
  for (int i = 0; i < s->length; i++) dup.str[i] = s->str[i];
  return dup;
}

// Returns a non-owning, non-mutable reference (view) into a substring of `str`.
// No new memory is allocated. Returns STR_EMPTY on invalid slice parameters.
String str_slice(const String* s, uint64_t start, uint64_t end) {
  if (end > s->length || start >= end) {
    printf("%s(): incorrect slice length\nlength: %lu, start: %lu, end: %lu\n", __FUNCTION__, s->length, start, end);
    return STR_EMPTY;
  }
  return (String) {
    .length = end - start,
    .str = s->str + start,
    .capacity = s->capacity,
    .mutable = false,
    .offset = s->str - s->str + start,
  }; 
}

// Returns a non-owning, non-mutable reference to a substring of `s`.
// The extracted slice is moved to the beginning of the original string,
// and the original string's internal pointer is offset.
// The caller can free either the slice or the original string once.
// Returns STR_EMPTY on invalid slice parameters.
String str_slice_owned(String* s, uint64_t start, uint64_t end) {
  if (end > s->length || start >= end) {
    printf("%s(): incorrect slice length\nlength: %lu, start: %lu, end: %lu\n", __FUNCTION__, s->length, start, end);
    return STR_EMPTY;
  }

  String slice = {
    .str = s->str,
    .length = end - start,
    .capacity = end - start,
    .offset = s->offset,
    .mutable = false
  };

  for (int i = 0; i < slice.length; i++) {
    char tmp = slice.str[start + i];
    for (int j = start + i; j > i; j--) {
      slice.str[j] = slice.str[j - 1];
    }
    slice.str[i] = tmp;
  }

  if (str_offset(s, slice.length) == BAD) return STR_EMPTY;
  return slice;
}

// Returns a new string by concatenating `a` and `b`.
// The caller is responsible for freeing the memory.
// Returns STR_EMPTY on failure.
String str_join(const String *a, const String *b) {
  String result = str_declare(a->capacity + b->capacity); 
  if (result.capacity == STR_EMPTY.capacity)
    return STR_EMPTY;
  
  result.length = a->length + b->length;
  int j = 0;
  for (int i = 0; i < a->length; i++, j++) result.str[j] = a->str[i];
  for (int i = 0; i < b->length; i++, j++) result.str[j] = b->str[i];
  return result;
}

// Appends the contents of `src` to the end of `dest`.
// Automatically resizes `dest` if necessary.
// Returns OK on success, BAD if `dest` is a non-mutable slice, HALT on memory allocation failure.
int8_t str_concat(String *dest, const String* src) {
  if (!dest->mutable) {
    printf("%s(): Can't modify a slice\n", __FUNCTION__); 
    return BAD;
  }
  dest->capacity += src->capacity;

  int64_t offset = str_rewind(dest);
  dest->str  = realloc(dest->str, sizeof(char) * dest->capacity);
  if(dest->str == NULL) {
    printf("%s(): malloc failed.\n", __FUNCTION__);
    return HALT;
  }
  str_offset(dest, offset);

  for (int i = dest->length, j = 0; j < src->length; i++, j++) dest->str[i] = src->str[j];
  dest->length += src->length;
  return OK;
}

// Replaces the contents of `dest` with the contents of `src`.
// Automatically resizes `dest` if needed.
// Returns OK on success, BAD if `dest` is a non-mutable slice, HALT on memory allocation failure.
int8_t str_copy(String *dest, const String* src) {
  if (!dest->mutable) {
    printf("%s(): Can't modify a slice\n", __FUNCTION__); 
    return BAD;
  }
  if (dest->capacity < src->length) {
    dest->capacity += src->length - dest->capacity;

    int64_t offset = str_rewind(dest);
    dest->str = realloc(dest->str, dest->capacity + 1);
    if (dest->str == NULL) {
      printf("%s(): realloc() failed.\n", __FUNCTION__);
      return HALT;
    }
    str_offset(dest, offset);

  }
  for (int i = 0; i < src->length; i++) {
    dest->str[i] = src->str[i];
  }
  dest->length = src->length;
  return OK;
}

// Performs a lexicographical comparison between `a` and `b`.
// Returns 0 if equal, >0 if `a` > `b`, and <0 if `a` < `b`.
int32_t str_cmp(const String* a, const String* b) {
  for (int i = 0; i < a->length && i < b->length; i++) {
    if (a->str[i] != b->str[i]) return a->str[i] - b->str[i];
  }
  return a->length - b->length;
}

// Returns a new formatted String, similar to `sprintf`.
// The caller is responsible for freeing the memory.
// Returns STR_EMPTY on failure.
String str_compose(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int req_length = vsnprintf(NULL, 0, fmt, args); // calculate length of the string
    va_end(args);

    if (req_length < 0) {
      printf("%s(): Error in format string\n", __FUNCTION__);
      return STR_EMPTY;
    }

    String composed = str_declare(req_length + 1);
    if (composed.capacity == STR_EMPTY.capacity)
      return STR_EMPTY;

    va_start(args, fmt);
    vsnprintf(composed.str, composed.capacity, fmt, args);
    va_end(args);

    composed.length = req_length;
    return composed;
}

// Returns the index of the first occurrence of `key` within `src`,
// or BAD if `key` is not found.
int64_t str_contains(const String* src, const char* key, uint64_t key_len) {
  int pos;
  for (int i = 0; i < src->length; i++) {
    for (pos = 0; pos < key_len && src->str[i + pos] == key[pos]; pos++);
    if (pos == key_len) {
      return i;
    }
  }
  return BAD;
}

// for printing debug information in number conversions.
static void print_invalid_number_err_msg(const String* s, int i) {
    for (int j = 0; j < s->length; j++) {
      if (j == i) {
        printf("%s(): \033[0;31m%c\033[0m", __FUNCTION__, s->str[i]);
      } else {
        printf("%s(): %c", __FUNCTION__, s->str[j]);
      }
    }
    printf("%s(): \n", __FUNCTION__);
    for (int j = 0; j < i; j++) printf(" ");
    printf("^ invalid character found.\n");
}

// Converts the string `s` to an `int64_t`.
// Returns the converted integer on success, BAD if the input string is not a valid integer.
int64_t str_to_int64(const String* s) {
  if (s->length == 0) return BAD;
  int8_t sign = 1;
  int i = 0;
  int64_t result = 0;

  if (s->str[0] == '-') {
    sign = -1;
    i++;
  } else if (s->str[0] == '+') {
    i++;
  }
  while (i < s->length && s->str[i] >= '0' && s->str[i] <= '9') {
    result = result * 10 + (s->str[i] - '0');
    i++;
  }
  if (i != s->length){
    printf("%s(): error converting to double: invalid character found\n", __FUNCTION__);
    print_invalid_number_err_msg(s, i);
    return BAD;
  }
  return result * sign;
}

// Converts the string `s` to a `double`.
// Returns the converted double on success, BAD on failure (e.g., invalid input).
double str_to_double(const String* s) {
  if (s->length == 0) return BAD;
  double result = 0.0;
  int8_t sign = 1;
  uint32_t i = 0;

  if (s->str[0] == '-') {
    sign = -1;
    i++;
  } else if (s->str[0] == '+') {
    i++;
  }

  while (i < s->length && s->str[i] >= '0' && s->str[i] <= '9') {
    result = result * 10.0 + (s->str[i] - '0');
    i++;
  }
  if (i < s->length && s->str[i] == '.') {
    i++;
    double fraction = 0.1;
    while (i < s->length && s->str[i] >= '0' && s->str[i] <= '9') {
      result += (s->str[i] - '0') * fraction;
      fraction /= 10.0;
      i++;
    }
  }

  if (i != s->length){
    printf("%s(): error converting to double: invalid character found\n", __FUNCTION__);
    print_invalid_number_err_msg(s, i);
    return BAD;
  }
  return result * sign;
}

// Replaces the first occurrence of `key` within `str` (starting from `start`)
// with `target`. Automatically resizes `str` if necessary.
// Returns the index after the replacement on success, BAD on error (non-mutable slice or invalid start index),
// or if the key is not found, HALT on memory allocation failure.
int str_replace_first(String* s, int start, const char* search_key, uint32_t key_len, const char* target, uint32_t target_len) {
  if (!s->mutable) {
    printf("%s(): Cannot modify a slice\n", __FUNCTION__);
    return BAD;
  }
  if (start < 0 || start >= s->length) {
    printf("%s(): invalid start index\n", __FUNCTION__);    
    return BAD;
  }

  uint32_t span_start; // to store begin index of key in s
  int8_t contains = 0;
  for (int i = start; i < s->length; i++) {
    for (span_start = 0; span_start < key_len && s->str[i + span_start] == search_key[span_start]; span_start++);
    if (span_start == key_len) {
      span_start = i;
      contains = 1;
      break;
    }
  }

  if (!contains) { // return if key is not present in s
    return BAD;
  }

  if (str_cmp(&(String){(char*)search_key, key_len, key_len}, &(String){(char*)target, target_len, target_len}) == 0) {
    goto ret; // no need of replacement if key and value are same. just return.
  }

  uint32_t span_end = span_start + key_len - 1;
  int32_t diff = key_len - target_len;
  if (diff > 0) { // target string is shorter than search_key
    s->length -= diff;
    for (int j = span_start + target_len; j < s->length; j++) {
      s->str[j] = s->str[j + diff]; // shift characters to the left
    }
  } else if (diff < 0) { // target string is longer than search_key
    diff *= -1;
    if (s->length + diff > s->capacity) {

      int64_t offset = str_rewind(s);
      s->str = realloc(s->str, sizeof(char) * (s->capacity + diff) + 1);
      if (s->str == NULL) {
        printf("%s(): malloc failure.\n", __FUNCTION__);
        return HALT;
      }
      str_offset(s, offset);

      s->capacity += diff;
      s->length += diff;
      for (int j = s->length - 1; j >= span_end; j--) {
        s->str[j + diff] = s->str[j];
      } 
    }
  }

  for (int i = span_start, j = 0; j < target_len; i++, j++) {
    s->str[i] = target[j];
  }

  ret:
    return span_start + target_len - 1;
}

// Replaces all occurrences of `key` with `replace_with`.
int8_t str_replace_all(String* s, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length) {
  int pos = 0;
  while ((pos = str_replace_first(s, pos, search_key, key_length, replace_with, val_length)) != BAD);
  return pos;
}

// Returns a null-terminated C string. The caller is responsible for freeing the returned pointer using `free()`.
// Returns NULL on memory allocation failure.
char*  str_to_cstring(const String* s) {
  char* cstring = malloc((sizeof(char) * s->length) + 1);
  if (cstring == NULL) {
    printf("%s(): malloc failure\n", __FUNCTION__);
    return NULL;
  } 
  for (size_t i = 0; i < s->length; i++) cstring[i] = s->str[i];
  cstring[s->length] = '\0';
  return cstring;
}

// Frees the memory allocated for the string and resets metadata.
void str_free(String* s) {
  s->capacity = 0;
  s->length = 0;
  free(s->str - s->offset);
  s->offset = 0;
  s->str = NULL;
  s->mutable = false;
}

// shorthand of str_replace_first() using String types.
#define sstr_replace_first(str_ptr, start, key_str, target_str) \
  str_replace_first(str_ptr, start, (key_str)->str, (key_str)->length, (target_str)->str, (target_str)->length)

// shorthand of str_replace_all() using String types.
#define sstr_replace_all(str_ptr, key_str, target_str) \
  str_replace_all(str_ptr, key_str.str, key_str.length, target_str.str, target_str.length)

// shorthand of str_contains() using String types.
#define sstr_contains(src_str, key_str) \
  (str_contains(src_str, (key_str)->str, (key_str)->length))

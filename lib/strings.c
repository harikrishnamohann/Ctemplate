#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/err.c"
#include "../include/strings.h"

#define DEBUG_ACTION WARN

uint64_t str_len(const char* str) {
  uint64_t len = 0;
  while (str[len] != '\0') len++;
  return len;
}

String str_declare(uint64_t capacity) {
  if (capacity <= 0) {
    debug_raise_err(INVALID_SIZE_ERR, "invalid string capacity");
    return (String) {NULL, 0, 0};
  }
  String s;
  s.length = 0;
  s.capacity = capacity;
  s.str = (char*)malloc(sizeof(char) * capacity);
  if (s.str == NULL) {
    debug_raise_err(MALLOC_FAILURE, NULL);
    return (String) { .str = NULL, .capacity = 0, .length = 0 };
  }
  return s;
}

String str_init(const char* s) {
  String str = str_declare(str_len(s));
  str.length = str.capacity;
  for (int i = 0; i < str.length; i++) str.str[i] = s[i];
  return str;
}

char str_access(String str, int64_t index) {
  if (index < 0) {
    index = str.length + index;
  }
  if (index < 0 || index >= str.length) {
    printf("length: %lu index: %ld\n", str.length, index);
    debug_raise_err(ADDRESS_BOUNDRY_ERR, NULL);
    return '\0';
  }
  return str.str[index];
}

int str_modify(String* str, int64_t index, char ch) {
  if (index < 0) {
    index = str->length + index;
  }
  if (index < 0 || index >= str->capacity) { 
    debug_raise_err(ADDRESS_BOUNDRY_ERR, NULL);
    printf("access: %lu, capacity: %lu, length: %ld\n", str->capacity, str->length, index);
    return -1;
  }
  if (ch == 0) {
    str->str[index] = 0;
    str->length = index + 1;
  } else {
    if (index >= str->length) {
      str->length = index + 1;
    }
    str->str[index] = ch;
  }
  return 0;
}

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

const String str_slice(String s, int start, int end) {
  if (end < 0) {
    end = s.length + end + 1;
  }
  if (start < 0 || end > s.length || start >= end) {
    printf("length: %lu, start: %d, end: %d\n", s.length, start, end);
    debug_raise_err(ADDRESS_BOUNDRY_ERR, "incorrect slice length");
    return (String) {NULL, 0, 0};
  }
  return (String) {
    .length = end - start,
    .str = s.str + start,
    .capacity = s.capacity,
  }; 
}

String str_join(String a, String b) {
  String result = str_declare(a.capacity + b.capacity); 
  result.length = a.length + b.length;
  int j = 0;
  for (int i = 0; i < a.length; i++, j++) result.str[j] = a.str[i];
  for (int i = 0; i < b.length; i++, j++) result.str[j] = b.str[i];
  return result;
}

int str_concat(String *dest, const String *src) {
  dest->capacity += src->capacity;
  dest->str  = realloc(dest->str, sizeof(char) * dest->capacity);
  if(dest->str == NULL) {
    debug_raise_err(MALLOC_FAILURE, NULL);
    return -1;
  }
  for (int i = dest->length, j = 0; j < src->length; i++, j++) dest->str[i] = src->str[j];
  dest->length += src->length;
  return 0;
}

int str_cmp(const String a, const String b) {
  int64_t diff = 0, i = 0;
  for (; i < a.length && i < b.length; i++) diff += a.str[i] - b.str[i];
  for (; i < a.length; i++) diff += a.str[i];
  for (; i < b.length; i++) diff -= b.str[i];
  return diff;
}

String str_compose(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int req_length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (req_length < 0) {
        debug_raise_err(MALLOC_FAILURE, "Error in vsnprintf format");
        return (String){NULL, 0,0}; 
    }

    String composed = str_declare(req_length + 1);

    va_start(args, fmt);
    vsnprintf(composed.str, composed.capacity, fmt, args);
    va_end(args);

    composed.length = req_length;
    return composed;
}

int str_contains(const String src, const String key) {
  int pos;
  for (int i = 0; i < src.length; i++) {
    for (pos = 0; pos < key.length && src.str[i + pos] == key.str[pos]; pos++);
    if (pos == key.length) {
      return i;
    }
  }
  return -1;
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
    return 0.0;
  }

  return result * sign;
}

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
    return 0.0;
  }
  return result * sign;
}

int str_replace_first(String* s, int start, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length) {
  if (start < 0) {
    debug_raise_err(ADDRESS_BOUNDRY_ERR, "invalid start index");    
    return -1;
  }

  uint32_t span_start;
  int8_t contains = 0;
  for (int i = start; i < s->length; i++) {
    for (span_start = 0; span_start < key_length && s->str[i + span_start] == search_key[span_start]; span_start++);
    if (span_start == key_length) {
      span_start = i;
      contains = 1;
      break;
    }
  }

  if (!contains) {
    return -1;
  }

  if (str_cmp((String){(char*)search_key, key_length, key_length}, (String){(char*)replace_with, val_length, val_length}) == 0) {
    goto ret;
  }

  uint32_t span_end = span_start + key_length - 1;
  int32_t diff = key_length - val_length;
  if (diff > 0) {
    s->length -= diff;
    for (int j = span_start + val_length; j < s->length; j++) {
      s->str[j] = s->str[j + diff];
    }
  } else if (diff < 0) {
    diff *= -1;
    if (s->length + diff > s->capacity) {
      s->str = realloc(s->str, sizeof(char) * (s->capacity + diff) + 1);
      if (s->str == NULL) {
        debug_raise_err(MALLOC_FAILURE, "failed to reallocate memory for replacement");
        return start;
      }
      s->capacity += diff;
      s->length += diff;
      for (int j = s->length - 1; j >= span_end; j--) {
        s->str[j + diff] = s->str[j];
      } 
    }
  }

  for (int i = span_start, j = 0; j < val_length; i++, j++) {
    s->str[i] = replace_with[j];
  }

  ret:
    return span_start + val_length - 1;
}

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

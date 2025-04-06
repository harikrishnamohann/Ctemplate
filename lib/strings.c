#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  char* str;
  uint64_t capacity;
  uint64_t length;
} String;

#define WARN 0
#define TERMINATE 1
#define LOG 2

#define DEBUG_ACTION WARN

typedef enum {
  SUCCESS,
  INDEX_OUT_OF_BOUNDS,
  MALLOC_FAILURE,
  NULL_REFERENCE,
  INVALID_SIZE,
  ARITHMETIC_ERROR,
} err_t;

uint64_t str_len(const char* str) {
  uint64_t len = 0;
  while (str[len] != 0) len++;
  return len;
}

#define debug_raise_err(errcode, msg) raise_err(errcode, DEBUG_ACTION, __FILE__, __FUNCTION__, __LINE__, msg)
void raise_err(err_t code, int action, const char* file_name, const char* fn, int line, const char* msg) {
  if (code != SUCCESS) {
    printf("\033[0;31m");
    if (action == WARN) {
      printf("WARNING!");
    } else {
      printf("ERRORO!");
    }
    printf("\033[0m %s :: %s() :: line %d\n", file_name, fn, line);
  }
  switch (code) {
    case INDEX_OUT_OF_BOUNDS :
      printf("You've tried to access an invalid index.\n");
      break;
    case MALLOC_FAILURE :
      printf("memory allocation failure!\n");
      break;
    case SUCCESS : return;
    case NULL_REFERENCE :
      printf("Invalid pointer reference.\n");
      break;
    case INVALID_SIZE :
      printf("you've tried to create an entity with 0 or negative size.\n");
      break;
    case ARITHMETIC_ERROR :
      printf("An arithmetic error occured.\n");
      break;
  }
  if (msg != NULL) {
    printf("msg: %s\n", msg);
  }
  if (action == TERMINATE) {
    printf("\033[0;31mPROGRAM TERMINAED \033[0m\n");
    exit(EXIT_FAILURE);
  }
}

String str_declare(uint64_t capacity) {
  if (capacity <= 0) {
    debug_raise_err(INVALID_SIZE, "invalid string capacity");
    return (String) {NULL, 0, 0};
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
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
    return '\0';
  }
  return str.str[index];
}

int str_modify(String* str, int64_t index, char ch) {
  if (index < 0) {
    index = str->length + index;
  }
  if (index < 0 || index >= str->capacity) { 
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
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
    debug_raise_err(NULL_REFERENCE, "failed to duplicate s");
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
    debug_raise_err(INDEX_OUT_OF_BOUNDS, "incorrect slice length");
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
    debug_raise_err(ARITHMETIC_ERROR, "error converting to double: invalid character found.");
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
    debug_raise_err(ARITHMETIC_ERROR, "error converting to double: invalid character found.");
    print_invalid_number_err_msg(s, i);
    return 0.0;
  }
  return result * sign;
}

#define str_replace_first_using_str(src_ptr, start, search_key_str, replace_str) (str_replace_first(src_ptr, start, search_key_str.str, search_key_str.length, replace_str.str, replace_str.length))
int str_replace_first(String* s, int start, const char* search_key, uint32_t key_length, const char* replace_with, uint32_t val_length) {
  if (start < 0) {
    debug_raise_err(INDEX_OUT_OF_BOUNDS, "invalid start index");    
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
      s->str = realloc(s->str, sizeof(char) * (s->capacity + diff));
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

#define str_replace_all_using_str(src_ptr, start, search_key_str, replace_str) (str_replace_first(src_ptr, start, search_key_str.str, search_key_str.length, replace_str.str, replace_str.length))
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

// io

int main() {
  String a = str_init("roses are rose");
  printf("length: %lu, capacity: %lu, str: %s\n", a.length, a.capacity, a.str);
  char key[] = "rose";
  char val[] = "orange";
  str_replace_all(&a,  key, str_len(key), val, str_len(val));
  printf("length: %lu, capacity: %lu, str: ", a.length, a.capacity);
  for (int i = 0; i < a.length; i++) {
    printf("%c", str_access(a, i));
  }
  printf("\n");
  str_free(&a);
  return 0;
}

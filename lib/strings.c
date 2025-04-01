#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  const char* str;
  uint64_t length;  
} Slice;

typedef struct {
  char* str;
  uint64_t capacity;
  uint64_t length;
} String;

#define DEBUG_ACTION TERMINATE

typedef enum {
  SUCCESS,
  INDEX_OUT_OF_BOUNDS,
  MALLOC_FAILURE,
  NULL_REFERENCE,
  INVALID_SIZE,
} err_t;

typedef enum {
  WARN,
  TERMINATE,
} action_t;

uint64_t str_len(const char* str) {
  uint64_t len = 0;
  while (str[len] != 0) len++;
  return len;
}

#define debug_raise_err(errcode, msg) raise_err(errcode, DEBUG_ACTION, __FILE__, __FUNCTION__, __LINE__, msg)
void raise_err(err_t code, action_t action, const char* file_name, const char* fn, int line, const char* msg) {
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
      printf("You tried to access an invalid array index.\n");
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
  if (index < 0 || index >= str.capacity) {
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
  }
  return str.str[index];
}

void str_modify(String* str, int64_t index, char ch) {
  if (index < 0) {
    index = str->length + index;
  }
  if (index < 0 || index >= str->capacity) { 
    debug_raise_err(INDEX_OUT_OF_BOUNDS, NULL);
    printf("access: %lu, capacity: %lu, length: %ld\n", str->capacity, str->length, index);
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
}

String str_dup(String s) {
  String dup = str_declare(s.capacity);
  dup.length = s.length;
  for (int i = 0; i < s.length; i++) dup.str[i] = s.str[i];
  return dup;
}

Slice str_slice(String s, int start, int end) {
  if (end < 0) {
    end = s.length + end;
  }
  if (start < 0 || end > s.length || start >= end) {
    printf("length: %lu, start: %d, end: %d\n", s.length, start, end);
    debug_raise_err(INDEX_OUT_OF_BOUNDS, "incorrect slice length");
  }
  return (Slice) {
  .length = end - start,
  .str = s.str + start,
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

void str_concat(String *dest, const String *src) {
  dest->capacity += src->capacity;
  dest->str  = realloc(dest->str, dest->capacity);
  if(dest->str == NULL) {
    debug_raise_err(MALLOC_FAILURE, NULL);
  }
  for (int i = dest->length, j = 0; j < src->length; i++, j++) dest->str[i] = src->str[j];
  dest->length += src->length;
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
    // First pass to calculate the required length
    va_start(args, fmt);
    String composed = str_declare(sizeof(char) * vsnprintf(NULL, 0, fmt, args) + 1);
    va_end(args);

    composed.length = composed.capacity;

    va_start(args, fmt);
    vsnprintf(composed.str, composed.length, fmt, args);
    va_end(args);

    return composed;
}

int str_contains(const String src, const char* key) {
  int pos, key_len = str_len(key);
  for (int i = 0; i < src.length; i++) {
    for (pos = 0; pos < key_len && src.str[i + pos] == key[pos]; pos++);
    if (pos == key_len) {
      return i;
    }
  }
  return -1;
}

int str_to_int(const String s);
float str_to_float(const String s);
double str_to_double(const String s);
Slice str_tokenize(const String s, const char* separator);
int str_replace_next(String* s, int pos, const String key, const String val);
int str_replace_all(String* s, const String key, const String val);

void str_free(String* s) {
  s->capacity = 0;
  s->length = 0;
  free(s->str);
  s->str = NULL;
}

int main(){
  String a = str_compose("Harikrishnamohan");
  Slice s = str_slice(a, str_contains(a, "moh"), -1);
  for (int i = 0; i < s.length; i++) printf("%c", s.str[i]);
  str_free(&a);
  return 0;
}

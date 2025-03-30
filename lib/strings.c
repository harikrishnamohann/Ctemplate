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

#define debug_raise_err(errcode) raise_err(errcode, DEBUG_ACTION, __FUNCTION__, __LINE__)
void raise_err(err_t code, action_t action, const char* fn, int line) {
  if (action == WARN && code != SUCCESS) {
    printf("\033[0;31mWARNING! \033[0m");
  }
  switch (code) {
    case INDEX_OUT_OF_BOUNDS :
      printf("You tried to access an invalid array index.\n");
      break;
    case MALLOC_FAILURE :
      printf("memory allocation failure!\n");
      break;
    case SUCCESS : return;
  }
  printf("function: %s, line: %d\n", fn, line);
  if (action == TERMINATE) {
    printf("\033[0;31mPROGRAM TERMINAED \033[0m\n");
    exit(EXIT_FAILURE);
  }
}

String str_declare(uint64_t capacity) {
  String s;
  s.length = 0;
  s.capacity = capacity;
  s.str = (char*)malloc(sizeof(char) * capacity);
  if (s.str == NULL) {
    debug_raise_err(MALLOC_FAILURE);
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
    debug_raise_err(INDEX_OUT_OF_BOUNDS);
  }
  return str.str[index];
}

void str_modify(String* str, int64_t index, char ch) {
  if (index < 0) {
    index = str->length + index;
  }
  if (index < 0 || index >= str->capacity) { 
    debug_raise_err(INDEX_OUT_OF_BOUNDS);
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

Slice str_slice(String s, int start, int end);
void str_copy(String a, String b);
String str_join(String a, String b);
void str_concat(String a, String b);
int str_compare(String a, String b);
String str_compose(const char* fmt, ...);
uint64_t str_contains(const String src, const char* seq);
int str_to_int(const String s);
float str_to_float(const String s);
double str_to_double(const String s);
Slice str_tokenize(const String s, const char* separator);

void str_free(String* s) {
  s->capacity = 0;
  s->length = 0;
  free(s->str);
  s->str = NULL;
}

int main(){
  String s = str_init("hello there! how are you doing?\n");
  printf("%c", str_access(str_dup(s), 0));
  str_free(&s);
  return 0;
}

// welcome to c programing
#include <stdio.h>
#include "../lib/strings.c"

int main() {
  String s = str_declare(STR_DYNAMIC);

  for (int i = 0; i < s.length; i++) {
    printf("%c", s.str[i]);
  }
  str_free(&s);
  // printf("Welcome to C programming!\n");
  return 0;
}

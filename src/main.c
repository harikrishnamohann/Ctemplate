// welcome to c programing
#include <stdio.h>
#include "../lib/strings.c"

int main() {
  String s = str_init("hello world");
  String slice = str_owned_slice(&s, 2, 5);
  for (int i = 0; i < slice.length; i++) {
    printf("%c", slice.str[i]);
  }
  printf("\n");
  for (int i = 0; i < s.length; i++) {
    printf("%c", s.str[i]);
  }
  str_free(&s);
  // printf("Welcome to C programming!\n");
  return 0;
}

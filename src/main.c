// welcome to c programing
#include <stdio.h>
#include <stdint.h>
#include "../lib/strings.c"
#include "../lib/err.c"

int main() {
  String a = str_init("Welcome");
  String b = str_init("hello world!\n");
  str_copy(&a, b);
  for (int i=0;i<a.length; i++) { printf("%c", a.str[i]); }
  str_free(&a);
  str_free(&b);
  return 0;
}

// welcome to c programing
#include <stdio.h>
#include "../lib/strings.c"
#include "../lib/err.c"

int main() {
  String s;
  err_expect(str_err, s = str_declare(-1));
  str_debug_print(s);
  str_free(&s);
  // printf("Welcome to C programming!\n");
  return 0;
}

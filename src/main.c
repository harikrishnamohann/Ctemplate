// welcome to c programing
#include <stdio.h>
#include <stdint.h>
#include "../include/strings.h"

int main() {
  String a = str_init("Welcome");
  printf("%s\n", a.str);
  str_free(&a);
  return 0;
}

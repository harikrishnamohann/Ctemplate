// welcome to c programing
#include <stdio.h>
#include <stdint.h>
#include "../lib/strings.c"
#include "../lib/err.c"

int main() {
  String key = str_init("tool");
  String val = str_init("TOOL");
  String s = str_init("philosophy is a tool, not a destination nor a goal");
  int x = str_replace_first_using_str(&s, 1, key, val);
  x = str_replace_first(&s, x, "nor", 3, "or", 2);
  for (int i=0;i<s.length; i++) { printf("%c", s.str[i]); }
  str_free(&key);
  str_free(&val);
  str_free(&s);
  return 0;
}

#include <stdio.h>
#include "../lib/strings.c"

int main() {
  String s = err_expect(err_buf, str_init("src/main.c:6:9: error: incompatible pointer to integer conversion passing 'FILE *' (aka 'struct _IO_FILE *') to parameter of type 'int' [-Wint-conversion]\
    6 |   write(stdout, \"hello world\n\", sizeof(char) * 15);"));
  str_debug_print(s);
  str_free(&s);
  // fwrite("C is awesome ^_^\n\0", sizeof(char), 18, stdout);
  return 0;
}
    

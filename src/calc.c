#include <calc/ast.h>

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <editline/readline.h>

void rec_eval(value ret, int nested, int last) {
  switch (ret.type) {
    case Real:
      if (nested) {
        if(last) {
          printf(" %.9lf ", ret.double_val);
        } else {
          printf(" %.9lf, ", ret.double_val);
        }
      } else {
        printf("%.9lf", ret.double_val);
      }
      break;
    case Complex: {
      if (nested) {
        if(last) {
          printf(" %.9lf + %.9lfi ", creal(ret.complex_val), cimag(ret.complex_val));
        } else {
          printf(" %.9lf + %.9lfi, ", creal(ret.complex_val), cimag(ret.complex_val));
        }
      } else {
        printf("%.9lf + %.9lfi", creal(ret.complex_val), cimag(ret.complex_val));
      }
      break;
    }
    case List:
      printf("{");
      int size = Size(&ret.list_val);
      for (int i = 0; i < size; i++) {
        int last = i == size-1;
        rec_eval(*((value *)GetVector(&ret.list_val, i)),
                 /*nested*/ 1, last);
      }
      printf("}");
      break;
  }
  if(nested == 0) {
    printf("\n");
  }
}

int repl() {
    char *buffer;
    while ((buffer = readline("> "))) {
        if (buffer && *buffer) {
            value ret = evaluate(buffer);
            rec_eval(ret, /*nested*/ 0, /*last*/ 0);
            add_history(buffer);
        }
        free(buffer);
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        static struct option long_options[] = {
            {"eval", required_argument, 0, 'e'}, {0, 0, 0, 0}};
        int option_index = 0;
        int c = getopt_long(argc, argv, "e:", long_options, &option_index);

        if (c == 'e') {
            if (optarg && *optarg) {
                value ret = evaluate(optarg);
                rec_eval(ret, /*nested*/ 0, /*last*/ 0);
            }
        }

    } else {
        repl();
    }
    return 0;
}

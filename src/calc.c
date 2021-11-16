#include <calc/ast.h>

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>

void rec_eval(ReturnExpr ret, int nested) {
    switch (ret.type) {
    case Real:
        if (nested) {
            printf(" %.9lf ", ret.double_val);
        } else {
            printf("%.9lf\n", ret.double_val);
        }
        break;
   case Complex: {
        if (nested) {
          printf(" %.9lf + %.9lfi ", creal(ret.complex_val), cimag(ret.complex_val));
        } else {
            printf("%.9lf + %.9lfi\n", creal(ret.complex_val), cimag(ret.complex_val));
        }
        break;
   }
    case List:
        printf("{");
        for (int i = 0; i < Size(&ret.list_val); i++) {
            rec_eval(*((ReturnExpr *)GetVector(&ret.list_val, i)),
                     /*nested*/ 1);
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
            ReturnExpr ret = evaluate(buffer);
            rec_eval(ret, /*nested*/ 0);
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
                ReturnExpr ret = evaluate(optarg);
                rec_eval(ret, /*nested*/ 0);
            }
        }

    } else {
        repl();
    }
    return 0;
}

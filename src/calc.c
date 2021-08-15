#include <calc/eval.h>
#include <getopt.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

void rec_eval(ReturnExpr ret, int nested) {
    switch (ret.type) {
    case Number:
        if (nested) {
            printf(" %.9lf ", ret.value.double_val);
        } else {
            printf("%.9lf\n", ret.value.double_val);
        }
        break;
    case List:
        printf("{");
        for (int i = 0; i < Size(&ret.value.list_val); i++) {
            rec_eval(*((ReturnExpr *)GetVector(&ret.value.list_val, i)),
                     /*nested*/ 1);
        }
        printf("}");
        break;
    }
}

int repl() {
    char *buffer;
    while ((buffer = readline("> "))) {
        if (buffer && *buffer) {
            ReturnExpr ret = evaluate(buffer);
            rec_eval(ret, /*nested*/ 0);
            printf("\n");
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
                printf("\n");
            }
        }

    } else {
        repl();
    }
    return 0;
}

#include <stdlib.h>
#include <readline/readline.h>
#include <getopt.h>
#include <calc/eval.h>

int repl() {
    char* buffer;
    while((buffer = readline("> "))) {
        printf("%lf\n", evaluate(buffer));
        free(buffer);
    }
    return 0;
}

int main(int argc, char** argv) {
    if(argc > 1) {
        static struct option long_options[] =
        { {"eval",  required_argument, 0, 'e'}, {0, 0, 0, 0} };
        int option_index = 0;
        int c = getopt_long (argc, argv, "e:", long_options, &option_index);

        if( c == 'e' ) {
            printf("%lf\n", evaluate(optarg));
        }

    } else {
        repl();
    }
    return 0;
}

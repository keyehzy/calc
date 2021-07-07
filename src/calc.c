#include <stdlib.h>
#include <readline/readline.h>

int repl() {
    char* buffer;
    while((buffer = readline("> "))) {
        printf("%s\n", buffer);
        /* TODO handle buffer */
        free(buffer);
    }
    return 0;
}

int main(int argc, char** argv) {
    repl();
    return 0;
}

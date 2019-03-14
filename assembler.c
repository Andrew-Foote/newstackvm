#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc <= 1 || argc > 3) {
        fprintf(stderr, "Invalid number of arguments: %d (1 or 2 required)", argc - 1);
        goto fail;
    }

    FILE *ifile = fopen(argv[1], "r");

    

    fail:
    return EXIT_FAILURE;
}

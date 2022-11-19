#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>


void print_usage(char **argv) {
    printf("USAGE: %s <path_to_file_to_compile>\n", argv[0]);
}

long file_size(FILE *file) {
    
    if (!file) {  return 0; }

    fpos_t original;
    if (fgetpos(file, &original) != 0) {
        printf("fgetpos() failed: %i\n", errno);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long out = ftell(file);
    if (fsetpos(file, &original) != 0) {
        printf("fsetpos() failed: %i\n", errno);
        return 0;
    }  

    return out;

}

char *files_contents(char *path) {
    
    FILE *file = fopen(path, "r");
    
    if (!file) {
        printf("Could not open file at %s\n", path);
        return NULL;
    }

    long size = file_size(file);
    char *contents = malloc(size + 1);
    char *write_it = contents;

    size_t bytes_read = 0;
    while (bytes_read < size) {
        size_t bytes_read_this_iteration = fread(write_it, 1, size - bytes_read, file);

        if (ferror(file)) {
            printf("Error while reading: %i\n", errno);
            free(contents);
            return NULL;
        }

        bytes_read += bytes_read_this_iteration;
        write_it += bytes_read_this_iteration;
        if (feof(file)) { break; }
    }
    
    if (bytes_read) {
        printf("Read: %ld\nSize: %ld\n", bytes_read, size);
        free(contents);
        return NULL;
    }

    contents[size] = '\0';
    return contents;
}


int main (int argc, char **argv) {

    if (argc < 2) {
        print_usage(argv);
        exit(0);
    }

    char *path = argv[1];
    char *contents = files_contents(path);
    if (contents) {
        printf("Contents of %s:\n---\n%s\n---\n", path, contents);

    }
    return 0;
}

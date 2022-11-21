#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

typedef struct Error {
    enum ErrorType {
        ERROR_NONE = 0,
        ERROR_TYPE,
        ERROR_TODO,
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_ARGS,
    } type;
    char *msg;
} Error;


Error ok = { ERROR_NONE, NULL };

void print_error(Error err) 
{
    printf("ERROR: ");
    switch (err.type) {
      default:
        printf("Unknow error type...");
        break;
      case ERROR_TODO:
        printf("TODO (not implemented");
      case ERROR_SYNTAX:
        printf("Invalid syntax");
      case ERROR_TYPE:
        printf("Mismatched types");
      case ERROR_ARGS:
        printf("Invalid arguments");
      case ERROR_NONE:
        break;
      case ERROR_GENERIC:
        break;
    }

    putchar('\n');
    if (err.msg) {
        printf("     : %s\n", err.msg);
    }
}



#define ERROR_CREATE(n, t, msg)       \
    Error (n) = { (t), (msg) }

#define ERROR_PREP(n, t, message)     \
    (n).type = (t);                   \
    (n).msg = (message);


const char *whitespace = " \r";
const char *delimiters = " \r\n,():";

// Given a source, get the next token and point to it with start and end
Error lex(char *source, char **start, char **end) 
{
    Error err = ok;
    if (!source || !start || !end) {
        ERROR_PREP(err, ERROR_ARGS, "Can't lexing empty source");
        return err;
    }

    *start = source;    
    *start += strcspn(*start, whitespace);

    if (**end == '\0') {
        return err;
    }

    *end = *start;
    *end += strcspn(*start, delimiters);

    if (*end == *start) {
        *end += 1;
    }

    printf("lexed: %.*s", *end - *start, *start);
    return err;
}

Error parse_expression(char *source)
{
    char *start = source;
    char *end = source;
    Error err = ok;

    while ((err = lex(end, &start, &end)).type == ERROR_NONE) {
        if (end - start == 0) {
            break;
        }
        printf("This: %.*s\n", end - start, start);
    }
    return err;
}

void print_usage(char **argv) 
{
    printf("USAGE: %s <path_to_file_to_compile>\n", argv[0]);
}

long file_size(FILE *file) 
{
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

char *files_contents(char *path) 
{
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

        if (feof(file)) { 
            break; 
        }
    }
    
    if (bytes_read < size) {
        printf("Read: %ld\nSize: %ld\n", bytes_read, size);
        free(contents);
        return NULL;
    }

    contents[bytes_read] = '\0';
    return contents;
}

int main (int argc, char **argv) 
{
    if (argc < 2) {
        print_usage(argv);
        exit(0);
    }

    char *path = argv[1];
    char *contents = files_contents(path);
    if (contents) {
        printf("Contents of %s:\n---\n%s\n---\n", path, contents);
        free(contents);

        Error err = parse_expression(contents);
        print_error(err);
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


const char *whitespace = " \r\n";
const char *delimiters = " \r\n,():";

typedef struct Error {
    enum ErrorType {
        ERROR_NONE = 0,
        ERROR_TYPE,
        ERROR_TODO,
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_ARGS,
        ERROR_FILE,
        ERROR_MAX,
    } type;
    char *msg;
} Error;

Error ok = { ERROR_NONE, NULL };

// TODO:
// |-- API to create new node
// |-- API to add node as child
typedef long long integer_t;
typedef struct Node {
    enum NodeType {
        NODE_TYPE_NONE,
        NODE_TYPE_INTEGER,
        NODE_TYPE_PROGRAM,
        NODE_TYPE_MAX,
    } type;

    union NodeValue {
        integer_t integer;
    } value;

    struct Node **child;
} Node;

#define none_type(node) ((node).type == NODE_TYPE_NONE)
#define int_type(node) ((node).type == NODE_TYPE_INTEGER)

// TODO: 
// |-- API to create new Binding;
// |-- API to add Binding to environment;
typedef struct Binding {
    char *id;
    Node *value;
    struct Binding *next;
} Binding;

// TODO: 
// |-- API to create new environment;
typedef struct Environment {
    struct Environment *parent;
    Binding *bind;
} Environment;



void print_error(Error err) 
{   
    if (err.type == ERROR_NONE) { return; }
        
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
    if (err.msg) { printf("     : %s\n", err.msg); }
}

#define ERROR_CREATE(n, t, msg)       \
    Error (n) = { (t), (msg) }

#define ERROR_PREP(n, t, message)     \
    (n).type = (t);                   \
    (n).msg = (message);


// Given a source, get the next token and point to it with start and end
Error lex(char *source, char **start, char **end) 
{
    Error err = ok;
    if (!source || !start || !end) {
        ERROR_PREP(err, ERROR_ARGS, "Can't lexing empty source");
        return err;
    }

    *start = source;    
    *start += strspn(*start, whitespace);
    *end = *start;

    if (**end == '\0')
        return err;

    *end += strcspn(*start, delimiters);

    if (*end == *start)
        *end += 1;
    
    return err;
}


void environment_set()
{

}

/// @return zero upon success
int valid_identifier(char *id) 
{
    return strpbrk(id, whitespace) == NULL ? 0 : 1;
}

Error parse_expression(char *source, Node *result)
{
    Error err = ok;
    char *start = source;
    char *end = source;

    while ((err = lex(end, &start, &end)).type == ERROR_NONE) {
        if (end - start == 0) { break; }
        printf("lexed: %.*s\n", end - start, start);
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
        // printf("Contents of %s:\n---\n%s\n---\n", path, contents);
        Node expression;
        Error err = parse_expression(contents, &expression);
        print_error(err);
        free(contents);
    }
    return 0;
}

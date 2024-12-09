#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../src/file.c"
#define maxFilesize 1024 * 1024 * 4
char *xxout(int size, char *name, char *content) {
    const char nline[] = "\n    ";
    const char headFmt[] = "char %s[] = {\n    ";
    const char hexFmt[] = "0x%02x,";
    char *result = malloc((sizeof(char)*size)*8 + 36);
    sprintf(result, headFmt, name);
    int i; int p=strlen(result);
    for(i=0;i<size;i++) {
        p+= sprintf(result+p, hexFmt, (unsigned char)content[i]);
        if ((i+1)%16==0) { p+= sprintf(result+p, nline); }
    }
    sprintf(result + p, "0x00\n};\n");
    return result;
}

int binout(char *name, char *input, char *output) {
    char *buffer = malloc(sizeof(char)*maxFilesize);
    int code = file_read(input, buffer, sizeof(char)*maxFilesize);
    if (code == false) { printf("__internal__: could not read %s\n", input); exit(1); }
    char *xxd = xxout(code, name, buffer); free(buffer);
    return file_write(output, xxd, maxFilesize * 8 + 255);
}

int main(int argc, char **argv) {
    binout("moongin_frame", "./helpers/moongin.c", "./src/moongin.h");
    binout("moongin_list", "./helpers/list.c", "./src/list.h");
    binout("moongin_gsdl", "./helpers/gsdl.c", "./src/gsdl.h");
    binout("moongin_glua", "./helpers/glua.c", "./src/glua.h");
    binout("moongin_item", "./helpers/item.c", "./src/item.h");
    return 0;
}
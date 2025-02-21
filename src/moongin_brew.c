#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "file.c"
#include "strings.h"
#include "helpers/list.h"
#include "helpers/item.h"
#include "helpers/moongin.h"
#include "helpers/gsdl.h"
#include "helpers/glua.h"

const char target_x86win[] =   "%s --target=x86_64-w64-mingw32 -o %s output.c -luser32 -lkernel32 -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_x86linux[] = "%s --target=x86_64-linux-gnu -o %s output.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_x86mac[] =   "%s --target=x86_64-apple-darwin -o %s output.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_ARMmac[] =   "%s --target=aarch64-apple-darwin -o %s output.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_ARMlinux[] = "%s --target=aarch64-linux-gnu -o %s output.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";

int sysync(const char *command) {
    printf("    ... running %s\n", command);
    pid_t pid = fork(); // Create a child process
    if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); } 
    else if (pid == 0) { execlp("sh", "sh", "-c", command, (char *)NULL); perror("execlp"); exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid"); exit(EXIT_FAILURE);
        }   return WIFEXITED(status);
    }
}

#define maxFilesize 1024 * 1024 * 4
char *xxout(int size, char *name, char *content) {
    const char nline[] = "\n    ";
    const char headFmt[] = "char %s[] = {\n    ";
    const char hexFmt[] = "0x%02x,";
    char *result = malloc(maxFilesize*8 + 36);
    sprintf(result, headFmt, name);
    int i; int p=strlen(result);
    for(i=0;i<size;i++) {
        p+= sprintf(result+p, hexFmt, (unsigned char)content[i]);
        if ((i+1)%16==0) { p+= sprintf(result+p, nline); }
    }
    sprintf(result + p, "0x00\n};\n");
    return result;
}

char command [512];
int main(int argc, char **argv) {
    char *compiler = NULL;
    char *luafile = NULL;
    char *outname = NULL;
    char **platforms = NULL; int platnum = 0;
    int i; int code;
    if (argc<2) { printf(help$); exit(1); }
    if (strcmp(argv[1],"-h")==0 |
        strcmp(argv[1],"--help")==0|
        strcmp(argv[1],"help")==0) {
            printf(help$); exit(0);
        }
    if (strcmp(argv[1],"-v")==0 |
        strcmp(argv[1],"--version")==0|
        strcmp(argv[1],"version")==0) {
            printf(vhead$); exit(0);
        }
    if (strcmp(argv[1],"-c")==0) {
        if (argc < 6) { printf("-c expected a binary name, lua, compiler, and platforms to build for\n"); exit(1); }
        outname = argv[2];
        luafile = argv[3];
        compiler = argv[4];
        platforms = argv + 5;
        platnum = argc - 5;
    } else {
        if (argc < 4) { printf("expected a binary name, lua file, and platforms to build for\n"); exit(1); }
        compiler = "clang";
        outname = argv[1];
        luafile = argv[2];
        platforms = argv + 3;
        platnum = argc - 3;
    }
    /* Check to make sure the *.lua source file exists, then luac it */
    if (file_exists(luafile)==false) { printf("no source file \"%s\" found\n", luafile); exit(1); }
    sprintf(command, "luac -o build.luac %s", luafile);
    code = system(command); if (code != 0) { 
        printf("error compiling source file \"%s\" to lua bytecode\n", luafile); exit(1);
    }

    /* Find and read the *.luac file for reading */
    char *mainfile = malloc(sizeof(char)*maxFilesize);
    int size = file_read("build.luac", mainfile, maxFilesize*sizeof(char));
    if (size == false) { printf("unable to open bytecode for reading\n"); exit(1); }
    char *xxd = xxout(size, "lua_entry", mainfile);

    /* Then make a *.c which injects the bytecode */
    char *mainc = malloc(maxFilesize*8 + 255);
    sprintf(mainc, moongin_frame, moongin_list, moongin_item, moongin_gsdl, moongin_glua, xxd);
    file_write("./output.c", mainc, (sizeof(char)*maxFilesize)*8 + 255);

    /* finish up by compiling it for each platform */
    for(i=0;i<platnum;i++) {
        if (strcmp(platforms[i], "xlinux")==0) 
        { sprintf(command, target_x86linux, compiler, outname); sysync(command); continue; }
        if (strcmp(platforms[i], "xmac")==0) 
        { sprintf(command, target_x86mac, compiler, outname); sysync(command); continue; }
        if (strcmp(platforms[i], "xwin")==0) 
        { sprintf(command, target_x86win, compiler, outname); sysync(command); continue; }
        if (strcmp(platforms[i], "armlinux")==0) 
        { sprintf(command, target_ARMlinux, compiler, outname); sysync(command); continue; }
        if (strcmp(platforms[i], "armmac")==0) 
        { sprintf(command, target_ARMmac, compiler, outname); sysync(command); continue; }
        printf("invalid platform \"%s\"\n", platforms[i]); exit(1);
    }

    /* clean up the generated build.luac and output.c */
    file_delete("build.luac");
    // file_delete("output.c");
}
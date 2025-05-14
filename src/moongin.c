#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#define maxFilesize 4 * 1024 * 1024
#include "builder/file.h"
#include "builder/bytes.h"
#include "builder/targets.h"
#include "builder/strings.h"
#include "builder/_bundle.h" // created during project `build` step
#include "builder/_runtime.h"

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
        if (argc < 6) { printf("-c expected a compiler, binary name, lua file, and platforms to build for\n"); exit(1); }
        compiler = argv[2];
        outname = argv[3];
        luafile = argv[4];
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
    char *luabuffer = malloc( size * 8 + 256 ); bytes(luabuffer, size, "lua_entry", mainfile);

    /* Then make a *.c which injects the bytecode */
    char *mainc = malloc(maxFilesize*8 + 255);
    sprintf(mainc, runtime_frame, runtime_stb, luabuffer);
    file_write("./moongin.c", mainc, (sizeof(char)*maxFilesize)*8 + 255);

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
    }

    /* clean up the generated build.luac and moongin.c */
    file_delete("build.luac");
    file_delete("moongin.c");
}
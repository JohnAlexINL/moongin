int bytes(char *buffer, int size, char *name, char *content) {
    const char nline[] = "\n    ";
    const char headFmt[] = "char %s[] = {\n    ";
    const char hexFmt[] = "0x%02x,";
    sprintf(buffer, headFmt, name);
    int i; int p=strlen(buffer);
    for(i=0;i<size;i++) {
        p+= sprintf(buffer+p, hexFmt, (unsigned char)content[i]);
        if ((i+1)%16==0) { p+= sprintf(buffer+p, nline); }
    }
    sprintf(buffer + p, "0x00\n};\n");
    return p;
}
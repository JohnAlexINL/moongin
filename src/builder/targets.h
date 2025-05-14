const char target_x86win[] =   "%s --target=x86_64-w64-mingw32 -o %s moongin.c -luser32 -lkernel32 -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_x86linux[] = "%s --target=x86_64-linux-gnu -o %s moongin.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_x86mac[] =   "%s --target=x86_64-apple-darwin -o %s moongin.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_ARMmac[] =   "%s --target=aarch64-apple-darwin -o %s moongin.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
const char target_ARMlinux[] = "%s --target=aarch64-linux-gnu -o %s moongin.c -pthread -lSDL2 -lSDL2_ttf -lSDL2_image -llua5.4 -lm";
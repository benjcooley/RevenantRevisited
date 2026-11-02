#include "stdio.h"
#include "dos.h"
#include "stdlib.h"
#include "conio.h"
#include "io.h"
#include "direct.h"

int32_t lines = 0;

void count(char* filespec)
{
    _finddata_t fi;
    int32_t ret, done;
    FILE* fp;
    char ch;

    done = 0;
    int32_t ff = _findfirst(filespec, &fi);
    while (!done)
    {
        if (ret > -1 && ff > -1)
        {
            fp = fopen(fi.name, "rt");
            while (!feof(fp))
            {
                ch = (char)fgetc(fp);
                if (!feof(fp) && ch == '\n')
                {
                    lines++;
                    if (!(lines % 100))
                        printf("lines: %d         \r", lines);
                }
            }
            fclose(fp);
        }
        else
        {
            done = 1;
        }
        ret = _findnext(ff, &fi);
    }
}

int32_t main(void)
{
    _chdir("c:\\revenant");
    count("*.def");
    printf("lines: %d         \r", lines);
    getch();
    return 0;
}
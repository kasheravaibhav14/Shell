#include "headerlist.h"
#include "printuser.h"

void cyan()
{
    printf("\033[0;36m");
}

void green()
{
    printf("\033[1;32m");
}

void reset()
{
    printf("\033[0m");
}

void printUserHost(char *cwd, char *home, char *user, char *hostname, int n)
{
    if (strncmp(home, cwd, n) == 0)
    {
        char relAdd[100];
        int len = strlen(cwd);
        for (int i = n; i < len; i++)
        {
            relAdd[i - n] = cwd[i];
            // printf("hi");
        }
        relAdd[len - n] = '\0';
        if (n < len)
        {
            green();
            printf("<%s@%s:", user, hostname);
            cyan();
            printf("~%s", relAdd);
            green();
            printf(">");
            reset();
        }
        else
        {
            green();
            printf("<%s@%s:", user, hostname);
            cyan();
            printf("~");
            green();
            printf(">");
            reset();
        }
    }
    else
    {
        green();
        printf("<%s@%s:", user, hostname);
        cyan();
        printf("~%s", cwd);
        green();
        printf(">");
        reset();
    }
}
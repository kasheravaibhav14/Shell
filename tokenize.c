#include "headerlist.h"
#include "tokenize.h"

int ar[3];
void trimleadingandTrailing(char *s)
{
    int i, j;

    for (i = 0; s[i] == ' ' || s[i] == '\t'; i++)
        ;

    for (j = 0; s[i]; i++)
    {
        s[j++] = s[i];
    }
    s[j] = '\0';
    for (i = 0; s[i] != '\0'; i++)
    {
        if (s[i] != ' ' && s[i] != '\t')
            j = i;
    }
    s[j + 1] = '\0';
}

int tokenize_semi(char *inp, char output[50][2000])
{
    char *token = (char *)malloc(1000);
    inp[strlen(inp) - 1] = ' ';
    token = strtok(inp, ";");
    int i = 0;
    while (token != NULL)
    {
        trimleadingandTrailing(token);
        strcpy(output[i++], token);
        token = strtok(NULL, ";");
    }
    free(token);
    return i;
}

int tokenize_pipe(char *inp, char output[20][1000])
{
    char *token = (char *)malloc(1000);
    // inp[strlen(inp) - 1] = ' ';
    token = strtok(inp, "|");
    int i = 0;
    while (token != NULL)
    {
        trimleadingandTrailing(token);
        strcpy(output[i++], token);
        token = strtok(NULL, "|");
    }
    free(token);
    // printf("%d\n",i);
    return i;
}

int *tokenize_each(char *inp, char *output[50], char *rdpid[10])
{
    char *token = (char *)malloc(1000);
    token = strtok(inp, " ");
    char *temp1 = (char *)malloc(100);
    int x = 0, j = 0, flag = 0, flag1 = 0, flag2 = 0;
    while (token != NULL)
    {
        trimleadingandTrailing(token);
        // printf("%s\n", token);
        if (token[0] != '\0')
        {
            if (token[0] == '>' || token[0] == '<')
            {
                rdpid[j++] = token;
                flag = 1;
            }
            else if (flag == 1)
            {
                flag = 0;
                rdpid[j++] = token;
            }
            else
            {
                output[x++] = token;
            }
        }
        token = strtok(NULL, " ");
    }
    free(token);
    ar[0] = x;
    ar[1] = j;
    // for (int i = 0; i < x; i++)
    //     printf("%d: %s\n", i, output[i]);
    // printf("end argv\n");
    // for (int i = 0; i < j; i++)
    //     printf("%s\n", rdpid[i]);
    return ar;
}

int tokenize_each_bynewline(char *inp, char *output[21])
{
    char *token = (char *)malloc(1000);
    token = strtok(inp, "\n");
    // printf("%s\n", token);
    int i = 0;
    while (token != NULL)
    {
        output[i++] = token;
        token = strtok(NULL, "\n");
        // printf("check tokenize each %d\n", i);
    }
    free(token);
    return i;
}

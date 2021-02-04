#include "headerlist.h"
#include "userdef.h"
#include "tokenize.h"

int parseint(char *ar)
{
    int len = strlen(ar);
    int n = 0;
    for (int i = 0; i < len; i++)
    {
        n = n * 10 + (ar[i] - '0');
    }
    return n;
}

char *prc_detail(char *ar)
{
    switch (ar[0])
    {
    case 'T':
        return "STOPPED";
    case 'R':
        return "RUNNING";
    case 'S':
        return "RUNNING";
    }
}

int userdef(char *argv[50], int cnt)
{
    if (strcmp("setenv", argv[0]) == 0)
    {
        if (cnt < 3)
        {
            printf("Less than required number of arguments. Try Again\n");
        }
        else
        {
            setenv(argv[1], argv[2], 1);
            printf("%s\n", getenv(argv[1]));
        }
    }
    else if (strcmp("unsetenv", argv[0]) == 0)
    {
        if (cnt < 2)
        {
            printf("Less than required number of arguments. Try Again\n");
        }
        else
        {
            unsetenv(argv[1]);
            if (getenv(argv[1]) != NULL)
                printf("%s\n", getenv(argv[1]));
        }
    }
    else if (strcmp("jobs", argv[0]) == 0)
    {

        int k = 1;
        for (int i = 0; i < 100; i++)
        {
            if (map[i].flag == 1)
            {
                char c[200], temp[400];
                char *pinfo_ar[50];
                sprintf(c, "/proc/%d/stat", map[i].key);
                int fd_stat = open(c, O_RDONLY);
                if (fd_stat < 0)
                {
                    printf("No such pid found\n");
                }
                else
                {
                    read(fd_stat, temp, 400);
                    tokenize_each(temp, pinfo_ar, NULL);
                    printf("[%d] %s %s [%d]\n", k++, prc_detail(pinfo_ar[2]), map[i].val, map[i].key);
                }
            }
        }
    }
    else if (strcmp("kjob", argv[0]) == 0)
    {
        if (cnt < 3)
        {
            printf("Less than required number of arguments. Try Again\n");
        }
        else
        {
            int k = 1, i;
            int j_no = parseint(argv[1]), sig_no = parseint(argv[2]);
            for (i = 0; i < 100; i++)
            {
                if (map[i].flag == 1)
                {
                    if (k == j_no)
                        break;
                    k++;
                }
            }
            if (k != j_no)
            {
                printf("No Such Job Exists\n");
            }
            else
                kill(map[i].key, sig_no);
        }
    }
    else if (strcmp("fg", argv[0]) == 0)
    {
        if (cnt < 2)
        {
            printf("Less than required number of arguments. Try Again\n");
        }
        else
        {
            int k = 0, i;
            int j_no = parseint(argv[1]);
            for (i = 0; i < 100; i++)
            {
                if (map[i].flag == 1)
                {
                    k++;
                }
                if (k == j_no)
                    break;
            }
            if (k != j_no)
            {
                printf("No Such Job Exists\n");
            }
            else
            {
                printf("%s with pid %d brought to foreground\n", map[i].val, map[i].key);
                int pid_child = map[i].key, statf;
                int pid_par = getpid(), grp_par = getpgrp();
                map[i].flag = -1;
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                tcsetpgrp(STDIN_FILENO, pid_child);
                kill(pid_child, SIGCONT);
                waitpid(pid_child, &statf, WUNTRACED);
                // printf("status: %d\n", statf);
                if (statf != 0)
                {
                    map[i].flag = 1;
                    printf("%s with pid %d sent to background\n", map[i].val, map[i].key);
                }
                tcsetpgrp(STDIN_FILENO, getpgrp());
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
            }
        }
    }
    else if (strcmp("bg", argv[0]) == 0)
    {
        // printf("entered bg\n");
        if (cnt < 2)
        {
            printf("Less than required number of arguments. Try Again\n");
        }
        else
        {
            int k = 0, i, pid = -1;
            int j_no = parseint(argv[1]);
            // printf("%d\n", j_no);
            for (i = 0; i < 100; i++)
            {
                if (map[i].flag == 1)
                {

                    k++;
                }
                if (k == j_no)
                {
                    pid = i;
                    break;
                }
            }
            if (k != j_no)
            {
                printf("No Such Job Exists\n");
            }
            else
            {
                // printf("%s %d \n", map[pid].val, j_no);
                if (kill(map[pid].key, SIGCONT) < 0)
                {
                    perror("Cannot resume background process!");
                }
            }
        }
    }
    else if (strcmp("overkill", argv[0]) == 0)
    {
        for (int i = 0; i < 100; i++)
        {
            if (map[i].flag == 1)
            {
                kill(map[i].key, SIGKILL);
                map[i].flag = -1;
            }
        }
    }
    else if (strcmp("quit", argv[0]) == 0)
    {
        exit(0);
    }
    return 0;
}
#include "headerlist.h"
#include "listing.h"
#include "tokenize.h"
#include "printuser.h"
#include "userdef.h"

int shellPid = 0;

int checkCMD(char *cmd)
{
    if (strcmp("ls", cmd) == 0)
        return 0;
    else if (strcmp("cd", cmd) == 0)
        return 1;
    else if (strcmp("pwd", cmd) == 0)
        return 2;
    else if (strcmp("echo", cmd) == 0)
        return 3;
    else if (strcmp("pinfo", cmd) == 0)
        return 4;
    else if (strcmp("history", cmd) == 0)
        return 5;
    else if (strcmp("setenv", cmd) == 0 || strcmp("unsetenv", cmd) == 0 || strcmp("jobs", cmd) == 0 || strcmp("kjob", cmd) == 0 || strcmp("fg", cmd) == 0 || strcmp("bg", cmd) == 0 || strcmp("overkill", cmd) == 0 || strcmp("quit", cmd) == 0)
        return 6;
    else
        return -1;
}

void childwatch(int sig)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    char name[80];
    for (int i = 0; i < 100; i++)
    {
        if (map[i].key == pid)
        {
            strcpy(name, map[i].val);
            map[i].flag = 0;
            break;
        }
    }
    if (pid > 0)
        printf("%s with pid %d exited normally\n", name, pid);
}

void ctrl_c_sig(int sig)
{
    if (getpid() != shellPid)
        kill(getpid(), SIGTERM);
    return;
}

void pausewatch(int sig)
{
    if (shellPid == getpid())
        return;
    raise(SIGTSTP);
}

int main()
{
    signal(SIGTSTP, pausewatch);
    char *user = getenv("USER");
    char hostname[1000];
    char home[1000];
    getcwd(home, sizeof(home));
    gethostname(hostname, sizeof(hostname));
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    int n = strlen(home);
    char *histlocation = (char *)malloc(4096);
    strcpy(histlocation, "history_ASS_2019111003");
    int fd_hist = open(histlocation, O_RDWR | O_CREAT, 0666);
    struct stat buf;
    stat(histlocation, &buf);
    char *histScan = (char *)malloc(20000);
    read(fd_hist, histScan, buf.st_size);
    char *hist[21];
    int ar[21];
    int histcnt = 0;
    histcnt = tokenize_each_bynewline(histScan, hist);
    int histpnt = histcnt;
    // printf("%d\n", histpnt);
    pid_t p_pid = getpid();
    shellPid = p_pid;
    int mapit = 0;
    system("clear");
    int bkp_stdout = dup(STDOUT_FILENO), bkp_stdin = dup(STDIN_FILENO);
    char *prevwd = (char *)malloc(4096);
    while (1)
    {
        signal(SIGINT, ctrl_c_sig);
        getcwd(cwd, sizeof(cwd));
        char *cmdLine = (char *)malloc(4096), *cmd1 = NULL, c1;
        memset(cmdLine, '\0', sizeof(cmdLine));
        char output[50][2000];
        unsigned long int siz = 20, i = 0;
        dup2(bkp_stdin, STDIN_FILENO);
        dup2(bkp_stdout, STDOUT_FILENO);
        printUserHost(cwd, home, user, hostname, n);
        if ((c1 = getchar()) < 0)
            exit(0);
        else
            cmdLine[0] = c1;
        if (c1 == '\n')
            continue;
        getline(&cmd1, &siz, stdin);
        strcat(cmdLine, cmd1);
        trimleadingandTrailing(cmdLine);
        if (cmdLine[0] != '\n')
        {
            histpnt %= 20;
            if (histpnt < 1)
            {
                hist[histpnt++] = cmdLine;
                if (histcnt < 20)
                    histcnt++;
            }
            else if (strncmp(hist[histpnt - 1], cmdLine, strlen(hist[histpnt - 1])) != 0)
            {
                hist[histpnt++] = cmdLine;
                if (histcnt < 20)
                    histcnt++;
                // printf("yse\n");
            }
            close(fd_hist);
            char *histloc = (char *)malloc(1024);
            sprintf(histloc, "%s/history_ASS_2019111003", home);
            fd_hist = open(histloc, O_RDWR | O_TRUNC, 0666);
            if (fd_hist < 0)
                perror("History file error");
            int x = 0;
            for (int i = histpnt; i < histcnt; i++)
            {
                write(fd_hist, hist[i], strlen(hist[i]));
                write(fd_hist, "\n", 1);
                ar[x++] = i;
            }
            for (int i = 0; i < histpnt; i++)
            {
                write(fd_hist, hist[i], strlen(hist[i]));
                write(fd_hist, "\n", 1);
                ar[x++] = i;
            }
            int cnt = tokenize_semi(cmdLine, output);
            char tempArr[100];
            char tempArr2[10000];
            // printf("%s\n", output[0]);
            //array for semicolon tokenize
            for (int i = 0; i < cnt; i++)
            {
                dup2(bkp_stdin, STDIN_FILENO);
                dup2(bkp_stdout, STDOUT_FILENO);
                if (strlen(output[i]) == 0)
                    continue;
                char piparg[20][1000];
                int cntpip = tokenize_pipe(output[i], piparg);
                int pipdes[cntpip][2];
                //array for pipes tokenize
                for (int m = 0; m < cntpip; m++)
                {
                    if (pipe(pipdes[m]) < 0)
                    {
                        perror("Could not create pipe");
                        exit(1);
                    }
                    if (cntpip > 1 && m != cntpip - 1)
                        dup2(pipdes[m][1], STDOUT_FILENO);
                    if (cntpip > 1 && m != 0)
                        dup2(pipdes[m - 1][0], STDIN_FILENO);
                    if (m == cntpip - 1)
                        dup2(bkp_stdout, STDOUT_FILENO);
                    close(pipdes[m][1]);
                    if (m > 0)
                        close(pipdes[m - 1][0]);
                    char *argv[50], *rd_pip[10];
                    char *pinfo_ar[50];
                    int *cntar = tokenize_each(piparg[m], argv, rd_pip);
                    int cnt1 = cntar[0];
                    for (int x = 0; x < cntar[1]; x++)
                    {
                        if (rd_pip[x][0] == '<' || rd_pip[x][0] == '>')
                        {
                            if (rd_pip[x][1] == '>' && rd_pip[x][1] == rd_pip[x][0])
                            {
                                int fd1 = open(rd_pip[x + 1], O_RDWR | O_APPEND | O_CREAT, 0666);
                                if (fd1 < 0)
                                {
                                    perror("redirection error");
                                }
                                else
                                {
                                    dup2(fd1, STDOUT_FILENO);
                                }
                            }
                            else
                            {
                                if (rd_pip[x][0] == '<')
                                {
                                    int fd2 = open(rd_pip[x + 1], O_RDONLY);
                                    if (fd2 < 0)
                                    {
                                        perror("redirection error 2");
                                    }
                                    else
                                    {
                                        dup2(fd2, STDIN_FILENO);
                                    }
                                }
                                else if (rd_pip[x][0] == '>')
                                {
                                    int fd3 = open(rd_pip[x + 1], O_RDWR | O_CREAT, 0666);
                                    if (fd3 < 0)
                                    {
                                        perror("redirection error3");
                                    }
                                    else
                                    {
                                        dup2(fd3, STDOUT_FILENO);
                                    }
                                }
                            }
                        }
                    }
                    int status, pid_temp;
                    int forkRet = 0;
                    pid_t c_pid = getpid();
                    int temp;
                    int waitflag = 0;
                    int cd_ret;
                    int flag_l = 0, flag_a = 0, flag_print = 0;
                    // printf("%s\n", argv[0]);
                    switch (checkCMD(argv[0]))
                    {
                    case 0:
                        for (int x = 1; x < cnt1; x++)
                        {
                            if (argv[x][0] == '-')
                            {
                                int len_str = strlen(argv[x]);
                                for (int y = 0; y < len_str; y++)
                                {
                                    if (argv[x][y] == 'l')
                                        flag_l = 1;
                                    if (argv[x][y] == 'a')
                                        flag_a = 1;
                                }
                            }
                        }
                        for (int x = 1; x < cnt1; x++)
                        {
                            if (argv[x][0] != '-')
                            {
                                flag_print = 1;
                                if (flag_l > 0)
                                {
                                    ls_l(flag_a, argv[x], home);
                                }
                                else
                                {
                                    ls(flag_a, argv[x], home);
                                }
                            }
                        }
                        if (flag_print == 0)
                        {
                            if (flag_l > 0)
                            {
                                ls_l(flag_a, ".", home);
                            }
                            else
                            {
                                ls(flag_a, ".", home);
                            }
                        }
                        break;
                    case 1:
                        /* cd */
                        if (strlen(cmdLine) == 2)
                        {
                            cd_ret = chdir(home);
                            if (strcmp(cwd, home) != 0)
                                strcpy(prevwd, cwd);
                            break;
                        }
                        else if (argv[1][0] == '~')
                        {
                            char dest[150] = "";
                            strcat(dest, home);
                            if (strlen(argv[1]) > 1)
                                strcat(dest, argv[1] + 1);
                            if (strcmp(dest, cwd) != 0)
                                strcpy(prevwd, cwd);
                            cd_ret = chdir(dest);
                        }
                        else if (argv[1][0] == '-')
                        {
                            if (prevwd[0] != '\0')
                            {
                                printf("%s\n", prevwd);
                                chdir(prevwd);
                            }
                            else
                                printf("Error: OLDPWD not set");
                        }
                        else
                        {
                            cd_ret = chdir(argv[1]);
                            char tempr[400];
                            getcwd(tempr, 400);
                            if (strcmp(tempr, cwd) != 0)
                                strcpy(prevwd, cwd);
                        }
                        if (cd_ret < 0)
                        {
                            perror("cd error ");
                        }
                        break;
                    case 2:
                        /* pwd */
                        getcwd(cwd, sizeof(cwd));
                        printf("%s\n", cwd);
                        break;
                    case 3:
                        /* echo */
                        temp = 1;
                        while (temp < cnt1)
                        {
                            printf("%s ", argv[temp++]);
                        }
                        printf("\n");
                        break;
                    case 4:
                        //pinfo
                        p_pid = getpid();
                        if (cnt1 != 1)
                        {
                            temp = strlen(argv[1]);
                            pid_temp = 0;
                            for (int i = 0; i < temp; i++)
                            {
                                pid_temp = pid_temp * 10 + argv[1][i] - 48;
                            }
                            p_pid = pid_temp;
                        }
                        sprintf(tempArr, "/proc/%d/stat", p_pid);
                        // printf("%s\n", tempArr);
                        int fd_stat = open(tempArr, O_RDONLY);
                        if (fd_stat < 0)
                        {
                            printf("No such pid found\n");
                        }
                        else
                        {
                            read(fd_stat, tempArr2, 1500);
                            tokenize_each(tempArr2, pinfo_ar, NULL);
                            printf("pid -- %d\n", p_pid);
                            printf("Process Status -- %s\n", pinfo_ar[2]);
                            printf("memory -- %s\n", pinfo_ar[22]);
                            printf("Executable Path -- ");
                            sprintf(tempArr, "/proc/%d/exe", p_pid);
                            int open_x = open(tempArr, O_RDONLY);
                            char path[1000];
                            readlink(tempArr, path, sizeof(tempArr2));
                            if (strncmp(cwd, path, n) == 0)
                            {
                                printf("~%s\n", path + n);
                            }
                            else
                                printf("%s\n", path);
                            memset(path, '\0', 1000);
                        }
                        break;
                    case 5:
                        if (cnt1 == 1)
                        {
                            int i = (histcnt - 10 >= 0) ? histcnt - 10 : 0;
                            for (i; i < histcnt; i++)
                                printf("%s\n", hist[ar[i]]);
                        }
                        else
                        {
                            int len = strlen(argv[1]), num = 0;
                            for (int i = 0; i < len; i++)
                            {
                                num = num * 10 + argv[1][0] - 48;
                            }
                            int i = (histcnt - num >= 0) ? histcnt - num : 0;
                            for (i; i < histcnt; i++)
                                printf("%s\n", hist[ar[i]]);
                        }
                        break;
                    case 6:
                        userdef(argv, cnt1);
                        break;
                    default:
                        signal(SIGCHLD, childwatch);
                        forkRet = fork();
                        if (forkRet == 0)
                        {
                            // printf("enter child\n");
                            if (argv[cnt1 - 1][0] == '&')
                            {
                                argv[cnt1 - 1] = NULL;
                                waitflag = -1;
                                setpgid(0, 0);
                                // printf("%d\t%d\n", getpid(), getpgid(getpid()));
                            }
                            if (execvp(argv[0], argv) < 0)
                            {
                                perror("Invalid Command");
                                exit(0);
                            }
                        }
                        else
                        {
                            if (argv[cnt1 - 1][0] != '&')
                            {
                                setpgid(forkRet, 0);
                                signal(SIGTTIN, SIG_IGN);
                                signal(SIGTTOU, SIG_IGN);
                                tcsetpgrp(STDIN_FILENO, forkRet);
                                waitpid(forkRet, &status, WUNTRACED);
                                tcsetpgrp(STDIN_FILENO, getpgrp());
                                signal(SIGTTIN, SIG_DFL);
                                signal(SIGTTOU, SIG_DFL);
                                if (WIFSTOPPED(status))
                                {
                                    printf("The process %s with PID %d is sent to background\n", argv[0], forkRet);
                                    map[mapit].key = forkRet;
                                    strcpy(map[mapit].val, argv[0]);
                                    map[mapit].flag = 1;
                                    mapit++;
                                }
                            }
                            else
                            {
                                map[mapit].key = forkRet;
                                strcpy(map[mapit].val, argv[0]);
                                map[mapit].flag = 1;
                                mapit++;
                                printf("The process %s with PID %d is lauched in background\n", argv[0], forkRet);
                            }
                        }
                        break;
                    }
                    memset(argv, '\0', sizeof(argv));
                }
                memset(piparg, '\0', sizeof(piparg));
            }
        }
    }
    return 0;
}
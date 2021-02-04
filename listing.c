#include "headerlist.h"
#include "listing.h"

char *month(int mn)
{
    switch (mn)
    {
    case 0:
        return "Jan";
        break;
    case 1:
        return "Feb";
        break;
    case 2:
        return "Mar";
        break;
    case 3:
        return "Apr";
        break;
    case 4:
        return "May";
        break;
    case 5:
        return "Jun";
        break;
    case 6:
        return "Jul";
        break;
    case 7:
        return "Aug";
        break;
    case 8:
        return "Sep";
        break;
    case 9:
        return "Oct";
        break;
    case 10:
        return "Nov";
        break;
    case 11:
        return "Dec";
        break;
    }
}

void ls(int arg, char *directry, char *home)
{
    DIR *dir;
    struct dirent *dir_entry;
    char *directory = (char *)malloc(500);
    if (directry[0] == '~')
    {
        strcpy(directory, home);
        strcat(directory, directry + 1);
    }
    else
    {
        strcpy(directory, directry);
    }
    dir = opendir(directory);

    if (dir)
    {
        while ((dir_entry = readdir(dir)) != NULL)
        {
            if (arg == 0)
            {
                if (dir_entry->d_name[0] != '.')
                    printf("%s\n", dir_entry->d_name);
            }
            else
                printf("%s\n", dir_entry->d_name);
        }
        closedir(dir);
    }
    else
    {
        perror("");
    }
    free(directory);
}

void ls_l(int arg, char *directry, char *home)
{
    DIR *dir;
    struct dirent *dir_entry;
    char *directory = (char *)malloc(500);
    struct stat buf;
    if (directry[0] == '~')
    {
        strcpy(directory, home);
        strcat(directory, directry + 1);
    }
    else
    {
        strcpy(directory, directry);
    }
    dir = opendir(directory);
    if (dir)
    {
        struct stat statbuf;
        int cnt = 0;
        int len_max[5] = {0};
        while ((dir_entry = readdir(dir)) != NULL)
        {
            if (arg == 0)
            {
                if (dir_entry->d_name[0] != '.')
                {
                    stat(dir_entry->d_name, &buf);
                    cnt = cnt + buf.st_blocks;
                }
            }
            else
            {
                stat(dir_entry->d_name, &buf);
                cnt = cnt + buf.st_blocks;
            }
        }
        //cnt/2 because all linux blocks are currently 1024 bytes and st_blocks returns blocks in 512 byte size
        printf("total %d\n", cnt / 2);
        dir = opendir(directory);
        while ((dir_entry = readdir(dir)) != NULL)
        {
            stat(dir_entry->d_name, &statbuf);
            int fd = open(dir_entry->d_name, O_RDONLY);
            if (arg == 0)
            {
                if (dir_entry->d_name[0] != '.')
                {
                    if (fd < 0)
                    {
                        perror("Permission Denied: ");
                    }
                    else
                    {
                        if (dir_entry->d_type == 8)
                            printf("-");
                        else
                            printf("d");
                        mode_t perm = statbuf.st_mode;
                        char permission[10];
                        struct tm *changetime;
                        permission[0] = (S_IRUSR & perm) ? 'r' : '-';
                        permission[1] = (S_IWUSR & perm) ? 'w' : '-';
                        permission[2] = (S_IXUSR & perm) ? 'x' : '-';
                        permission[3] = (S_IRGRP & perm) ? 'r' : '-';
                        permission[4] = (S_IWGRP & perm) ? 'w' : '-';
                        permission[5] = (S_IXGRP & perm) ? 'x' : '-';
                        permission[6] = (S_IROTH & perm) ? 'r' : '-';
                        permission[7] = (S_IWOTH & perm) ? 'w' : '-';
                        permission[8] = (S_IXOTH & perm) ? 'x' : '-';
                        permission[9] = '\0';
                        printf("%s ", permission);
                        printf("%2ld ", statbuf.st_nlink);
                        printf("%10s ", getpwuid(statbuf.st_uid)->pw_name);
                        printf("%10s ", getgrgid(statbuf.st_gid)->gr_name);
                        printf("%12ld ", statbuf.st_size);
                        changetime = localtime(&statbuf.st_ctim.tv_sec);
                        printf("%s ", month(changetime->tm_mon));
                        printf("%2d ", changetime->tm_mday);
                        printf("%2d:", changetime->tm_hour);
                        if ((changetime->tm_min / 10) > 0)
                            printf("%2d ", changetime->tm_min);
                        else
                            printf("0%1d ", changetime->tm_min);
                        printf("%s\n", dir_entry->d_name);
                    }
                }
            }
            else
            {
                if (fd < 0)
                {
                    perror("");
                }
                else
                {
                    if (dir_entry->d_type == 8)
                        printf("-");
                    else
                        printf("d");
                    mode_t perm = statbuf.st_mode;
                    char permission[10];
                    struct tm *changetime;
                    permission[0] = (S_IRUSR & perm) ? 'r' : '-';
                    permission[1] = (S_IWUSR & perm) ? 'w' : '-';
                    permission[2] = (S_IXUSR & perm) ? 'x' : '-';
                    permission[3] = (S_IRGRP & perm) ? 'r' : '-';
                    permission[4] = (S_IWGRP & perm) ? 'w' : '-';
                    permission[5] = (S_IXGRP & perm) ? 'x' : '-';
                    permission[6] = (S_IROTH & perm) ? 'r' : '-';
                    permission[7] = (S_IWOTH & perm) ? 'w' : '-';
                    permission[8] = (S_IXOTH & perm) ? 'x' : '-';
                    permission[9] = '\0';
                    printf("%s ", permission);
                    printf("%2ld ", statbuf.st_nlink);
                    printf("%10.10s ", getpwuid(statbuf.st_uid)->pw_name);
                    printf("%10.10s ", getgrgid(statbuf.st_gid)->gr_name);
                    printf("%12ld ", statbuf.st_size);
                    changetime = localtime(&statbuf.st_ctim.tv_sec);
                    printf("%s ", month(changetime->tm_mon));
                    printf("%2d ", changetime->tm_mday);
                    printf("%2d:", changetime->tm_hour);
                    if ((changetime->tm_min / 10) > 0)
                        printf("%2d ", changetime->tm_min);
                    else
                        printf("0%1d ", changetime->tm_min);
                    printf("%s\n", dir_entry->d_name);
                }
            }
        }
    }
    else
    {
        perror("");
    }
    closedir(dir);
    free(directory);
}

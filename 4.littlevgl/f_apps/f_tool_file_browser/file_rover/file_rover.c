#include "file_rover.h"
#include "stdio.h"
#include "string.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>

/* Listing view flags. */
#define SHOW_FILES 0x01u
#define SHOW_DIRS 0x02u
#define SHOW_HIDDEN 0x04u
/* Default listing view flags.
   May include SHOW_FILES, SHOW_DIRS and SHOW_HIDDEN. */
#define RV_FLAGS SHOW_FILES | SHOW_DIRS
#define BUFLEN FILE_PATH_MAX

/* 用于对列表条目进行排序的比较。 */
static int rowcmp(const void *a, const void *b)
{
    int isdir1, isdir2, cmpdir;
    const f_row_t *r1 = a;
    const f_row_t *r2 = b;
    isdir1 = S_ISDIR(r1->mode);
    isdir2 = S_ISDIR(r2->mode);
    cmpdir = isdir2 - isdir1;
    return cmpdir ? cmpdir : strcoll(r1->name, r2->name);
}

/* 获取当前工作目录中的所有条目。*/
static int ls(f_row_t **rowsp, uint8_t flags)
{
    DIR *dp;
    struct dirent *ep;
    struct stat statbuf;
    f_row_t *rows;
    int i, n;

    if (!(dp = opendir(".")))
        return -1;
    n = -2; /* We don't want the entries "." and "..". */
    while (readdir(dp))
        n++;
    if (n == 0)
    {
        closedir(dp);
        return 0;
    }
    rewinddir(dp);
    rows = malloc(n * sizeof *rows);
    i = 0;
    while ((ep = readdir(dp)))
    {
        if (!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, ".."))
            continue;
        if (!(flags & SHOW_HIDDEN) && ep->d_name[0] == '.')
            continue;
        lstat(ep->d_name, &statbuf);
        rows[i].islink = S_ISLNK(statbuf.st_mode);
        stat(ep->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (flags & SHOW_DIRS)
            {
                rows[i].name = malloc(strlen(ep->d_name) + 2);
                strcpy(rows[i].name, ep->d_name);
                if (!rows[i].islink)
                    strcat(rows[i].name, "/");
                rows[i].mode = statbuf.st_mode;
                i++;
            }
        }
        else if (flags & SHOW_FILES)
        {
            rows[i].name = malloc(strlen(ep->d_name) + 1);
            strcpy(rows[i].name, ep->d_name);
            rows[i].size = statbuf.st_size;
            rows[i].mode = statbuf.st_mode;
            i++;
        }
    }
    n = i; /*忽略由过滤器引起的数组中未使用的空间. */
    qsort(rows, n, sizeof(*rows), rowcmp);
    closedir(dp);
    *rowsp = rows;
    return n;
}

static void free_rows(f_row_t **rowsp, int nfiles)
{
    int i;

    for (i = 0; i < nfiles; i++)
        free((*rowsp)[i].name);
    free(*rowsp);
    *rowsp = NULL;
}

void file_rover_test(void)
{
    int i, j;
    f_row_t *rows;
    int nfiles;
    char cwd[FILE_PATH_MAX];
    getcwd(cwd, FILE_PATH_MAX);
    printf("%d %s \r\n", __LINE__, cwd);

    if (cwd[strlen(cwd) - 1] != '/')
        strcat(cwd, "/");

    printf("Loading \"%s\"...\r\n", cwd);

    if (chdir(cwd) == -1) //切换到CWD路径
    {
        getcwd(cwd, FILE_PATH_MAX - 1);
        if (cwd[strlen(cwd) - 1] != '/')
            strcat(cwd, "/");
        return;
    }

    nfiles = ls(&rows, RV_FLAGS);

    for (i = 0; i < nfiles; i++)
    {
        rows[i].marked = 0;
        printf("name = %s size = [%ld] mode = %d islink = %d marked = %d\r\n", rows[i].name, rows[i].size, rows[i].mode, rows[i].islink, rows[i].marked);
        // if (S_ISDIR(EMODE(j)))
        // {
        //     mbstowcs(WBUF, ENAME(j), FILE_PATH_MAX);
        //     if (ISLINK(j))
        //         wcscat(WBUF, L"/");
        // }

        // char *suffix, *suffixes = "BKMGTPEZY";
        // off_t human_size = rows[i].size * 10;
        // static wchar_t WBUF[BUFLEN];

        // int length = mbstowcs(WBUF, rows[i].name, FILE_PATH_MAX);
        // int namecols = wcswidth(WBUF, length);
        // for (suffix = suffixes; human_size >= 10240; suffix++)
        //     human_size = (human_size + 512) / 1024;
        // if (*suffix == 'B')
        //     swprintf(WBUF + length, FILE_PATH_MAX - length, L"%*d %c",
        //                 (int)(100 - namecols - 6),
        //                 (int)human_size / 10, *suffix);
        // else
        //     swprintf(WBUF + length, FILE_PATH_MAX - length, L"%*d.%d %c",
        //                 (int)(100 - namecols - 8),
        //                 (int)human_size / 10, (int)human_size % 10, *suffix);
        // printf("%ls\r\n",WBUF);
    }
    if (nfiles)
        free_rows(&rows, nfiles);

    // else if (!strcmp(key, RVK_CD_DOWN))
    // {
    //     if (!rover.nfiles || !S_ISDIR(EMODE(ESEL)))
    //         continue;
    //     if (chdir(ENAME(ESEL)) == -1)
    //     {
    //         message(RED, "Cannot access \"%s\".", ENAME(ESEL));
    //         continue;
    //     }
    //     strcat(CWD, ENAME(ESEL));
    //     cd(1);
    // }
    // else if (!strcmp(key, RVK_CD_UP))
    // {
    //     char *dirname, first;
    //     if (!strcmp(CWD, "/"))
    //         continue;
    //     CWD[strlen(CWD) - 1] = '\0';
    //     dirname = strrchr(CWD, '/') + 1;
    //     first = dirname[0];
    //     dirname[0] = '\0';
    //     cd(1);
    //     dirname[0] = first;
    //     dirname[strlen(dirname)] = '/';
    //     try_to_sel(dirname);
    //     dirname[0] = '\0';
    //     if (rover.nfiles > HEIGHT)
    //         SCROLL = ESEL - HEIGHT / 2;
    //     update_view();
    // }
}

int file_rover_get_row_cwd(char *path, f_row_list_t *rows)
{
    if (!path || !rows)
    {
        return -1;
    }
    if (rows->row)
    {
        free_rows(&rows->row, rows->nfiles);
        rows->row = NULL;
        rows->nfiles = 0;
    }

    getcwd(path, FILE_PATH_MAX);

    if (path[strlen(path) - 1] != '/')
    {
        strcat(path, "/");
    }

    if (chdir(path) == -1) //切换到CWD路径
    {
        getcwd(path, FILE_PATH_MAX - 1);
        if (path[strlen(path) - 1] != '/')
        {
            strcat(path, "/");
        }
        return -1;
    }

    rows->nfiles = ls(&rows->row, RV_FLAGS);
    //  for (int i = 0; i < rows->nfiles; i++){
    //     printf("name = %s size = [%ld] mode = %d islink = %d marked = %d\r\n", rows->row[i].name,rows->row[i].size,rows->row[i].mode,rows->row[i].islink,rows->row[i].marked);
    //  }
    return 0;
}

int file_rover_get_row_down(const char *path, f_row_list_t *rows)
{
    if (!path || !rows)
    {
        return -1;
    }

    if (rows->row)
    {
        free_rows(&rows->row, rows->nfiles);
        rows->row = NULL;
        rows->nfiles = 0;
    }

    if (chdir(path) == -1)
    {
        return -1;
    }

    rows->nfiles = ls(&rows->row, RV_FLAGS);

    return 0;
    ;
}

int file_rover_get_row_up(const char *path, char *dest_path, f_row_list_t *rows)
{
    if (!path || !rows || !dest_path)
    {
        return -1;
    }

    if (0 == strcmp(path, "/"))
    {
        return -1;
    }

    if (rows->row)
    {
        free_rows(&rows->row, rows->nfiles);
        rows->row = NULL;
        rows->nfiles = 0;
    }
    strcmp(dest_path, path);
    char *dirname;
    dest_path[strlen(dest_path) - 1] = '\0';
    dirname = strrchr(dest_path, '/') + 1;
    dirname[0] = '\0';

    if (chdir(dest_path) == -1)
    {
        return -1;
    }

    rows->nfiles = ls(&rows->row, RV_FLAGS);
    return 0;
}

int file_rover_is_dir(mode_t mide)
{
    if (S_ISDIR(mide))
    {
        return 1;
    }
    return 0;
}

int file_rover_get_size_buf(char *buff, off_t size)
{
    if (!buff)
    {
        return -1;
    }

    char *suffix, *suffixes = "BKMGTPEZY";
    off_t human_size = size * 10;

    for (suffix = suffixes; human_size >= 10240; suffix++)
    {
        human_size = (human_size + 512) / 1024;
    }
    if (*suffix == 'B')
    {
        sprintf(buff, "%d %c", (int)human_size / 10, *suffix);
    }
    else
    {
        sprintf(buff, "%d.%d %c", (int)human_size / 10, (int)human_size % 10, *suffix);
    }
    return 0;
}

#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#define MAX_PATH 260
#define KEY 10
#define ENCRYPT 0
#define DECRYPT 1

static const char dirpath[] = "/home/Nisyua/Documents";
static const char charlist[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

int chk(char input[]){
    for(int i=0;i+7<strlen(input);i++){
      if(input[i]=='Y'&&input[i+1]=='O'&&
            input[i+2]=='U'&&input[i+3]=='T'&&
              input[i+4]=='U'&&input[i+5]=='B'&&
                input[i+6]=='E'&&input[i+7]=='R')
                 return 1;
      }
    return 0;
}

void caesar(char inp[], int len, int flag)
{
    int i, j, index;
    if (flag == DECRYPT)
    {
        for (i = 0; i < len; i++)
        {
            if (inp[i] == '/')
                continue;
            else
            {
                for (j = 0; j < strlen(charlist); j++)
                {
                    if (charlist[j] == inp[i])
                        index = j;
                }
                index -= KEY;
                while (index < 0)
                    index += strlen(charlist);
                inp[i] = charlist[index];
            }
        }
    }
    else if (flag == ENCRYPT)
    {
        for (i = 0; i < len; i++)
        {
            if (inp[i] == '/')
                continue;
            else
            {
                for (j = 0; j < strlen(charlist); j++)
                {
                    if (charlist[j] == inp[i])
                        index = j;
                }
                index += KEY;
                index %= strlen(charlist);
                inp[i] = charlist[index];
            }
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char name[MAX_PATH], fname[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    strcpy(name, path);
    caesar(name, strlen(name), ENCRYPT);
    strcat(fname, dirpath);
    strcat(fname, name);
    res = lstat(fname, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path,
                       void *buf,
                       fuse_fill_dir_t filler,
                       off_t offset,
                       struct fuse_file_info *fi)
{
    char name[MAX_PATH], fname[MAX_PATH], dec[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    if(strcmp(path,"/") == 0)
    {
        path = dirpath;
        sprintf(fname, "%s", path);
    }
    else
    {
        strcpy(name, path);
        caesar(name, strlen(name), ENCRYPT);
        memset(fname, '\0', MAX_PATH);
        strcat(fname, dirpath);
        strcat(fname, name);
    }
    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(fname);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        if(strcmp(de->d_name,".")==0||strcmp(de->d_name,"..")==0)
            continue;
        if (strcmp(de->d_name, "@ZA>AXio") == 0)
        {
            system("touch /home/Nisyua/ketemu");
        }
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        memset(dec, '\0', MAX_PATH);
        strcpy(dec, de->d_name);
        caesar(dec, strlen(dec), DECRYPT);
        res = (filler(buf, dec, &st, 0));
        if(res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path,
                    char *buf,
                    size_t size,
                    off_t offset,
                    struct fuse_file_info *fi)
{
    char name[MAX_PATH], fname[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    if(strcmp(path,"/") == 0)
    {
        path = dirpath;
        sprintf(fname, "%s", path);
    }
    else
    {
        strcpy(name, path);
        caesar(name, strlen(name), ENCRYPT);
        memset(fname, '\0', MAX_PATH);
        strcat(fname, dirpath);
        strcat(fname, name);
    }
    int res = 0;
    int fd = 0 ;

    (void) fi;
    fd = open(fname, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static struct fuse_operations xmp_oper =
{
    .getattr	= xmp_getattr,
    .readdir	= xmp_readdir,
    .read		= xmp_read,
};

int main(int argc, char *argv[])
{
    umask(0);
    argc = 2;
    argv[0] = "./SISOPM4";
    argv[1] = "/home/Nisyua/shift4_mount";
    argv[2] = NULL;
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

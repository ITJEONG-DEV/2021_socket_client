#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int open_read(char* filename);
int open_write(char* filename);
void close_read(int fd);
void close_write(int fd);
void read_file(int fd, int fd_w);
int write_file(int fd, char* buf);

int real_max = 230;

int main(void)
{
    char* filename = "example.txt";
    char* filename2 = "example2.txt";

    int fd_r = open_read(filename);
    int fd_w = open_write(filename2);

    read_file(fd_r, fd_w);

    close_read(fd_r);
    close_write(fd_w);
}

int open_read(char* filename)
{
    int fd;
    if((fd = open(filename, O_RDONLY)) == -1)
    {
        printf("%s open 중 오류 발생\n", filename);
        return -1;
    }

    return fd;
}

int open_write(char* filename)
{
    int fd;
    if((fd = open(filename, O_WRONLY |  O_CREAT | O_TRUNC)) == -1)
    {
        printf("%s open 중 오류 발생\n", filename);
        return -1;
    }

    return fd;
}

void close_read(int fd)
{
    close(fd);
}

void close_write(int fd)
{
    close(fd);
}

void read_file(int fd, int fd_w)
{
    char buf[real_max];

    int len;
    while(1)
    {
        memset(buf, 0x00, real_max);
        len = read(fd, buf, real_max);
        int result = write_file(fd_w, buf);
        printf("%s\n", buf);
        if(len == 0)
            break;
    }
}

int write_file(int fd, char* buf)
{
    if(write(fd, buf, strlen(buf)) == -1)
    {
        perror("write failed");
        return -1;
    }
    return 0;
}
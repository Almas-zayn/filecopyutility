#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#define BUFFER_SIZE 1024

static ssize_t read_all(int fd, void *buffer, size_t count)
{
    uint8_t *pointer = buffer;
    size_t left = count;

    while (left > 0)
    {
        ssize_t bytes_read = read(fd, pointer, left);
        if (bytes_read < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (bytes_read == 0)
            break;

        left -= (size_t)bytes_read;
        pointer += bytes_read;
    }
    return (ssize_t)(count - left);
}

static ssize_t write_all(int fd, const void *buffer, size_t count)
{
    const uint8_t *pointer = buffer;
    size_t left = count;

    while (left > 0)
    {
        ssize_t bytes_written = write(fd, pointer, left);

        if (bytes_written < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        left -= (size_t)bytes_written;
        pointer += bytes_written;
    }

    return (ssize_t)count;
}

bool isFileEmpty(int fd)
{
    if (fd == -1)
    {
        perror("The file is not open with this file Descripter Invalid file descripter ");
        return true;
    }
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("fstat");
        return 1;
    }
    if (st.st_size == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        if (argc == 2)
        {
            printf("Missing Destination file");
        }
        else
        {
            printf("Missing Source File and Destnation file names or invalid number of arguments\n");
        }
        return 1;
    }

    int srcfd = open(argv[1], O_RDONLY);
    if (srcfd == -1)
    {
        perror("open source");
        return 1;
    }
    if (isFileEmpty(srcfd))
    {
        printf("The Source File : %s is Empty ", argv[1]);
        return 0;
    }

    int desfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (desfd == -1)
    {
        perror("open destination");
        close(srcfd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read_all(srcfd, buffer, BUFFER_SIZE)) > 0)
    {
        if (write_all(desfd, buffer, (size_t)bytes_read) != bytes_read)
        {
            perror("write");
            close(srcfd);
            close(desfd);
            return 1;
        }
    }

    if (bytes_read < 0)
        perror("read");

    close(srcfd);
    close(desfd);

    printf("File copy from Source File : %s to Destination File : %s completed successfully\n", argv[1], argv[2]);
    return 0;
}

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


#include <string.h>

int main(
    void)

{

    int
        *handle;
    char
        string[40];

    int
        length,
        res;

    /* Create a file named "TEST.$$$" in the current directory and write a string to it. If "TEST.$$$" already exists, it will be overwritten. */



    if ((handle = open(
             "TEST.$$$",  (O_WRONLY|O_TRUNC))
             ) == -1)

    {

        printf(
            "Error opening file.\n");
            int fd = creat( "TEST.$$$", 0644);
            close(fd);
    }

        if ((handle = open(
             "TEST.$$$",  (O_WRONLY|O_TRUNC))
             ) == -1)

    {
            exit(1);
    }




    strcpy(string,
           "Hello, world!\n");

    length =
        strlen(string);

    if ((res = write(handle, string, length)) != length)

    {

        printf(
            "Error writing to the file.\n");

        exit(1);
    }

    printf(
        "Wrote %d bytes to the file.\n", res);

    close(handle);

    return 0;
}
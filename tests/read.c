#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 256

//opens the driver and reads from it

int main() {
    int fd = open("/dev/OsAss2DriverDevice", O_RDWR);

    if (fd == -1) {
        perror("open");
        return 1;
    }
    char buffer[BUFFER_SIZE];

    int result = read(fd, buffer, BUFFER_SIZE);

    if (result == -1) {
        perror("read");
        return 1;
    }

    printf("%s\n", buffer);
}
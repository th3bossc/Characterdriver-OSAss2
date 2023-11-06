#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#define BUFFER_SIZE 256

//opens the driver and writes the username of the current process to it

int main() {
    char buffer[BUFFER_SIZE];

    uid_t uid = getuid();

    struct passwd* pwd = getpwuid(uid);

    if (pwd == NULL) {
        perror("getpwuid");
        return 1;
    }

    int fd = open("/dev/OsAss2DriverDevice", O_RDWR);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    strcpy(buffer, pwd->pw_name);

    int result = write(fd, buffer, BUFFER_SIZE);

    if (result == -1) {
        perror("write");
        return 1;
    }
}
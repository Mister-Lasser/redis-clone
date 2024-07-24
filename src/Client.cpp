#include <complex>
#include <cstdlib>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //local

    int rv = connect(
        client_fd,
        (const struct sockaddr *)&addr,
        sizeof(addr)
    );
    if (rv) {
        die("connect");
    }

    char msg[] = "hello";
    write(client_fd, msg, strlen(msg));

    char rbuf[64] = {};
    ssize_t n = read(client_fd, rbuf, sizeof(rbuf)-1);
    if(n<0) {
        die("read");
    }

    printf("server says: %s\n", msg);
    close(client_fd);
    return 0;
}

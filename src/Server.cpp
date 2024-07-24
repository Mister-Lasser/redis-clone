#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char* msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int conn_fd) {
    char rbuf[64] = {};
    ssize_t n = read(conn_fd, rbuf, sizeof(rbuf)-1);
    if (n<0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(conn_fd, wbuf, strlen(wbuf));
}

int main() {

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "Start of program\n";
    int server_fd = socket(AF_INET,SOCK_STREAM, 0);
    if(server_fd < 0) {
        die("socket()");
    }

    int val = 1;
    //might check this for error later
    setsockopt(
        server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &val,
        sizeof(val)
    );

    //bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); //wildcard address 0.0.0.0

    int rv = bind(server_fd, (const sockaddr *)&addr, sizeof(addr));
    if(rv) {
        die("bind()");
    }

    //listen
    rv = listen(server_fd, SOMAXCONN);
    if(rv) {
        die("listen()");
    }

    while (true) {
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int conn_fd = accept(
            server_fd,
            (struct sockaddr *)&client_addr,
            &socklen
        );
        if (conn_fd < 0) {
            continue; //error
        }

        do_something(conn_fd);
        close(conn_fd);
    }
    return 0;
}

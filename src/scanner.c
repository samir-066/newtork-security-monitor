#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

void scanner_init(void) {
    printf("Scanner initialized successfully.\n");
}

int scan_port(const char *ip, int port) {
    if(port<1 || port>65535)  //verification si le port est valide
        return -1;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd==-1) //on verifie le retour de socket
        return -1;

    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int ip_bin = inet_pton(AF_INET, ip, &addr.sin_addr); //conversion de l'adresse IP vers du binaire
    if(ip_bin != 1) { //on verifie que la conversion a fonctionne
        close(fd);
        return -1;
    }

    int result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if(result == 0) {
        close(fd);
        return 1;
    } else if (errno == ECONNREFUSED) {
        close(fd);
        return 0;
    } else {
        close(fd);
        return -1;
    }

    return -1;
}
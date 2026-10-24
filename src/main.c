#include <stdio.h>
#include "scanner.h"
#include <stdlib.h>
#include <arpa/inet.h>

static int estEntier(const char *ch) {
    int i = 0;
    if(ch[0] == '\0') return -1;
    while(ch[i] != '\0') {
        if(ch[i]<'0' || ch[i]>'9' || i>=5) return -1;
        i++;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <IP> <START_PORT> <END_PORT>\n", argv[0]);
        return 1;
    }

    if(estEntier(argv[2]) == -1 || estEntier(argv[3]) == -1) {
        printf("Les ports doivent contenir uniquement des chiffres. \n");
        return 1;
    }

    long start_port = strtol(argv[2], NULL, 10);
    long end_port = strtol(argv[3], NULL, 10);


    if((start_port<1 || start_port>65535) || (end_port<1 || end_port>65535)) {
        printf("Start Port et End Port doivent etre des entiers compris entre 1 et 65535\n");
        return 1;
    }


    if(start_port>end_port) {
        printf("End Port doit etre superieur ou egale a start port\n");
        return 1;
    }
    struct in_addr addr;
    if(inet_pton(AF_INET, argv[1], &addr) != 1) {
        printf("Adresse IP invalide\n");
        return 1;
    }

    printf("IP : %s\n", argv[1]);
    printf("Port debut : %ld\n", start_port);
    printf("Port fin : %ld\n\n", end_port);

    int ports_ouverts = 0;

    for (int i = (int)start_port; i <= (int)end_port; i++) {
        int result = scan_port(argv[1], i);

        if (result == 1) {
            printf("Port %d : Ouvert\n", i);
            ports_ouverts++;
        } else if (result == 0) {
            printf("Port %d : Ferme\n", i);
        } else {
            printf("Port %d : Erreur de connexion\n", i);
        }
    }   
    printf("Ports ouverts : %d\n", ports_ouverts);
    return 0;
}



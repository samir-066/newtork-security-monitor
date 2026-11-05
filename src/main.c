#include <stdio.h>
#include "scanner.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

static int lire_port(const char *texte, int *port) {
    char *fin;
    errno = 0;

    if (*texte=='\0') return 0;

    long valeur = strtol(texte, &fin, 10);

    if (errno!=0 || *fin!='\0' || valeur<1 || valeur>65535) return 0;

    *port = (int)valeur;
    return 1;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <START_PORT> <END_PORT>\n", argv[0]);
        return 1;
    }

    struct in_addr adresse;

    if (inet_pton(AF_INET, argv[1], &adresse) != 1) {
        fprintf(stderr, "Adresse IPv4 invalide\n");
        return 1;
    }
    
    int start_port, end_port;

    if(!lire_port(argv[2],&start_port) || !lire_port(argv[3], &end_port) || start_port>end_port) {
        fprintf(stderr, "Plage de ports invalide\n");
        return 1;
    }

    scanner_init();
    printf("IP : %s\n", argv[1]);
    printf("Ports : %d - %d\n\n", start_port, end_port);

    int resultat = scan_port(argv[1], start_port, end_port);
    int retour;
    if(resultat<0) retour = 1;
    else retour = 0;
    return retour;
}



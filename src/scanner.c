#define _POSIX_C_SOURCE 200809L
#define MAX_CONNEXIONS 50
#define TIMEOUT_MS 1000

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <time.h>
#include "scanner.h"

long long maintenant_ms(void) {
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime");
        return -1;
    }
    return (long long)ts.tv_sec*1000 + ts.tv_nsec/1000000;
}

void afficher_resultat(int port, int erreur) {
    if(erreur==0) printf("Port %d : Ouvert\n", port);
    else if(erreur==ECONNREFUSED) printf("Port %d : Ferme\n", port);
    else printf("Port %d : Erreur (%d)\n", port, erreur);
}

void scanner_init(void) {
    printf("Scanner TCP concurrent initialise.\n");
}

int scan_port(const char *ip, int start_port, int end_port) {
    struct in_addr adresse_ip;

    if(inet_pton(AF_INET, ip, &adresse_ip) != 1) {
        fprintf(stderr, "Adresse IPv4 invalide\n");
        return -1;
    }

    if(start_port<1 || end_port>65535 || start_port>end_port) { //verification si le port est valide
        fprintf(stderr, "Plage de ports invalide\n");
        return -1;
    }
    int total_ouverts = 0;
    int debut;
    for (debut = start_port; debut <= end_port;
         debut += MAX_CONNEXIONS) {

        int nombre = end_port - debut + 1;

        if (nombre > MAX_CONNEXIONS) {
            nombre = MAX_CONNEXIONS;
        }

        struct pollfd pfds[MAX_CONNEXIONS] = {0};
        int ports[MAX_CONNEXIONS] = {0};
        long long echeances[MAX_CONNEXIONS] = {0};
        int actifs = 0;

        for (int i = 0; i < nombre; i++) {
            ports[i] = debut + i;
            pfds[i].fd = -1;

            int fd = socket(AF_INET, SOCK_STREAM, 0);

            if (fd == -1) {
                perror("socket");
                continue;
            }

            int flags = fcntl(fd, F_GETFL, 0);

            if (flags == -1 ||
                fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
                perror("fcntl");
                close(fd);
                continue;
            }

            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(ports[i]);
            addr.sin_addr = adresse_ip;

            int resultat = connect(
                fd,
                (struct sockaddr *)&addr,
                sizeof(addr)
            );

            if (resultat == 0) {
                afficher_resultat(ports[i], 0);
                total_ouverts++;
                close(fd);
            }
            else if (errno == EINPROGRESS) {
                long long instant = maintenant_ms();

                if (instant == -1) {
                    close(fd);
                    continue;
                }

                pfds[i].fd = fd;
                pfds[i].events = POLLOUT;
                echeances[i] = instant + TIMEOUT_MS;
                actifs++;
            }
            else {
                afficher_resultat(ports[i], errno);
                close(fd);
            }
        }

        // Attendre toutes les connexions du lot
        while (actifs > 0) {
            long long instant = maintenant_ms();

            if (instant == -1) {
                break;
            }

            int attente_ms = TIMEOUT_MS;

            // Chercher la prochaine echeance
            for (int i = 0; i < nombre; i++) {
                if (pfds[i].fd == -1) {
                    continue;
                }

                long long restant = echeances[i] - instant;

                if (restant < 0) {
                    restant = 0;
                }

                if (restant < attente_ms) {
                    attente_ms = (int)restant;
                }
            }

            int retour = poll(pfds, nombre, attente_ms);

            if (retour == -1) {
                if (errno == EINTR) {
                    continue;
                }

                perror("poll");
                break;
            }

            instant = maintenant_ms();

            if (instant == -1) {
                break;
            }

            for (int i = 0; i < nombre; i++) {
                if (pfds[i].fd == -1) {
                    continue;
                }

                int termine = 0;

                if (pfds[i].revents != 0) {
                    if (pfds[i].revents & POLLNVAL) {
                        printf("Port %d : Descripteur invalide\n",
                               ports[i]);
                    }
                    else {
                        int erreur = 0;
                        socklen_t taille = sizeof(erreur);

                        if (getsockopt(
                            pfds[i].fd,
                            SOL_SOCKET,
                            SO_ERROR,
                            &erreur,
                            &taille
                        ) == -1) {
                            perror("getsockopt");
                        }
                        else {
                            afficher_resultat(ports[i], erreur);

                            if (erreur == 0) {
                                total_ouverts++;
                            }
                        }
                    }

                    termine = 1;
                }
                else if (instant >= echeances[i]) {
                    printf("Port %d : Timeout\n", ports[i]);
                    termine = 1;
                }

                if (termine) {
                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                    pfds[i].revents = 0;
                    actifs--;
                }
            }
        }

        // Nettoyage en cas d'erreur ou d'interruption
        for (int i = 0; i < nombre; i++) {
            if (pfds[i].fd != -1) {
                printf("Port %d : Analyse interrompue\n",
                       ports[i]);
                close(pfds[i].fd);
            }
        }
    }
    printf("\nPorts ouverts : %d\n", total_ouverts);
    return total_ouverts;
}
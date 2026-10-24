#include <stdio.h>
#include "scanner.h"

int main(void) {
    int start_port = 8078;
    int end_port = 8082;
    int result;
    int i;
    int port_ouvert = 0;
    for(i = start_port; i <= end_port; i++) {
        result = scan_port("127.0.0.1", i);
        if(result == 1) {
            printf("Port %d : Ouvert\n",i);
            port_ouvert++;
        } else if (result==0) {
            printf("Port %d : Ferme\n",i);
        } else {
            printf("Port %d : connexion echouee\n",i);
        }
    }
    printf("Port Ouvert : %d\n",port_ouvert);
    return 0;
}
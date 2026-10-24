#include <stdio.h>
#include "scanner.h"

int main(void) {
    int port = 8080;
    int result = scan_port("127.0.0.1", port);

    if(result == 1) {
        printf("Port %d : Ouvert\n",port);
    } else if (result==0) {
        printf("Port %d : Ferme\n",port);
    } else {
        printf("Port %d : connexion echouee\n",port);
    }
    
    return 0;
}
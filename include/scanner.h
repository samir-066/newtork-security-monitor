#ifndef SCANNER_H
#define SCANNER_H

void scanner_init(void);
int scan_port(const char *ip, int start_port, int end_port);

#endif
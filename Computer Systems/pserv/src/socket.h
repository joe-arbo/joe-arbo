#ifndef _SOCKET_H
#define _SOCKET_H

int socket_open_bind_listen(char * port_number_string, int backlog);
int socket_accept_client(int socket);

#endif /* _SOCKET_H */

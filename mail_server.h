#ifndef MAIL_SERVER_H
#define MAIL_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 55555
#define MAX_MESSAGE_SIZE 4096

int NR_CLIENTI=0;

struct _client{
    char* MailAdress;
    char* Nume;
    char* Prenume;
};
typedef struct _client Client;

struct _mail{
    char* Subiect;
    char* Message;
    Client* User;
};
typedef struct _mail Mail;

Client* addClient(Client* clienti, char* mailAdress, char* nume, char* prenume);
Client* removeClient(Client* clienti, char* mailAdress);

void loadClients(Client** clienti);
void saveClients(Client* clienti);

void handle_client(int client_socket);

#endif /* MAIL_SERVER_H */
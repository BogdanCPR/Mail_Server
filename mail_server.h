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

extern int NR_CLIENTI;

struct _client{
    char* MailAdress;
    char* Password;
};
typedef struct _client Client;

struct _mail{
    int MailId;
    char* Subject;
    char* Message;
    char* SenderAddress;
    char* ReceiverAddress;
};
typedef struct _mail Mail;

Client* addClient(Client* clienti, char* mailAdress, char* name);
Client* removeClient(Client* clienti, char* mailAdress);

void loadClients(Client** clienti);
void saveClients(Client* clienti);

void handle_client(int client_socket);

int generateRandomID();


// Client 

int show_login_register(char** mail, char** password);

int show_menu(char* mail, char* password);

void clear_console();



#endif /* MAIL_SERVER_H */
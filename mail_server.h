#ifndef MAIL_SERVER_H
#define MAIL_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define PORT 55555
#define MAX_MESSAGE_SIZE 4096

#define SENDER_ONLY 1
#define RECEIVER_ONLY 2
#define DEFAULT 3

#define ACTION_RM_SENDER 1
#define ACTION_RM_RECEIVER 2
#define ACTION_DELETE 3

int NR_CLIENTS=0;
int NR_MAILS=0;

struct _client{
    char* MailAdress;
    char* Name;
};
typedef struct _client Client;

struct _mail{
    int flag;
    int MailId;
    char* Subject;
    char* Message;
    char* SenderAddress;
    char* ReceiverAddress;
};
typedef struct _mail Mail;

Client* addClient(Client* clients, char* mailAdress, char* name);
Client* removeClient(Client* clients, char* mailAdress);

void loadClients(Client** clients);
void saveClients(Client* clients);

void handle_client(int client_socket);

Mail* addMail(Mail* mails, char* subject, char* message, char* senderAddress, char* receiverAddress, int flag, int id);
Mail* removeMail(Mail* mails,int mailId, int action);

void loadMails (Mail** mails);
void saveMails(Mail* mails);

int generateRandomID();
void itoa(int num, char **string);

void breakpoint(char* message);

#endif /* MAIL_SERVER_H */
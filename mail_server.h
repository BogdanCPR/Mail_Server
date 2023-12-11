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
#include <pthread.h>
#include <signal.h>

#define PORT 55555
#define MAX_MESSAGE_SIZE 4096

#define SENDER_ONLY 1
#define RECEIVER_ONLY 2
#define DEFAULT 3

#define ACTION_RM_SENDER 1
#define ACTION_RM_RECEIVER 2
#define ACTION_DELETE 3

extern int NR_CLIENTS;
extern int NR_MAILS;

struct _client{
    int sessionID;
    char* MailAdress;
    char* Password;
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

// Client 

int show_login_register(char** mail, char** password);
int show_menu(char* mail, char* password);
int connect_to_server(char* ip);

void clear_console();
void press_enter_to_continue();
void set_client_fd(int fd);

char* encrypt(const char *data, int sessionID);
char* decrypt(const char *data, int sessionID);

// Server
Client* addClient(Client* clients, char* mailAdress, char* name);
Client* removeClient(Client* clients, char* mailAdress);

void loadClients(Client** clients);
void saveClients(Client* clients);

int handle_client(int client_socket, Client **clients, Mail **mails);

Mail* addMail(Mail* mails, char* subject, char* message, char* senderAddress, char* receiverAddress, int flag, int id);
Mail* removeMail(Mail* mails,int mailId, int action);

void loadMails (Mail** mails);
void saveMails(Mail* mails);

int generateRandomID();
void itoa(int num, char **string);

void breakpoint(char* message);

#endif /* MAIL_SERVER_H */
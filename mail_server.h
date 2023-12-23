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
#include <string.h>  
#include <termios.h>

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

typedef unsigned long long int RSA_key_t;

struct RSA_key{
    RSA_key_t n;
    RSA_key_t ed;
};
typedef struct RSA_key RSA_Key;

struct RSA_key_pair{
    RSA_Key publicKey;
    RSA_Key privateKey;
};

typedef struct RSA_key_pair RSA_KeyPair;

//Encryption / Decryption
RSA_key_t* encryptMessage(char*message,RSA_Key key);
char* decryptMessage(RSA_key_t* encryptedMessage, RSA_key_t size, RSA_Key key);
RSA_key_t encrypt(RSA_key_t message, RSA_Key key);
RSA_key_t decrypt(RSA_key_t message, RSA_Key key);
RSA_KeyPair generateKeyPair();

int sendEncryptedMessage(char *message, int socket, RSA_Key Key);
int receiveDecryptedMessage(char **decryptedMessage, int socket, RSA_Key Key);

// Client 
int show_login_register(char** mail, char** password);
int show_menu(char* mail, char* password);
int connect_to_server(char* ip);

void clear_console();
void press_enter_to_continue();
void set_client_fd(int fd);
void receiveKey(int cfd);
void printMailPattern();

char* old_encrypt(const char *data, int sessionID);
char* old_decrypt(const char *data, int sessionID);

// Server
void initialiseLog();
void addLog(char* message);

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
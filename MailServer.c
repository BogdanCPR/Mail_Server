#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 55555
#define MAX_MESSAGE_SIZE 1024

int NR_CLIENTI=0;

typedef struct{
    char* MailAdress;
    char* Nume;
    char* Prenume;
}Client;

typedef struct{
    char* Subiect;
    char* Message;
    Client* User;
}Mail;

Client* addClient(Client* clienti, char* mailAdress, char* nume, char* prenume)
{
    NR_CLIENTI++;
    Client *AUX = (Client*)malloc(NR_CLIENTI*sizeof(Client));
    for(int i=0;i<NR_CLIENTI-1;i++)
    {
        AUX[i].MailAdress = (char*)malloc((strlen(clienti[i].MailAdress)+1)*sizeof(char));
        AUX[i].Nume = (char*)malloc((strlen(clienti[i].Nume)+1)*sizeof(char));
        AUX[i].Prenume = (char*)malloc((strlen(clienti[i].Prenume)+1)*sizeof(char));
        strcpy(AUX[i].MailAdress,clienti[i].MailAdress);
        strcpy(AUX[i].Nume,clienti[i].Nume);
        strcpy(AUX[i].Prenume,clienti[i].Prenume);
    }
    AUX[NR_CLIENTI-1].MailAdress = (char*)malloc(strlen(mailAdress)*sizeof(char));
    AUX[NR_CLIENTI-1].Nume = (char*)malloc(strlen(nume)*sizeof(char));
    AUX[NR_CLIENTI-1].Prenume = (char*)malloc(strlen(prenume)*sizeof(char));
    strcpy(AUX[NR_CLIENTI-1].MailAdress,mailAdress);
    strcpy(AUX[NR_CLIENTI-1].Nume,nume);
    strcpy(AUX[NR_CLIENTI-1].Prenume,prenume);

    return AUX;
}

void loadClients(Client** clienti)
{
    int fd = open("clients.db", O_RDONLY);
    if(fd == -1)
    {
        perror("error opening file");
        return;
    }
    
    char buf[1024];
    char adresa[1024];
    char nume[1024];
    char prenume[1024];
    int i=0;
    char citit[1];
    while (read(fd,citit,1) > 0) 
    {
        if(citit[0] != '\n')
        {
            buf[i++]=citit[0];
        }   
        else
        {
            buf[i]='\0';
            char *token = strtok(buf, " ");
            strcpy(adresa,token);
            
            token=strtok(NULL," ");
            strcpy(nume,token);

            token=strtok(NULL,"\n");
            strcpy(prenume,token);

            *clienti = addClient(*clienti,adresa,nume,prenume);
            i=0;
        } 
    }

    close(fd);
}


Client* removeClient(Client* clienti, char* mailAdress)
{
    NR_CLIENTI--;
    int k=0;
    Client *AUX = (Client*)malloc(NR_CLIENTI*sizeof(Client));
    for(int i=0;i<NR_CLIENTI+1;i++)
    {
        if(strcmp(clienti[i].MailAdress,mailAdress))
        {
            AUX[k].MailAdress = (char*)malloc((strlen(clienti[i].MailAdress)+1)*sizeof(char));
            AUX[k].Nume = (char*)malloc((strlen(clienti[i].Nume)+1)*sizeof(char));
            AUX[k].Prenume = (char*)malloc((strlen(clienti[i].Prenume)+1)*sizeof(char));
            strcpy(AUX[k].MailAdress,clienti[i].MailAdress);
            strcpy(AUX[k].Nume,clienti[i].Nume);
            strcpy(AUX[k].Prenume,clienti[i].Prenume);
            k++;
        }
    }
    return AUX;
}

void saveClients(Client* clienti)
{
    int fd = open("clients.db", O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd == -1)
    {
        perror("error opening file");
        return;
    }

    for (int i = 0; i < NR_CLIENTI; i++) {
        write(fd, clienti[i].MailAdress, strlen(clienti[i].MailAdress));
        write(fd, " ", 1);
        write(fd, clienti[i].Nume, strlen(clienti[i].Nume));
        write(fd, " ", 1);
        write(fd, clienti[i].Prenume, strlen(clienti[i].Prenume));
        write(fd, "\n", 1);
    }

    close(fd);
}



void handle_client(int client_socket) {
    char buffer[MAX_MESSAGE_SIZE];
    int message_len;

    while(1) {
        message_len = recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);

        if(message_len <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        buffer[message_len] = '\0'; // Add null terminator to treat received data as string

        // Process the message (e.g., check recipient, format, etc.)
        printf("Received message: %s\n", buffer);

        // Assuming the message is valid, forward it to the client application
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    Client* clienti = NULL;

    loadClients(&clienti);
    //init(&clienti);
    // printf("Adresa: %s\nNume: %s\nPrenume: %s",clienti[0].MailAdress,clienti[0].Nume,clienti[0].Prenume);
    // clienti = addClient(clienti,"capritabogdan@casin.ro","Caprita","Bogdan");
    // clienti = addClient(clienti,"sindilarstefan@casin.ro","Sindilar","Stefan");
    // saveClients(clienti);

    printf("Adresa: %s\nNume: %s\nPrenume: %s",clienti[1].MailAdress,clienti[1].Nume,clienti[1].Prenume);
    // saveClients(clienti);
    // clienti = removeClient(clienti,"test@casin.ro");
    // printf("Adresa: %s\nNume: %s\nPrenume: %s",clienti[0].MailAdress,clienti[0].Nume,clienti[0].Prenume);

    

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Listen for incoming connections
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", PORT);

    while(1) {
        // Accept connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);

        printf("New connection accepted\n");

        // Handle client
        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}

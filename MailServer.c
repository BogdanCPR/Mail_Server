#include "mail_server.h"

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
    int fd = open("clients.db", O_WRONLY | O_TRUNC | O_CREAT, 0644);
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

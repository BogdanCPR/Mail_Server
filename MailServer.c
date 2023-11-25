#include "mail_server.h"

int NR_CLIENTI = 0;

int generateRandomID()
{
    int fd = open("ids.db", O_RDONLY);
    if(fd == -1)
    {
        perror("error opening file");
        return 0; 
    }
    int existingNumber;
    int ID = rand()%100000;
    int foundDuplicate=0;
    while (read(fd,&existingNumber,sizeof(int))>0)
    {
        if(ID == existingNumber)
        {
            foundDuplicate=1;
            break;
        }
    }
    close(fd);
    if(foundDuplicate)
    {
        return generateRandomID();
    }
    fd = open("ids.db", O_WRONLY | O_APPEND);
    if(fd == -1)
    {
        perror("error opening file");
        return 0;
    }
    if(write(fd,&ID,sizeof(int))== -1)
    {
        perror("error writing in file");
        return 0;
    }
    close(fd);
}


Client* addClient(Client* clienti, char* mailAdress, char* password)
{
    NR_CLIENTI++;
    Client *AUX = (Client*)malloc(NR_CLIENTI*sizeof(Client));
    for(int i=0;i<NR_CLIENTI-1;i++)
    {
        AUX[i].MailAdress = (char*)malloc((strlen(clienti[i].MailAdress)+1)*sizeof(char));
        AUX[i].Password = (char*)malloc((strlen(clienti[i].Password)+1)*sizeof(char));
        strcpy(AUX[i].MailAdress,clienti[i].MailAdress);
        strcpy(AUX[i].Password,clienti[i].Password);
    }
    AUX[NR_CLIENTI-1].MailAdress = (char*)malloc(strlen(mailAdress)*sizeof(char));
    AUX[NR_CLIENTI-1].Password = (char*)malloc(strlen(password)*sizeof(char));
    strcpy(AUX[NR_CLIENTI-1].MailAdress,mailAdress);
    strcpy(AUX[NR_CLIENTI-1].Password,password);

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
    int id;
    char subject[1024];
    char message[4096];
    char adress[64];
    char name[64];
    int i=0;
    char readed[1];
    while (read(fd,readed,1) > 0) 
    {
        if(readed[0] != '\n')
        {
            buf[i++]=readed[0];
        }   
        else
        {
            buf[i]='\0';
            char *token = strtok(buf, " ");
            strcpy(adress,token);
            
            token=strtok(NULL,"\n");
            strcpy(name,token);

            *clienti = addClient(*clienti,adress,name);
            i=0;
        } 
    }

    close(fd);
}
//format: sourceAdress destinationAdress subject<\n>
//        message
//        <\n>.<\n>
void LoadMails (Mail** mails)
{
    int fd = open("mails.db", O_RDONLY);
    if(fd == -1)
    {
        perror("error opening file");
        return;
    }

    char buf[4096];
    int ID;
    char sourceAdress[1024];
    char destinationAdress[1024];
    char subject[1024];
    char message[1024];

    int i=0;
    char readed[1];

    while (read(fd,readed,1) > 0) 
    {
        if(readed[0] != '\n')
        {
            buf[i++]=readed[0];
        }   
        else
        {
            
        } 
    }
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
            AUX[k].Password = (char*)malloc((strlen(clienti[i].Password)+1)*sizeof(char));
            strcpy(AUX[k].MailAdress,clienti[i].MailAdress);
            strcpy(AUX[k].Password,clienti[i].Password);
            k++;
        }
    }
    return AUX;
}

Mail* removeMail(Mail* mails,int ID)
{

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
        write(fd, clienti[i].Password, strlen(clienti[i].Password));
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

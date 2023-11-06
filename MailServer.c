#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 53555
#define MAX_MESSAGE_SIZE 1024

int NR_CLIENTI=0;
int NR_MAILS=0;

typedef struct{
    char* MailAdress;
    char* Name;
}Client;

typedef struct{
    int ID;
    char* Subiect;
    char* Message;
    char* SourceAdress;
    char* DestinationAdress;
}Mail;

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


Client* addClient(Client* clienti, char* mailAdress, char* name)
{
    NR_CLIENTI++;
    Client *AUX = (Client*)malloc(NR_CLIENTI*sizeof(Client));
    for(int i=0;i<NR_CLIENTI-1;i++)
    {
        AUX[i].MailAdress = (char*)malloc((strlen(clienti[i].MailAdress)+1)*sizeof(char));
        AUX[i].Name = (char*)malloc((strlen(clienti[i].Name)+1)*sizeof(char));
        strcpy(AUX[i].MailAdress,clienti[i].MailAdress);
        strcpy(AUX[i].Name,clienti[i].Name);
    }
    AUX[NR_CLIENTI-1].MailAdress = (char*)malloc(strlen(mailAdress)*sizeof(char));
    AUX[NR_CLIENTI-1].Name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(AUX[NR_CLIENTI-1].MailAdress,mailAdress);
    strcpy(AUX[NR_CLIENTI-1].Name,name);

    return AUX;
}

Mail* addNewMail(Mail* mails, char* subject, char* message, char* sourceAdress, char* destinationAdress)
{
    NR_MAILS++;
    Mail * AUX = (Mail*) malloc (NR_MAILS*sizeof(Mail));
    for(int i=0;i<NR_MAILS;i++)
    {
        AUX[i].Subiect =(char*)malloc((strlen(mails[i].Subiect)+1)*sizeof(char));
        AUX[i].Message =(char*)malloc((strlen(mails[i].Message)+1)*sizeof(char));
        AUX[i].SourceAdress =(char*)malloc((strlen(mails[i].SourceAdress)+1)*sizeof(char));
        AUX[i].DestinationAdress =(char*)malloc((strlen(mails[i].DestinationAdress)+1)*sizeof(char));
        strcpy(AUX[i].Subiect,mails[i].Subiect);
        strcpy(AUX[i].Message,mails[i].Message);
        strcpy(AUX[i].SourceAdress,mails[i].SourceAdress);
        strcpy(AUX[i].DestinationAdress,mails[i].DestinationAdress);
        AUX[i].ID=mails[i].ID;
    }

    AUX[NR_CLIENTI-1].Subiect = (char*)malloc(strlen(subject)*sizeof(char));
    AUX[NR_CLIENTI-1].Message = (char*)malloc(strlen(message)*sizeof(char));
    AUX[NR_CLIENTI-1].SourceAdress = (char*)malloc(strlen(sourceAdress)*sizeof(char));
    AUX[NR_CLIENTI-1].DestinationAdress = (char*)malloc(strlen(destinationAdress)*sizeof(char));

    AUX[NR_CLIENTI-1].ID=generateRandomID();
    strcpy(AUX[NR_CLIENTI-1].Subiect,subject);
    strcpy(AUX[NR_CLIENTI-1].Message,message);
    strcpy(AUX[NR_CLIENTI-1].SourceAdress,sourceAdress);
    strcpy(AUX[NR_CLIENTI-1].DestinationAdress,destinationAdress);
    

    return AUX;
}

Mail* AddMail(Mail* mails,int id, char* subject, char* message, char* sourceAdress, char* destinationAdress)
{
    NR_MAILS++;
    Mail * AUX = (Mail*) malloc (NR_MAILS*sizeof(Mail));
    for(int i=0;i<NR_MAILS;i++)
    {
        AUX[i].Subiect =(char*)malloc((strlen(mails[i].Subiect)+1)*sizeof(char));
        AUX[i].Message =(char*)malloc((strlen(mails[i].Message)+1)*sizeof(char));
        AUX[i].SourceAdress =(char*)malloc((strlen(mails[i].SourceAdress)+1)*sizeof(char));
        AUX[i].DestinationAdress =(char*)malloc((strlen(mails[i].DestinationAdress)+1)*sizeof(char));
        strcpy(AUX[i].Subiect,mails[i].Subiect);
        strcpy(AUX[i].Message,mails[i].Message);
        strcpy(AUX[i].SourceAdress,mails[i].SourceAdress);
        strcpy(AUX[i].DestinationAdress,mails[i].DestinationAdress);
        AUX[i].ID=mails[i].ID;
    }

    AUX[NR_CLIENTI-1].Subiect = (char*)malloc(strlen(subject)*sizeof(char));
    AUX[NR_CLIENTI-1].Message = (char*)malloc(strlen(message)*sizeof(char));
    AUX[NR_CLIENTI-1].SourceAdress = (char*)malloc(strlen(sourceAdress)*sizeof(char));
    AUX[NR_CLIENTI-1].DestinationAdress = (char*)malloc(strlen(destinationAdress)*sizeof(char));

    AUX[NR_CLIENTI-1].ID=id;
    strcpy(AUX[NR_CLIENTI-1].Subiect,subject);
    strcpy(AUX[NR_CLIENTI-1].Message,message);
    strcpy(AUX[NR_CLIENTI-1].SourceAdress,sourceAdress);
    strcpy(AUX[NR_CLIENTI-1].DestinationAdress,destinationAdress);
    

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
            AUX[k].Name = (char*)malloc((strlen(clienti[i].Name)+1)*sizeof(char));
            strcpy(AUX[k].MailAdress,clienti[i].MailAdress);
            strcpy(AUX[k].Name,clienti[i].Name);
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
    int fd = open("clients.db", O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd == -1)
    {
        perror("error opening file");
        return;
    }

    for (int i = 0; i < NR_CLIENTI; i++) {
        write(fd, clienti[i].MailAdress, strlen(clienti[i].MailAdress));
        write(fd, " ", 1);
        write(fd, clienti[i].Name, strlen(clienti[i].Name));
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
    Mail* mailuri = NULL;
    srand(time(NULL));


    loadClients(&clienti);
    //init(&clienti);
    // printf("Adresa: %s\nNume: %s\nPrenume: %s",clienti[0].MailAdress,clienti[0].Nume,clienti[0].Prenume);
    // clienti = addClient(clienti,"capritabogdan@casin.ro","Caprita","Bogdan");
    // clienti = addClient(clienti,"sindilarstefan@casin.ro","Sindilar","Stefan");
    // saveClients(clienti);

    printf("Adress: %s\nName: %s\n",clienti[1].MailAdress,clienti[1].Name);
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

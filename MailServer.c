#include "mail_server.h"

int NR_CLIENTS = 0;
int NR_MAILS = 0;

void breakpoint(char *message)
{
    printf("%s\n", message);
    exit(0);
}

void reverseString(char **str, int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        // Schimbăm caracterele de pe pozițiile start și end
        char temp = (*str)[start];
        (*str)[start] = (*str)[end];
        (*str)[end] = temp;

        // Mutăm în interiorul șirului
        start++;
        end--;
    }
}

void itoa(int nr, char **string)
{
    *string = (char *)malloc(12);
    if (*string == NULL)
    {
        fprintf(stderr, "Eroare la alocarea memoriei");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int esteNegativ = 0;
    if (nr < 0)
    {
        esteNegativ = 1;
        nr = -nr;
    }
    do
    {
        (*string)[i++] = nr % 10 + '0';
        nr /= 10;
    } while (nr != 0);
    if (esteNegativ)
    {
        (*string)[i++] = '-';
    }
    (*string)[i] = '\0';
    reverseString(string, i);
}

int generateRandomID()
{
    int fd = open("ids.db", O_RDONLY);
    if (fd == -1)
    {
        perror("error opening file");
        return 0;
    }
    int existingNumber;
    int ID;
    int foundDuplicate = 1;
    while (foundDuplicate == 1)
    {
        ID = rand() % 100000;
        lseek(fd, 0, SEEK_SET);
        foundDuplicate = 0;
        while (read(fd, &existingNumber, sizeof(int)) > 0)
        {
            if (ID == existingNumber)
            {
                foundDuplicate = 1;
                break;
            }
        }
    }
    close(fd);
    fd = open("ids.db", O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        perror("error opening file");
        return 0;
    }

    if (write(fd, &ID, sizeof(int)) == -1)
    {
        perror("error writing in file");
        return 0;
    }
    close(fd);
}

Client *addClient(Client *clienti, char *mailAdress, char *password)
{
    NR_CLIENTS++;
    Client *AUX = (Client *)malloc(NR_CLIENTS * sizeof(Client));
    for (int i = 0; i < NR_CLIENTS - 1; i++)
    {
        AUX[i].MailAdress = (char *)malloc((strlen(clienti[i].MailAdress) + 1) * sizeof(char));
        AUX[i].Password = (char *)malloc((strlen(clienti[i].Password) + 1) * sizeof(char));
        strcpy(AUX[i].MailAdress, clienti[i].MailAdress);
        strcpy(AUX[i].Password, clienti[i].Password);
    }
    AUX[NR_CLIENTS - 1].MailAdress = (char *)malloc(strlen(mailAdress) * sizeof(char));
    AUX[NR_CLIENTS - 1].Password = (char *)malloc(strlen(password) * sizeof(char));
    strcpy(AUX[NR_CLIENTS - 1].MailAdress, mailAdress);
    strcpy(AUX[NR_CLIENTS - 1].Password, password);

    return AUX;
}

void loadClients(Client **clienti)
{
    int fd = open("clients.db", O_RDONLY);
    if (fd == -1)
    {
        perror("error opening file");
        return;
    }

    char buf[1024];
    char adress[64];
    char password[64];
    int i = 0;
    char readed[1];
    while (read(fd, readed, 1) > 0)
    {
        if (readed[0] != '\n')
        {
            buf[i++] = readed[0];
        }
        else
        {
            buf[i] = '\0';
            char *token = strtok(buf, " ");
            strcpy(adress, token);

            token = strtok(NULL, "\n");
            strcpy(password, token);

            *clienti = addClient(*clienti, adress, password);
            i = 0;
        }
    }

    close(fd);
}

Client *removeClient(Client *clienti, char *mailAdress)
{
    NR_CLIENTS--;
    int k = 0;
    Client *AUX = (Client *)malloc(NR_CLIENTS * sizeof(Client));
    for (int i = 0; i < NR_CLIENTS + 1; i++)
    {
        if (strcmp(clienti[i].MailAdress, mailAdress))
        {
            AUX[k].MailAdress = (char *)malloc((strlen(clienti[i].MailAdress) + 1) * sizeof(char));
            AUX[k].Password = (char *)malloc((strlen(clienti[i].Password) + 1) * sizeof(char));
            strcpy(AUX[k].MailAdress, clienti[i].MailAdress);
            strcpy(AUX[k].Password, clienti[i].Password);
            k++;
        }
    }
    return AUX;
}

void saveClients(Client *clienti)
{
    int fd = open("clients.db", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("error opening file");
        return;
    }
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        write(fd, clienti[i].MailAdress, strlen(clienti[i].MailAdress));
        write(fd, " ", 1);
        write(fd, clienti[i].Password, strlen(clienti[i].Password));
        write(fd, "\n", 1);
    }

    close(fd);
}
//rethink it;;;;;;;;
int validate_client(char *client_message, int client_socket, Client* clients)
{
    char *commandIdentifier = strtok(client_message, "/");
    char *command = strtok(NULL, "~");
    if (!strcmp(commandIdentifier, "LG"))
    {
        char* mail = strtok(command,"/");
        char* password = strtok(NULL,"\0");
        for(int i=0;i<NR_CLIENTS;i++)
        {
            if(!strcmp(mail,clients[i].MailAdress) && !strcmp(password,clients[i].Password))
            {
                //log login success
                int sessionId = rand()%10000;
                clients[i].sessionID = sessionId;
                return sessionId;
            }
        }
        return 0;
    }
    if (!strcmp(commandIdentifier, "RG"))
    {
        char* mail = strtok(command,"/");
        char* password = strtok(NULL,"\0");
        for(int i=0;i<NR_CLIENTS;i++)
        {
            if(!strcmp(mail,clients[i].MailAdress))
            {
                //log address already exists
                return 0;
            }
        }
        addClient(clients, mail, password);
        return 0;
    }
    return -1;
}

int handle_client(int client_socket, Client* clients)
{
    char buffer[MAX_MESSAGE_SIZE];
    int message_len;
    printf("CLIENT FD %d\n", client_socket);
    while (1)
    {
        message_len = recv(client_socket, buffer, MAX_MESSAGE_SIZE, 0);
        if (message_len <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }
        buffer[message_len] = '\0';
        int returnCode = validate_client(buffer,client_socket, clients);
        if(returnCode == -1)
        {
            close(client_socket);
            return -1;
        }


    }
    close(client_socket);
    return 1;
}

/*
flag:
`SENDER_ONLY` only sender will see the mail
`RECEIVER_ONLY` only receiver will see the mail
`DEFAULT` both sender and receiver will see the mail

id:
`-1` generate random unique id
`>0` use specific id (WILL NOT BE ADDED IN ids.db)
*/
Mail *addMail(Mail *mails, char *subject, char *message, char *senderAddress, char *receiverAddress, int flag, int id)
{
    NR_MAILS++;
    Mail *AUX = (Mail *)malloc(NR_MAILS * sizeof(Mail));
    for (int i = 0; i < NR_MAILS - 1; i++)
    {
        AUX[i].Message = (char *)malloc((strlen(mails[i].Message) + 1) * sizeof(char));
        AUX[i].Subject = (char *)malloc((strlen(mails[i].Subject) + 1) * sizeof(char));
        AUX[i].SenderAddress = (char *)malloc((strlen(mails[i].SenderAddress) + 1) * sizeof(char));
        AUX[i].ReceiverAddress = (char *)malloc((strlen(mails[i].ReceiverAddress) + 1) * sizeof(char));
        strcpy(AUX[i].Message, mails[i].Message);
        strcpy(AUX[i].Subject, mails[i].Subject);
        strcpy(AUX[i].SenderAddress, mails[i].SenderAddress);
        strcpy(AUX[i].ReceiverAddress, mails[i].ReceiverAddress);
        AUX[i].MailId = mails[i].MailId;
        AUX[i].flag = mails[i].flag;
    }
    AUX[NR_MAILS - 1].Message = (char *)malloc((strlen(message) + 1) * sizeof(char));
    AUX[NR_MAILS - 1].Subject = (char *)malloc((strlen(subject) + 1) * sizeof(char));
    AUX[NR_MAILS - 1].SenderAddress = (char *)malloc((strlen(senderAddress) + 1) * sizeof(char));
    AUX[NR_MAILS - 1].ReceiverAddress = (char *)malloc((strlen(receiverAddress) + 1) * sizeof(char));
    strcpy(AUX[NR_MAILS - 1].Message, message);
    strcpy(AUX[NR_MAILS - 1].Subject, subject);
    strcpy(AUX[NR_MAILS - 1].SenderAddress, senderAddress);
    strcpy(AUX[NR_MAILS - 1].ReceiverAddress, receiverAddress);
    if (id == -1)
    {
        AUX[NR_MAILS - 1].MailId = generateRandomID();
    }
    else
    {
        AUX[NR_MAILS - 1].MailId = id;
    }
    AUX[NR_MAILS - 1].flag = flag;
    return AUX;
}

/*action:
          `ACTION_RM_SENDER` removes mail from sender
          `ACTION_RM_RECEIVER` removes mail from receiver
          `ACTION_DELETE` deletes mail from database*/
Mail *removeMail(Mail *mails, int mailId, int action)
{
    int flag;
    if (action == ACTION_RM_SENDER)
    {
        int ok = 0;
        for (int i = 0; i < NR_MAILS; i++)
        {
            if (mails[i].MailId == mailId)
            {
                ok = 1;
                mails[i].flag &= 2;
                flag = mails[i].flag;
            }
        }
        if (ok)
        {
            // log remove from sender
        }
        else
        {
            // log already removed from sender
        }
    }
    if (action == ACTION_RM_RECEIVER)
    {
        int ok = 0;
        for (int i = 0; i < NR_MAILS; i++)
        {
            if (mails[i].MailId == mailId)
            {
                ok = 1;
                mails[i].flag &= 1;
                flag = mails[i].flag;
            }
        }
        if (ok)
        {
            // log remove from receiver
        }
        else
        {
            // log already removed from receiver
        }
    }
    if (action == ACTION_DELETE || flag == 0)
    {
        NR_MAILS--;
        int k = 0;
        Mail *AUX = (Mail *)malloc(NR_MAILS * sizeof(Mail));
        int ok = 0;
        for (int i = 0; i < NR_MAILS + 1; i++)
        {
            if (mails[i].MailId != mailId)
            {
                AUX[k].Message = (char *)malloc((strlen(mails[i].Message) + 1) * sizeof(char));
                AUX[k].Subject = (char *)malloc((strlen(mails[i].Subject) + 1) * sizeof(char));
                AUX[k].SenderAddress = (char *)malloc((strlen(mails[i].SenderAddress) + 1) * sizeof(char));
                AUX[k].ReceiverAddress = (char *)malloc((strlen(mails[i].ReceiverAddress) + 1) * sizeof(char));
                strcpy(AUX[k].Message, mails[i].Message);
                strcpy(AUX[k].Subject, mails[i].Subject);
                strcpy(AUX[k].SenderAddress, mails[i].SenderAddress);
                strcpy(AUX[k].ReceiverAddress, mails[i].ReceiverAddress);
                AUX[k].MailId = mails[i].MailId;
                AUX[k].flag = mails[i].flag;
                k++;
            }
            else
            {
                ok = 1;
            }
        }
        if (ok)
        {
            // log mail deleted
            return AUX;
        }
        // log tried to remove inexistent mail
        NR_MAILS++;
        return mails;
    }
    return mails;
}

void loadMails(Mail **mails)
{
    int fd = open("mails.db", O_RDONLY);
    if (fd == -1)
    {
        perror("error opening file");
        return;
    }

    char buf[8192];
    int id;
    int flag;
    char subject[1024];
    char message[4096];
    char senderAddress[64];
    char receiverAddress[64];
    int i = 0;
    char readed[1];
    while (read(fd, readed, 1) > 0)
    {
        if (readed[0] != '~')
        {
            buf[i++] = readed[0];
        }
        else
        {
            read(fd, readed, 1); // scapa de ultimul '\n'
            buf[i] = '\0';
            // flag
            char *token = strtok(buf, " ");
            flag = atoi(token);
            // ID
            token = strtok(NULL, " ");
            id = atoi(token);
            // sender address
            token = strtok(NULL, " ");
            strcpy(senderAddress, token);
            // receiver address
            token = strtok(NULL, " ");
            strcpy(receiverAddress, token);
            // subject
            token = strtok(NULL, "\n");
            strcpy(subject, token);
            // message
            token = strtok(NULL, "~");
            strcpy(message, token);
            i = 0;
            *mails = addMail(*mails, subject, message, senderAddress, receiverAddress, flag, id);
        }
    }

    close(fd);
}
void saveMails(Mail *mails)
{
    int fd = open("mails.db", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    int fd_id = open("ids.db", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1 || fd_id == -1)
    {
        perror("error opening files");
        return;
    }

    for (int i = 0; i < NR_MAILS; i++)
    {
        char *id = NULL;
        itoa(mails[i].MailId, &id);
        char *flag = NULL;
        itoa(mails[i].flag, &flag);
        write(fd, flag, strlen(flag));
        write(fd, " ", 1);
        write(fd, id, strlen(id));
        write(fd, " ", 1);
        write(fd, mails[i].SenderAddress, strlen(mails[i].SenderAddress));
        write(fd, " ", 1);
        write(fd, mails[i].SenderAddress, strlen(mails[i].SenderAddress));
        write(fd, " ", 1);
        write(fd, mails[i].Subject, strlen(mails[i].Subject));
        write(fd, "\n", 1);
        write(fd, mails[i].Message, strlen(mails[i].Message));
        write(fd, "~\n", 2);
        write(fd_id, &(mails[i].MailId), sizeof(int));
    }
    close(fd);
}

#include "mail_server.h"

int NR_CLIENTS = 0;
int NR_MAILS = 0;

void breakpoint(char *message)
{
    printf("%s\n", message);
    exit(0);
}

void initialiseLog()
{
    int log_fd = open("server.log", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (log == NULL)
    {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
    char message[] = "Server started";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time[30];
    sprintf(time,"%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    write(log_fd, time, strlen(time));
    write(log_fd, message, strlen(message));
    write(log_fd, "\n\n", 2);
    close(log_fd);
}

void addLog(char *message)
{
    int log_fd = open("server.log", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (log == NULL)
    {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char time[30];
    sprintf(time,"%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    write(log_fd, time, strlen(time));
    write(log_fd, message, strlen(message));
    write(log_fd, "\n\n", 2);
    close(log_fd);
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
    return ID;
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

int request_login(char *request, int client_socket, Client **clients, RSA_Key privateKey)
{
    char *mail = strtok(request, "/");
    char *password = strtok(NULL, "~");
    printf("Clients's creditentials for login:\nMail: %s\nPassword: %s\n", mail, password);
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp(mail, (*clients)[i].MailAdress) && !strcmp(password, (*clients)[i].Password))
        {
            // log login success
            int sessionId = (rand() % 90000) + 10000;
            (*clients)[i].sessionID = sessionId;
            char *SSID_string;
            itoa(sessionId, &SSID_string);
            int bytesSend = sendEncryptedMessage(SSID_string, client_socket, privateKey);
            //int bytesSend = send(client_socket, SSID_string, 5, 0);
            if (bytesSend < 1)
            {
                perror("error sending reply");
                exit(0);
            }
            char logMessage[100];
            sprintf(logMessage,"Client successfully loged in with the session id: %s\n", SSID_string);
            addLog(logMessage);
            printf("Client successfully loged in with the session id: %s\n", SSID_string);

            return 1;
        }
    }
    // creditentals were incorrect
    //int bytesSend = send(client_socket, "0", 1, 0);
    int bytesSend = sendEncryptedMessage("0", client_socket, privateKey);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        exit(0);
    }
    printf("Client's creditentials were incorect.\n");
    addLog("Client's creditentials were incorect.\n");
    return 0;
}

int request_register(char *request, int client_socket, Client **clients, RSA_Key privateKey)
{
    char *mail = strtok(request, "/");
    char *password = strtok(NULL, "~");
    char logMessage[100];
    printf("Clients's creditentials for register:\nMail: %s\nPassword: %s\n", mail, password);
    sprintf(logMessage,"Clients's creditentials for register:\nMail: %s\nPassword: %s\n", mail, password);
    addLog(logMessage);
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp(mail, (*clients)[i].MailAdress))
        {
            //int bytesSend = send(client_socket, "0", 1, 0);
            int bytesSend = sendEncryptedMessage("0", client_socket, privateKey);
            if (bytesSend < 1)
            {
                perror("error sending reply");
                exit(0);
            }
            printf("Client introduced an email that already exists");
            addLog("Client introduced an email that already exists");
            return 1;
        }
    }
    *clients = addClient(*clients, mail, password);
    if (clients == NULL)
    {
        perror("error adding client");
        exit(0);
    }
    //int bytesSend = send(client_socket, "1", 1, 0);
    int bytesSend = sendEncryptedMessage("1", client_socket, privateKey);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        exit(0);
    }
    printf("Client successfully created an account.\n");
    addLog("Client successfully created an account.\n");
    return 0;
}

int request_write(char *requestBody, int client_socket, Client **clients, Mail **mails, RSA_Key privateKey)
{
    char *idString = strtok(requestBody, "/");
    int sessionId = atoi(idString);
    char *senderAddress = strtok(NULL, "/");
    char *receiverAddress = strtok(NULL, "/");
    char *subject = strtok(NULL, "/");
    char *message = strtok(NULL, "\0");

    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, senderAddress))
        {
            if (sessionId == (*clients)[i].sessionID)
            {
                *mails = addMail(*mails, subject, message, senderAddress, receiverAddress, DEFAULT, -1);
                printf("Mail sent\n");
                addLog("Mail sent\n");
                //send(client_socket, "1", 1, 0);
                sendEncryptedMessage("1", client_socket, privateKey);
                return 1;
            }
            else
            {
                if ((*clients)[i].sessionID != 0)
                {
                    printf("Incorrect session id");
                    addLog("Incorrect session id");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                else
                {
                    printf("Client not connected\n");
                    addLog("Client not connected\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("Client not connected\n");
    addLog("Client not connected\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int deleteClientData(char *mailAddress, Client **clients, Mail **mails)
{
    for (int i = 0; i < NR_MAILS; i++)
    {
        if (!strcmp((*mails)[i].SenderAddress, mailAddress))
        {
            *mails = removeMail(*mails, (*mails)[i].MailId, ACTION_RM_SENDER);
            if (*mails == NULL)
            {
                printf("Failed to remove mail\n");
                addLog("Failed to remove mail\n");
                return 0;
            }
        }
        if (!strcmp((*mails)[i].ReceiverAddress, mailAddress))
        {
            *mails = removeMail(*mails, (*mails)[i].MailId, ACTION_RM_RECEIVER);
            if (*mails == NULL)
            {
                printf("Failed to remove mail\n");
                addLog("Failed to remove mail\n");
                return 0;
            }
        }
    }
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp(mailAddress, (*clients)[i].MailAdress))
        {
            *clients = removeClient(*clients, mailAddress);
            if (*clients == NULL)
            {
                printf("Failed to delete account\n");
                addLog("Failed to delete account\n");
                return 0;
            }
        }
    }
    return 1;
}

int request_deleteAccount(char *requestBody, int client_socket, Client **clients, Mail **mails, RSA_Key privateKey)
{
    char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *mailAddress = strtok(NULL, "\0");
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, mailAddress))
        {
            if ((*clients)[i].sessionID == sessionID)
            {
                int retCode = deleteClientData((*clients)[i].MailAdress, clients, mails);
                if (retCode == 0)
                {
                    return 0;
                }
                printf("Account deleted\n");
                addLog("Account deleted\n");
                //send(client_socket, "1", 1, 0);
                sendEncryptedMessage("1", client_socket, privateKey);
                return 1;
            }
            else
            {
                if ((*clients)[i].sessionID == 0)
                {
                    printf("Client is not connected");
                    addLog("Client is not connected");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                if ((*clients)[i].sessionID != sessionID)
                {
                    printf("Wrong session id\n");
                    addLog("Wrong session id\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("There are no clients\n");
    addLog("There are no clients\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int request_retrieveMails(char *requestBody, int client_socket, Client **clients, Mail **mails, RSA_Key privateKey)
{
    char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *mailAddress = strtok(NULL, "\0");
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, mailAddress))
        {
            if ((*clients)[i].sessionID == sessionID)
            {
                char *mailsToSend = (char *)malloc(1);
                strcpy(mailsToSend, "\0");
                for (int j = 0; j < NR_MAILS; j++)
                {
                    if (!strcmp((*mails)[j].ReceiverAddress, mailAddress) && (*mails)[j].flag != SENDER_ONLY)
                    {
                        char *idString;
                        itoa((*mails)[j].MailId, &idString);
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen(idString) + 1) * sizeof(char));
                        strcat(mailsToSend, idString);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].SenderAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].SenderAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].ReceiverAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].ReceiverAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Subject) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Subject);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Message) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Message);
                        strcat(mailsToSend, "~");
                    }
                }
                //int bytesSend = send(client_socket, mailsToSend, strlen(mailsToSend), 0);
                int bytesSend = sendEncryptedMessage(mailsToSend, client_socket, privateKey);
                if (bytesSend < 1)
                {
                    perror("error sending reply");
                    exit(0);
                }
                printf("Mails sent\n");
                addLog("Mails sent\n");
                return 1;
            }
            else
            {
                if ((*clients)[i].sessionID == 0)
                {
                    printf("Client is not connected");
                    addLog("Client is not connected");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                if ((*clients)[i].sessionID != sessionID)
                {
                    printf("Wrong session id\n");
                    addLog("Wrong session id\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("There are no clients\n");
    addLog("There are no clients\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int request_retrieveMailsSent(char *requestBody, int client_socket, Client **clients, Mail **mails, RSA_Key privateKey)
{
    char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *mailAddress = strtok(NULL, "\0");
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, mailAddress))
        {
            if ((*clients)[i].sessionID == sessionID)
            {
                char *mailsToSend = (char *)malloc(1);
                strcpy(mailsToSend, "\0");
                for (int j = 0; j < NR_MAILS; j++)
                {
                    if (!strcmp((*mails)[j].SenderAddress, mailAddress) && (*mails)[j].flag != RECEIVER_ONLY)
                    {
                        char *idString;
                        itoa((*mails)[j].MailId, &idString);
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen(idString) + 1) * sizeof(char));
                        strcat(mailsToSend, idString);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].SenderAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].SenderAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].ReceiverAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].ReceiverAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Subject) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Subject);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Message) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Message);
                        strcat(mailsToSend, "~");
                    }
                }
                //int bytesSend = send(client_socket, mailsToSend, strlen(mailsToSend), 0);
                int bytesSend = sendEncryptedMessage(mailsToSend, client_socket, privateKey);
                if (bytesSend < 1)
                {
                    perror("error sending reply");
                    exit(0);
                }
                printf("Mails sent\n");
                addLog("Mails sent\n");
                return 1;
            }
            else
            {
                if ((*clients)[i].sessionID == 0)
                {
                    printf("Client is not connected");
                    addLog("Client is not connected");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                if ((*clients)[i].sessionID != sessionID)
                {
                    printf("Wrong session id\n");
                    addLog("Wrong session id\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("There are no clients\n");
    addLog("There are no clients\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int request_retrieveAllMails(char* requestBody, int client_socket,Client** clients,Mail** mails,RSA_Key privateKey)
{   
      char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *mailAddress = strtok(NULL, "\0");
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, mailAddress))
        {
            if ((*clients)[i].sessionID == sessionID)
            {
                char *mailsToSend = (char *)malloc(1);
                strcpy(mailsToSend, "\0");
                for (int j = 0; j < NR_MAILS; j++)
                {
                    if ((!strcmp((*mails)[j].SenderAddress, mailAddress) && (*mails)[j].flag != RECEIVER_ONLY) 
                        || (!strcmp((*mails)[j].ReceiverAddress, mailAddress) && (*mails)[j].flag != SENDER_ONLY))
                    {
                        char *idString;
                        itoa((*mails)[j].MailId, &idString);
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen(idString) + 1) * sizeof(char));
                        strcat(mailsToSend, idString);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].SenderAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].SenderAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].ReceiverAddress) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].ReceiverAddress);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Subject) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Subject);
                        strcat(mailsToSend, "/");
                        mailsToSend = (char *)realloc(mailsToSend, (strlen(mailsToSend) + strlen((*mails)[j].Message) + 1) * sizeof(char));
                        strcat(mailsToSend, (*mails)[j].Message);
                        strcat(mailsToSend, "~");
                    }
                }
                //int bytesSend = send(client_socket, mailsToSend, strlen(mailsToSend), 0);
                int bytesSend = sendEncryptedMessage(mailsToSend, client_socket, privateKey);
                if (bytesSend < 1)
                {
                    perror("error sending reply");
                    exit(0);
                }
                printf("Mails sent\n");
                addLog("Mails sent\n");
                return 1;
            }
            else
            {
                if ((*clients)[i].sessionID == 0)
                {
                    printf("Client is not connected");
                    addLog("Client is not connected");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                if ((*clients)[i].sessionID != sessionID)
                {
                    printf("Wrong session id\n");
                    addLog("Wrong session id\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("There are no clients\n");
    addLog("There are no clients\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int request_deleteMail(char *requestBody, int client_socket, Client **clients, Mail **mails, RSA_Key privateKey)
{
    char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *mailAddress = strtok(NULL, "/");
    char *mailIdString = strtok(NULL, "\0");
    int mailId = atoi(mailIdString);
    for (int i = 0; i < NR_CLIENTS; i++)
    {
        if (!strcmp((*clients)[i].MailAdress, mailAddress))
        {
            if ((*clients)[i].sessionID == sessionID)
            {
                int ok = 0;
                for (int j = 0; j < NR_MAILS; j++)
                {
                    if ((*mails)[j].MailId == mailId && !strcmp((*mails)[j].SenderAddress, mailAddress))
                    {
                        *mails = removeMail(*mails, mailId, ACTION_RM_SENDER);
                        if (*mails == NULL)
                        {
                            printf("Failed to remove mail\n");
                            addLog("Failed to remove mail\n");
                            return 0;
                        }
                        ok = 1;
                    }
                    if ((*mails)[j].MailId == mailId && !strcmp((*mails)[j].ReceiverAddress, mailAddress))
                    {
                        *mails = removeMail(*mails, mailId, ACTION_RM_RECEIVER);
                        if (*mails == NULL)
                        {
                            printf("Failed to remove mail\n");
                            addLog("Failed to remove mail\n");
                            return 0;
                        }
                        ok = 1;
                    }
                }
                if (ok)
                {
                    printf("Mail deleted\n");
                    addLog("Mail deleted\n");
                    //send(client_socket, "1", 1, 0);
                    sendEncryptedMessage("1", client_socket, privateKey);
                    return 1;
                }
                else
                {
                    printf("Mail not found\n");
                    addLog("Mail not found\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
            else
            {
                if ((*clients)[i].sessionID == 0)
                {
                    printf("Client is not connected");
                    addLog("Client is not connected");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
                if ((*clients)[i].sessionID != sessionID)
                {
                    printf("Wrong session id\n");
                    addLog("Wrong session id\n");
                    //send(client_socket, "0", 1, 0);
                    sendEncryptedMessage("0", client_socket, privateKey);
                    return 0;
                }
            }
        }
    }
    printf("There are no clients\n");
    addLog("There are no clients\n");
    //send(client_socket, "0", 1, 0);
    sendEncryptedMessage("0", client_socket, privateKey);
    return 0;
}

int request_changePassword(char *requestBody,int client_socket,Client** clients,RSA_Key privateKey)
{
    char *session_string = strtok(requestBody, "/");
    int sessionID = atoi(session_string);
    char *old_password = strtok(NULL, "/");
    char *new_password = strtok(NULL, "\0");
    for(int i=0;i<NR_CLIENTS;i++)
    {
        if((*clients)[i].sessionID==sessionID)
        {
            if(!strcmp((*clients)[i].Password,old_password))
            {
                strcpy((*clients)[i].Password,new_password);
                printf("Password changed\n");
                addLog("Password changed\n");
                //send(client_socket, "1", 1, 0);
                sendEncryptedMessage("1", client_socket, privateKey);
                return 1;
            }
            else
            {
                printf("Incorrect password\n");
                addLog("Incorrect password\n");
                //send(client_socket, "0", 1, 0);
                sendEncryptedMessage("0", client_socket, privateKey);
                return 0;
            }
        }
        else
        {
            if((*clients)[i].sessionID==0)
            {
                printf("Client is not connected");
                addLog("Client is not connected");
                //send(client_socket, "0", 1, 0);
                sendEncryptedMessage("0", client_socket, privateKey);
                return 0;
            }
            if((*clients)[i].sessionID!=sessionID)
            {
                printf("Wrong session id\n");
                addLog("Wrong session id\n");
                //send(client_socket, "0", 1, 0);
                sendEncryptedMessage("0", client_socket, privateKey);
                return 0;
            }
        }
    }
}

int establishKeyChange(int client_socket, RSA_Key publicKey)
{
    char *nString;
    itoa(publicKey.n, &nString);
    char *edString;
    itoa(publicKey.ed, &edString);
    char *message = (char *)malloc((strlen(nString) + strlen(edString) + 3) * sizeof(char));
    strcpy(message, nString);
    strcat(message, " ");
    strcat(message, edString);
    int bytesSend = send(client_socket, message, strlen(message), 0);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        return 0;
    }
    strcpy(message, "\0");
    int bytesReceived = recv(client_socket, message, MAX_MESSAGE_SIZE, 0);
    message[bytesReceived] = '\0';
    if (bytesReceived < 1)
    {
        perror("error receiving reply");
        return 0;
    }
    if (message[0] == '1')
    {
        printf("Client accepted public key\n");
        addLog("Client accepted public key");
    }
    else
    {
        printf("Client rejected public key\n");
        addLog("Client rejected public key");
        return 0;
    }
    send(client_socket, "1", 1, 0);
    return 1;
}

    

int handle_client(int client_socket, Client **clients, Mail **mails)
{
    RSA_KeyPair keyPair = generateKeyPair();
    printf("clients's file descriptor: %d\n", client_socket);

    int code = establishKeyChange(client_socket, keyPair.publicKey);
    if (code == 0)
    {
        printf("Could not establish key change\n");
        addLog("Could not establish key change");
        return 0;
    }

    while (1)
    {
        char *decryptedMessage=NULL;
        int messageSize = receiveDecryptedMessage(&decryptedMessage, client_socket, keyPair.privateKey);
        if (messageSize == 0)
        {
            printf("Client disconnected\n");
            addLog("Client disconnected");
            return 0;
        }

        decryptedMessage[messageSize] = '~';

        char *requestType = strtok(decryptedMessage, "/");
        char *requestBody = strtok(NULL, "~");

        if (!strcmp(requestType, "LG"))
        {
            printf("Client requested to log in\n");
            addLog("Client requested to log in");
            request_login(requestBody, client_socket, clients, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "RG"))
        {
            printf("Client requested to register\n");
            addLog("Client requested to register");
            request_register(requestBody, client_socket, clients, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "WR"))
        {
            printf("Client requested to send a mail\n");
            addLog("Client requested to send a mail");
            request_write(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "RMR"))
        {
            printf("Client requested to receive his mails\n");
            addLog("Client requested to receive his mails");
            request_retrieveMails(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "RMS"))
        {
            printf("Client requested to receive mails sent\n");
            addLog("Client requested to receive mails sent");
            request_retrieveMailsSent(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "RMA"))
        {
            printf("Client requested to receive all mails\n");
            addLog("Client requested to receive all mails");
            request_retrieveAllMails(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "DM"))
        {
            printf("Client requested to delete a mail\n");
            addLog("Client requested to delete a mail");
            request_deleteMail(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "DA"))
        {
            printf("Client requested to delete his account\n");
            addLog("Client requested to delete his account");
            request_deleteAccount(requestBody, client_socket, clients, mails, keyPair.privateKey);
        }
        else if (!strcmp(requestType, "CP"))
        {
            printf("Client requested to change his password\n");
            addLog("Client requested to change his password");
            request_changePassword(requestBody, client_socket, clients, keyPair.privateKey);
        }
        else
        {
            printf("Client sent an unknown request");
            addLog("Client sent an unknown request");
        }
        free(decryptedMessage);
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
        write(fd, mails[i].ReceiverAddress, strlen(mails[i].ReceiverAddress));
        write(fd, " ", 1);
        write(fd, mails[i].Subject, strlen(mails[i].Subject));
        write(fd, "\n", 1);
        write(fd, mails[i].Message, strlen(mails[i].Message));
        write(fd, "~\n", 2);
        write(fd_id, &(mails[i].MailId), sizeof(int));
    }
    close(fd);
}

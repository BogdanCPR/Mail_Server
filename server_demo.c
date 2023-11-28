#include "mail_server.h"

#define THREAD_NUM 8
#define MAX_CLIENTS 256


typedef struct _task {
    int (*taskfunction)(int, Client *);
    int arg1;
    Client* arg2;
} Task;
Task clientQueue[256];
int clientCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(Task* task){
    task->taskfunction(task->arg1,task->arg2);
}

void addClientToQueue(Task client)
{
    pthread_mutex_lock(&mutexQueue);
    clientQueue[clientCount] = client;
    clientCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void *startThread(void *args)
{
    while (1)
    {
        pthread_mutex_lock(&mutexQueue);
        while (clientCount == 0)
        {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        Task client = clientQueue[0];
        int i;
        for (i = 0; i < clientCount - 1; i++)
        {
            clientQueue[i] = clientQueue[i + 1];
        }
        clientCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&client);
    }
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    pthread_t threads[THREAD_NUM];

    Client *clients = NULL;
    Mail *mails = NULL;

    // loadClients(&clients);
    // loadMails(&mails);

    // mails = addMail(mails,"Test", "Exemplu de mesaj\npentru aplicatia de server de mail\n proiect pso","capritabogdan@casin.ro","sindilarstefan@casin.ro",DEFAULT,-1);
    // mails = addMail(mails,"Test2", "Alt\nexemplu\nca sa fie.","sindilarstefan@casin.ro","capritabogdan@casin.ro",DEFAULT,-1);
    // mails = removeMail(mails, 30886,ACTION_RM_RECEIVER);
    // saveMails(mails);
    // saveClients(clients);
    // return(0);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_create(&threads[i], NULL, &startThread, NULL) != 0)
        {
            perror("Fail to create the thread");
        }
    }

    listen(server_socket, MAX_CLIENTS);
    printf("Server listening on port %d...\n", PORT);
    while (1)
    {
        // Accept connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        printf("New connection accepted, client socket file descriptor = %d\n", client_socket);
        Task t = {
            .taskfunction = &handle_client,
            .arg1 = client_socket,
            .arg2 = clients
        };
        addClientToQueue(t);
    }

    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("Fail to join the thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);
    close(server_socket);
    return 0;
}
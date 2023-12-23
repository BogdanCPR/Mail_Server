#define _POSIX_C_SOURCE 200809L
#include "mail_server.h"

#define THREAD_NUM 8
#define MAX_CLIENTS 256

typedef struct _task
{
    int (*taskfunction)(int, Client **, Mail **);
    int arg1;
    Client **arg2;
    Mail **arg3;
} Task;
Task clientQueue[256];
int clientCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

_Atomic int running = 1;
void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        running = 0;
        printf("\nSERVER CLOSING...\n");
    }
}

void executeTask(Task *task)
{
    task->taskfunction(task->arg1, task->arg2, task->arg3);
}

void addClientToQueue(Task client)
{
    pthread_mutex_lock(&mutexQueue);
    clientQueue[clientCount] = client;
    clientCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void *startThread()
{
    while (running)
    {
        pthread_mutex_lock(&mutexQueue);
        while (clientCount == 0 && running)
        {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        if (!running)
        {
            pthread_mutex_unlock(&mutexQueue);
            break;
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
    printf("Closing thread...\n");
}

int main()
{
    initialiseLog();
    srand(time(NULL));
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    pthread_t threads[THREAD_NUM];

    Client *clients = NULL;
    Mail *mails = NULL;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_RESETHAND;
    sa.sa_handler = sig_handler;
    sigaction(SIGINT, &sa, NULL);

    loadClients(&clients);
    loadMails(&mails);

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
    while (running)
    {
        // Accept connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket != -1)
        {
            printf("New connection accepted, client socket file descriptor = %d\n", client_socket);
            Task t = {
                .taskfunction = &handle_client,
                .arg1 = client_socket,
                .arg2 = &clients,
                .arg3 = &mails};
            addClientToQueue(t);
        }
    }
    printf("A iesit din while MAIN THREAD\n");

    pthread_mutex_lock(&mutexQueue);
    pthread_cond_broadcast(&condQueue);
    pthread_mutex_unlock(&mutexQueue);

    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("Fail to join the thread");
        }
        printf("THREADUL %d a dat join\n", i);
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    saveClients(clients);
    saveMails(mails);
    close(server_socket);
    return 0;
}
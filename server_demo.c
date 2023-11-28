#include "mail_server.h"



int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    Client *clients = NULL;
    Mail *mails = NULL;

    /*loadClients(&clients);
    loadMails(&mails);

    //mails = addMail(mails,"Test", "Exemplu de mesaj\npentru aplicatia de server de mail\n proiect pso","capritabogdan@casin.ro","sindilarstefan@casin.ro",DEFAULT,-1);
    //mails = addMail(mails,"Test2", "Alt\nexemplu\nca sa fie.","sindilarstefan@casin.ro","capritabogdan@casin.ro",DEFAULT,-1);
    //mails = removeMail(mails, 30886,ACTION_RM_RECEIVER);
    saveMails(mails);
    saveClients(clients);
    //return(0);*/
    

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
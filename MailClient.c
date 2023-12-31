#include "mail_server.h"

#define BUF_SIZE 100
#define BUF_MAIL_SIZE 8192
#define MY_NR_MAILS 10

int client_fd;
int session_id;
Mail mails[MY_NR_MAILS];
int my_index = 0;
RSA_Key publicKey;

void printMailPattern() 
{
    printf(
 "     ██████  █████  ███████ ██ ███    ██     ███    ███  █████  ██ ██      \n"
 "    ██      ██   ██ ██      ██ ████   ██     ████  ████ ██   ██ ██ ██      \n"
 "    ██      ███████ ███████ ██ ██ ██  ██     ██ ████ ██ ███████ ██ ██      \n"
 "    ██      ██   ██      ██ ██ ██  ██ ██     ██  ██  ██ ██   ██ ██ ██      \n"
 "     ██████ ██   ██ ███████ ██ ██   ████     ██      ██ ██   ██ ██ ███████ \n\n");
                                                                           
}

// Functie de criptare folosind XOR
char* old_encrypt(const char *data, int sessionID) 
{
    size_t len = strlen(data);
    unsigned char *sessionBytes = (unsigned char *)&sessionID;
    
    char *encryptedData = (char *)malloc(len + 1); // +1 pentru terminatorul nul

    for (size_t i = 0; i < len; ++i) 
    {
        encryptedData[i] = data[i] ^ sessionBytes[i % sizeof(int)];
    }
    encryptedData[len] = '\0'; 

    return encryptedData;
}

// Functie de decriptare folosind XOR
char* old_decrypt(const char *data, int sessionID) 
{
    size_t len = strlen(data);
    unsigned char *sessionBytes = (unsigned char *)&sessionID;

    char *decryptedData = (char *)malloc(len + 1); // +1 pentru terminatorul nul

    for (size_t i = 0; i < len; ++i) 
    {
        decryptedData[i] = data[i] ^ sessionBytes[i % sizeof(int)];
    }
    decryptedData[len] = '\0';

    return decryptedData;
}

void send_message(int cfd, char *message) 
{
    sendEncryptedMessage(message, cfd, publicKey);
}

void send_clear_message(int cfd, char* message)
{
    int bytes_sent;

    // Send the message to the server
    bytes_sent = send(cfd, message, strlen(message), 0);

    if (bytes_sent < 0) 
    {
        perror("Error sending message");
    } else {
        printf("Message sent successfully: %s\n", message);
    }
}

char* receive_response(int cfd) 
{
    char* buf = (char*)malloc(sizeof(char) * BUF_SIZE);

    if (buf == NULL) 
    {
        perror("Memory allocation error");
        return NULL;
    }

    int _lenght = receiveDecryptedMessage(&buf, cfd, publicKey);
    buf[_lenght] = '\0';

    return buf;
}

char* receive_clear_response(int cfd)
{
    char* buf = (char*)malloc(sizeof(char) * BUF_SIZE);

    if (buf == NULL) 
    {
        perror("Memory allocation error");
        return NULL;
    }

    // Primește răspunsul de la server
    int bytes_received = recv(cfd, buf, BUF_SIZE - 1, 0);

    if (bytes_received < 0) 
    {
        perror("Recv failed");
        free(buf);
        return NULL;
    } else 
    if (bytes_received == 0) 
    {
        printf("Server disconnected\n");
        free(buf);
        return NULL;
    } else 
    {
        buf[bytes_received] = '\0';
        printf("Received response from server: %s\n", buf);
        return buf;
    }
}

char* receive_long_response(int cfd) 
{
    char* buf = (char*)malloc(sizeof(char) * BUF_MAIL_SIZE);

    if (buf == NULL) 
    {
        perror("Memory allocation error");
        return NULL;
    }
    int _lenght = receiveDecryptedMessage(&buf, cfd, publicKey);
    buf[_lenght] = '\0';

    printf("Received response from server: %s\n", buf);

    return buf;
}

void set_client_fd(int fd)
{
    client_fd = fd;
}

void clear_console()
{
    #ifdef _WIN32
        system("cls");  // Pentru Windows
    #else
        system("clear");  // Pentru Linux/Unix
    #endif

    printMailPattern();
}

void displayLoadingAnimation() 
{
    int duration = 2; // Durata animației în secunde
    int frames = 4;   // Numărul de cadre în animație
    char animation[] = {'|', '/', '-', '\\'};

    for (int i = 0; i < duration; i++) {
        for (int j = 0; j < frames; j++) {
            printf("\r%c Loading... ", animation[j]);
            fflush(stdout);
            usleep(100000); // Pauza de 100ms între cadre (poate fi ajustată)
        }
    }

    printf("\rDone!        \n"); // Mesajul de final
}

void press_enter_to_continue() 
{
    printf("\nPress enter to continue...");
    fflush(stdout);
    while (getchar() != '\n');

    displayLoadingAnimation();
}

void get_user_input(char *input, int size, const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);
    
    scanf(" %[^\n]", input);    // citeste pana la \n
    getchar();
}

void get_user_input_password(char *input, int size, const char *prompt)
{
    struct termios old_term, new_term;

    // Disable terminal echo
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    printf("%s", prompt);
    fflush(stdout);

    scanf(" %[^\n]", input);    // citeste pana la \n
    getchar();

    // Enable terminal echo
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    printf("\n");
}

int login_client(char** mail, char** password)
{
    char _mail[30];
    char _password[20];
    printf("\n");

    get_user_input(_mail, sizeof(_mail), "Mail: ");
    get_user_input_password(_password, sizeof(_password), "Password: ");

    // TODO
    char msg[BUF_SIZE];
    strcpy(msg, "LG/");
    strcat(msg, _mail);
    strcat(msg, "/");
    strcat(msg, _password);

    send_message(client_fd, msg);

    char* response = receive_response(client_fd);

    if (response != NULL) 
    {
        if (response[0] != '0')
        {
            session_id = atoi(response);
            strcpy(*mail, _mail);
            strcpy(*password, _password);
            free(response);
            return 1;
        } else
        if (response[0] == '0')
        {
            free(response);
            return 0;
        }
    }

    return -1;

    /*if (strcmp(_mail, "1") == 0 && strcmp(_password, "1") == 0)
    {  
        strcpy(*mail, _mail);
        strcpy(*password, _password);
        session_id = 123;

        return 1;
    }
    else
        return 0;*/
}

int register_client()
{
    char _mail[30];
    char _password[20];
    char _passwordConfirm[20];
    printf("\n");

    get_user_input(_mail, sizeof(_mail), "Mail: ");
    get_user_input_password(_password, sizeof(_password), "Password: ");
    get_user_input_password(_passwordConfirm, sizeof(_passwordConfirm), "Confirm Password: ");
    
    if (strcmp(_password, _passwordConfirm) != 0)
    {
        printf("Passwords do not match!\n");
        return 0;
    }

    // TODO
    printf("Clientul: %s, %s \n", _mail, _password);

    // TODO
    char msg[BUF_SIZE];
    strcpy(msg, "RG/");
    strcat(msg, _mail);
    strcat(msg, "/");
    strcat(msg, _password);

    send_message(client_fd, msg);

    char* response = receive_response(client_fd);

    if (response != NULL)
    {
        if (response[0] == '1')
        {
            free(response);
            return 1;
        } else
        if (response[0] == '0')
        {
            free(response);
            return 0;
        }
    }

    return -1;
}

int show_login_register(char** mail, char** password)
{
    clear_console();

    printf("********************** Welcome to Mail Application! **********************\n");
    printf("*  1. Login                                                              *\n");
    printf("*  2. Register                                                           *\n");
    printf("*  3. Exit                                                               *\n");
    printf("**************************************************************************\n");
    printf("\nEnter your choice: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        clear_console();

        printf("================================= Login... ===============================\n");

        if (login_client(mail, password))
        {
            printf("Login Successful\n");
            displayLoadingAnimation();
            clear_console();
            return 1;
        }
        else
        {
            printf("Invalid Data\n");
            press_enter_to_continue();
            clear_console();
            return 0;
        }

        break;
    case 2:
        clear_console();

        printf("============================== Register... ===============================\n");

        if (register_client())
        {
            printf("Registration successful!\n");
            displayLoadingAnimation();
        }    
        else
        {
            printf("Registration failed. Please try again.\n");
            press_enter_to_continue();
        }
        
        break;
    case 3:
        clear_console();

        printf("================================== Exit ==================================\n");

        exit(EXIT_SUCCESS);

        break;
    default:
        printf("Invalid choice!\n");
        break;
    }

    clear_console();

    return 0;
}

void write_email(char* mail) 
{
    printf("=== Write an Email ===\n");

    Mail _newMail;
    _newMail.SenderAddress = malloc(100 * sizeof(char));
    _newMail.ReceiverAddress = malloc(100 * sizeof(char));
    _newMail.Subject = malloc(100 * sizeof(char));
    _newMail.Message = malloc(500 * sizeof(char));
    
    strcpy(_newMail.SenderAddress, mail);
    get_user_input(_newMail.ReceiverAddress, sizeof(_newMail.ReceiverAddress), "Recipient: ");
    get_user_input(_newMail.Subject, sizeof(_newMail.Subject), "Subject: ");
    get_user_input(_newMail.Message, sizeof(_newMail.Message), "Content: ");

    printf("\nEmail sent successfully:\n");
    printf("Sender: %s\n", _newMail.SenderAddress);
    printf("Recipient: %s\n", _newMail.ReceiverAddress);
    printf("Subject: %s\n", _newMail.Subject);
    printf("Content: %s\n", _newMail.Message);

    // TODO
    char _mail[8192];
    strcat(_mail, "/");
    strcat(_mail, _newMail.SenderAddress);
    strcat(_mail, "/");
    strcat(_mail, _newMail.ReceiverAddress);
    strcat(_mail, "/");
    strcat(_mail, _newMail.Subject);
    strcat(_mail, "/");
    strcat(_mail, _newMail.Message);

    char _msg[BUF_SIZE];
    snprintf(_msg, sizeof(_msg), "WR/%d/", session_id);
    //strcat(_msg, "/WR");
    strcat(_msg, _mail);

    //printf("\n%s\n", _msg);

    send_message(client_fd, _msg);

    char* response = receive_response(client_fd);

    if (response != NULL) 
    {
        if (response[0] == '1')
        {
            printf("Email Sent Successfully!\n");
        } else
        if (response[0] == '0')
        {
            printf("Error Sending Email!\n");
        }
    }
}

void get_mails(char* mail, int type)
{
    char _msg[BUF_SIZE];

    switch (type)
    {
    case 0:
        snprintf(_msg, sizeof(_msg), "RMA/%d/", session_id);     // RMA recive all mails
        strcat(_msg, mail);
        break;
    case 1:
        snprintf(_msg, sizeof(_msg), "RMS/%d/", session_id);     // RM recive send mails
        strcat(_msg, mail);
        break;
    case 2:
        snprintf(_msg, sizeof(_msg), "RMR/%d/", session_id);     // RM recive recoved mails
        strcat(_msg, mail);
        break;
    
    default:
        break;
    }

    
    send_message(client_fd, _msg);

    char* _mail = receive_long_response(client_fd);

    if (_mail != NULL)
    {
        char *_token;
        char *_str = strdup(_mail);
        _token = strtok(_str, "~");

        while (_token != NULL) 
        {
            Mail _struct_mail;
            _struct_mail.Subject = malloc(100 * sizeof(char));
            _struct_mail.Message = malloc(500 * sizeof(char));
            _struct_mail.SenderAddress = malloc(100 * sizeof(char));
            _struct_mail.ReceiverAddress = malloc(100 * sizeof(char));

            if (_struct_mail.Subject == NULL || _struct_mail.Message == NULL ||
                _struct_mail.SenderAddress == NULL || _struct_mail.ReceiverAddress == NULL) 
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }

            sscanf(_token, "%d/%99[^/]/%99[^/]/%99[^/]/%499[^~]",
                &_struct_mail.MailId, _struct_mail.SenderAddress, _struct_mail.ReceiverAddress,
                _struct_mail.Subject, _struct_mail.Message);

            if (my_index < MY_NR_MAILS)
                mails[my_index++] = _struct_mail;
            else
                printf("Error at MY_NR_MAILS\n");

            _token = strtok(NULL, "~");
        }
        
        free(_str);
    }

    free(_mail);
}

void display_mail(int MailId, char* SenderAddress, char* ReceiverAddress, char* Subject, char* Message)
{
    printf("----------------------------------------------------------------\n");
    printf("Mail ID: %d\n", MailId);
    printf("Sender Address: %s\n", SenderAddress);
    printf("Receiver Address: %s\n", ReceiverAddress);
    printf("Subject: %s\n", Subject);
    printf("Content: %s\n", Message);
    printf("----------------------------------------------------------------\n\n");
}

void display_mails(char* mail, int type)         // type: 0 -> toate mailurile | 1 -> mailurile trimise
{                                                                       // 2 -> mailurile primite
    for(int i = 0; i < my_index; i++)
    {
        Mail _mail = mails[i];

        if (type == 0)
        {
            display_mail(_mail.MailId, _mail.SenderAddress, _mail.ReceiverAddress, 
                _mail.Subject, _mail.Message);
        } else
        if (type == 1 && strcmp(_mail.SenderAddress, mail) == 0)
        {
            display_mail(_mail.MailId, _mail.SenderAddress, _mail.ReceiverAddress, 
                _mail.Subject, _mail.Message);
        } else
        if (type == 2 && strcmp(_mail.ReceiverAddress, mail) == 0)
        {
            display_mail(_mail.MailId, _mail.SenderAddress, _mail.ReceiverAddress, 
                _mail.Subject, _mail.Message);
        }
    }
}

int change_password(char* mail, char* old_password)
{
    // CP
    char currentPassword[30];
    char newPassword[30];

    get_user_input(currentPassword, sizeof(currentPassword), "Enter current password: ");

    if (strcmp(currentPassword, old_password) != 0) 
    {
        printf("Incorrect current password. Password not changed.\n");
        return 0;
    }

    get_user_input(newPassword, sizeof(newPassword), "Enter new password: ");

    // TODO
    char _msg[BUF_SIZE];
    snprintf(_msg, sizeof(_msg), "CP/%d/", session_id);     // CP/SESS_ID/old_pass/new_pass
    strcat(_msg, old_password);
    strcat(_msg, "/");
    strcat(_msg, newPassword);

    send_message(client_fd, _msg);

    char* response = receive_response(client_fd);

    if (response != NULL)
    {
        if (response[0] == '1')
        {
            free(response);
            return 1;
        } else
        if (response[0] == '0')
        {
            free(response);
            return 0;
        }
    }

    return -1;
}

int delete_acount(char* mail, char* password)
{
    printf ("Sure you want to delete the %s account?\n", mail);
    printf ("--- Yes/No ---\n");
    char input[4];

    get_user_input(input, sizeof(input), "->");

    if (strcmp(input, "Yes") == 0 || strcmp(input, "yes") == 0 || strcmp(input, "y") == 0 || 
        strcmp(input, "y") == 0)
    {
        printf("Your answer is: Yes\n");

        //TODO
        char _msg[BUF_SIZE];
        snprintf(_msg, sizeof(_msg), "DA/%d/", session_id);
        //strcat(_msg, "/DA/");
        strcat(_msg, mail);

        //printf("\n%s\n", _msg);

        send_message(client_fd, _msg);

        char* response = receive_response(client_fd);

        if (response != NULL) 
        {
            if (response[0] == '1')
            {
                printf("The account has been successfully deleted!\n");
            } else
            if (response[0] == '0')
            {
                printf("Error Deleting Account!\n");
            }
        }

        return 1;
    }
    else
    if (strcmp(input, "No") == 0 || strcmp(input, "no") == 0 || strcmp(input, "N") == 0 || 
        strcmp(input, "n") == 0)
    {
        printf("Your answer is: No\n");

        return 0;
    }

    return -1;
}

int delete_mail(char* mail)
{
    printf("Do you want to delete any mail?\n");

    printf ("--- Yes/No ---\n");
    char input[4];

    get_user_input(input, sizeof(input), "->");

    if (strcmp(input, "Yes") == 0 || strcmp(input, "yes") == 0 || strcmp(input, "y") == 0 || 
        strcmp(input, "y") == 0)
    {
        printf("Your answer is: Yes\n");

        printf("\nEnter the email id: \n");

        char _id[10];
        fflush(stdout);
        scanf("%s", _id);
        getchar();

        //TODO
        char _msg[BUF_SIZE];
        snprintf(_msg, sizeof(_msg), "DM/%d/", session_id);
        //strcat(_msg, "/DM/");
        strcat(_msg, mail);
        strcat(_msg, "/");
        strcat(_msg, _id);

        printf("\n%s\n", _msg);

        send_message(client_fd, _msg);

        char* response = receive_response(client_fd);

        if (response != NULL) 
        {
            if (response[0] == '1')
            {
                printf("The mail with the id: %s has been successfully deleted!\n", _id);
            } else
            if (response[0] == '0')
            {
                printf("Error Deleting Email!\n");
            }
        }

        return 1;
    }
    else
    if (strcmp(input, "No") == 0 || strcmp(input, "no") == 0 || strcmp(input, "N") == 0 || 
        strcmp(input, "n") == 0)
    {
        printf("Your answer is: No\n");

        return 0;
    }

    return -1;
}

int show_menu(char* mail, char* password)
{
    printf("******************************* Main Menu! *******************************\n");
    printf("*  1. View all emails                                                    *\n");
    printf("*  2. View sent emails                                                   *\n");
    printf("*  3. View received emails                                               *\n");
    printf("*  4. Write email                                                        *\n");
    printf("*  5. Change password                                                    *\n");
    printf("*  6. Delete account                                                     *\n");
    printf("*  7. Go back                                                            *\n");
    printf("**************************************************************************\n");
    printf("\nEnter your choice: ");

    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        clear_console();

        printf("Viewing all emails...\n");

        my_index = 0;
        get_mails(mail, 0);
        display_mails(mail, 0);
        delete_mail(mail);
        press_enter_to_continue();

        break;
    case 2:
        clear_console();

        printf("Viewing sent emails...\n");

        my_index = 0;
        get_mails(mail, 1);
        display_mails(mail, 1);
        delete_mail(mail);
        press_enter_to_continue();

        break;
    case 3:
        clear_console();

        printf("Viewing received emails...\n");

        my_index = 0;
        get_mails(mail, 2);
        display_mails(mail, 2);
        delete_mail(mail);
        press_enter_to_continue();

        break;
    case 4:
        clear_console();

        printf("Writing email...\n");

        write_email(mail);
        press_enter_to_continue();

        break;
    case 5:
        clear_console();

        printf("Change password...\n");

        // TODO
        if (change_password(mail, password))
        {
            printf("Password changed successfully!\n");
            displayLoadingAnimation();
        }    
        else
        {
            printf("Change password failed. Please try again.\n");
            press_enter_to_continue();
        }

        break;
    case 6:
        clear_console();

        printf("Delete account...\n");

        if (delete_acount(mail, password))
        {
            printf("\nAccount deleted successfully.\n");
            return 0;
        }
        else
            printf("\nThe account could not be deleted.\n");

        press_enter_to_continue();

        break;
    case 7:
        clear_console();

        return 0;
        break;

    default:
        printf("Invalid choice!\n");
        break;
    }

    clear_console();

    return 1;
}

int connect_to_server(char* ip)
{
    int status, cfd;
	struct sockaddr_in serv_addr;

	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

    printf("Connection Successful!\n");

    return cfd;
}

void receiveKey(int cfd)
{
    char* _msg = receive_clear_response(cfd);

    printf("MSG Rcv: %s\n", _msg);
    
    char *_token = strtok(_msg, " ");
    publicKey.n = atoi(_token);
    publicKey.ed = atoi(strtok(NULL, " "));

    send_clear_message(cfd, "1");         // ACK
    receive_clear_response(cfd);          // pt sincronizare cu serverul 1
}
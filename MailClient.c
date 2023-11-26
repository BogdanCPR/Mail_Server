#include "mail_server.h"
#include <unistd.h>


void clear_console()
{
    #ifdef _WIN32
        system("cls");  // Pentru Windows
    #else
        system("clear");  // Pentru Linux/Unix
    #endif
}

void press_enter_to_continue() 
{
    printf("\nPress enter to continue...");
    fflush(stdout);
    while (getchar() != '\n');
}

void get_user_input(char *input, int size, const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);
    
    scanf(" %[^\n]", input);    // citeste pana la \n
    getchar();
}

int login_client(char** mail, char** password)
{
    char _mail[30];
    char _password[20];
    printf("\n");

    get_user_input(_mail, sizeof(_mail), "Mail: ");
    get_user_input(_password, sizeof(_password), "Password: ");

    if (strcmp(_mail, "1") == 0 && strcmp(_password, "1") == 0)
    {  
        strcpy(*mail, _mail);
        strcpy(*password, _password);

        return 1;
    }
    else
        return 0;
}

void register_client()
{
    char _mail[30];
    char _password[20];
    printf("\n");

    get_user_input(_mail, sizeof(_mail), "Mail: ");
    get_user_input(_password, sizeof(_password), "Password: ");

    // TODO
    printf("Clientul: %s, %s \n", _mail, _password);

}

int show_login_register(char** mail, char** password)
{
    clear_console();

    printf("********** Welcome to Mail Application! **********\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        clear_console();

        printf("========== Login... ==========\n");

        if (login_client(mail, password))
        {
            printf("Login Successful\n");
            sleep(1);
            clear_console();
            return 1;
        }
        else
        {
            printf("Invalid Data\n");
            sleep(1);
            clear_console();
            return 0;
        }

        break;
    case 2:
        clear_console();

        printf("========== Register... ==========\n");

        register_client();
        sleep(1);

        break;
    case 3:
        clear_console();

        printf("========== Exit ==========\n");
        exit(EXIT_SUCCESS);

        break;
    default:
        printf("Invalid choice!\n");
        break;
    }

    clear_console();

    return 0;
}

void write_email() 
{
    char recipient[100];
    char subject[100];
    char content[500];

    printf("=== Write an Email ===\n");

    get_user_input(recipient, sizeof(recipient), "Recipient: ");
    get_user_input(subject, sizeof(subject), "Subject: ");
    get_user_input(content, sizeof(content), "Content: ");

    printf("\nEmail sent successfully:\n");
    printf("Recipient: %s\n", recipient);
    printf("Subject: %s\n", subject);
    printf("Content: %s\n", content);
}

void display_all_mails(const char *mails) 
{
    char *token;
    char *str = strdup(mails);
    token = strtok(str, "~");

    while (token != NULL) {
        Mail mail;

        mail.Subject = malloc(100 * sizeof(char));
        mail.Message = malloc(500 * sizeof(char));
        mail.SenderAddress = malloc(100 * sizeof(char));
        mail.ReceiverAddress = malloc(100 * sizeof(char));

        if (mail.Subject == NULL || mail.Message == NULL ||
            mail.SenderAddress == NULL || mail.ReceiverAddress == NULL) 
        {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        sscanf(token, "%d/%99[^/]/%99[^/]/%99[^/]/%499[^~]",
               &mail.MailId, mail.SenderAddress, mail.ReceiverAddress,
               mail.Subject, mail.Message);

        printf("------------------------------------------------------\n");
        printf("Mail ID: %d\n", mail.MailId);
        printf("Sender Address: %s\n", mail.SenderAddress);
        printf("Receiver Address: %s\n", mail.ReceiverAddress);
        printf("Subject: %s\n", mail.Subject);
        printf("Content: %s\n", mail.Message);
        printf("------------------------------------------------------\n\n");

        token = strtok(NULL, "~");
    }

    free(str);
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

        return 1;
    }
    else
    if (strcmp(input, "No") == 0 || strcmp(input, "no") == 0 || strcmp(input, "N") == 0 || 
        strcmp(input, "n") == 0)
    {
        printf("Your answer is: No\n");
        //TODO

        return 0;
    }
}

int delete_mail()
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

        int _id;
        fflush(stdout);
        scanf(" %d", &_id);
        getchar();

        //TODO

        printf("The mail with the id: %d has been successfully deleted!\n", _id);

        return 1;
    }
    else
    if (strcmp(input, "No") == 0 || strcmp(input, "no") == 0 || strcmp(input, "N") == 0 || 
        strcmp(input, "n") == 0)
    {
        printf("Your answer is: No\n");
        //TODO

        return 0;
    }

}

int show_menu(char* mail, char* password)
{
    printf("********** Main Menu **********\n");
    printf("1. View emails\n");
    printf("2. Write email\n");
    printf("3. Delete account\n");
    printf("4. Go back\n");
    printf("Enter your choice: ");

    char* _mail;
    char* _password;
    strcpy(_mail, mail);
    strcpy(_password, _password);

    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
        clear_console();

        printf("Viewing emails...\n");

        const char *mails = "1/sender1@example.com/receiver1@example.com/Subject 1/Content 1~2/sender2@example.com/receiver2@example.com/Subject 2/Content 2";
        display_all_mails(mails);
        delete_mail();
        press_enter_to_continue();

        break;
    case 2:
        clear_console();

        printf("Writing email...\n");

        write_email();
        press_enter_to_continue();

        break;
    case 3:
        clear_console();

        printf("Delete account...\n");

        if (delete_acount(_mail, _password))
        {
            printf("\nAccount deleted successfully.\n");
            return 0;
        }
        else
            printf("\nThe account could not be deleted.\n");

        press_enter_to_continue();

        break;
    case 4:
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

#include "mail_server.h"

RSA_key_t mod_pow(RSA_key_t base, RSA_key_t exponent, RSA_key_t modulus)
{
    RSA_key_t result = 1;
    base = base % modulus;

    while (exponent > 0)
    {
        if ((exponent & 1) == 1)
        {
            result = (result * base) % modulus;
        }

        base = (base * base) % modulus;
        exponent >>= 1;
    }
    return result;
}

RSA_key_t encrypt(RSA_key_t message, RSA_Key key)
{
    return mod_pow(message, key.ed, key.n);
}

RSA_key_t decrypt(RSA_key_t encryptedMessage, RSA_Key key)
{
    return mod_pow(encryptedMessage, key.ed, key.n);
}

int isPrime(RSA_key_t n)
{
    if (n <= 1)
        return 0;
    if (n <= 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;
    RSA_key_t i = 5;
    while (i * i <= n)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
        i += 6;
    }
    return 1;
}

RSA_key_t findNextPrime(RSA_key_t n)
{
    if (n <= 1)
        return 2;
    RSA_key_t prime = n;
    int found = 0;
    while (!found)
    {
        prime++;
        if (isPrime(prime))
            found = 1;
    }
    return prime;
}

RSA_key_t gcd(RSA_key_t a, RSA_key_t b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

RSA_key_t modInverse(RSA_key_t a, RSA_key_t m)
{
    RSA_key_t m0 = m, t, q;
    RSA_key_t x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

RSA_KeyPair generateKeyPair()
{
    RSA_KeyPair RSA_KeyPair;
    RSA_key_t p = 0, q = 0, n = 0, phi = 0, e = 0, d = 0;
    do
    {
        while (p == q)
        {
            p = rand() % 9000 + 1000;
            q = rand() % 9000 + 1000;
        }
        p = findNextPrime(p);
        q = findNextPrime(q);
        n = p * q;
        phi = (p - 1) * (q - 1);
        e = rand() % phi;
        while (gcd(e, phi) != 1)
        {
            e = rand() % phi;
        }
        d = modInverse(e, phi);

    } while (e * d % phi != 1);

    if (e * d % phi != 1)
    {
        printf("ERROR: %llu %llu %llu\n", e, d, phi);
        exit(0);
    }
    RSA_KeyPair.publicKey.n = n;
    RSA_KeyPair.publicKey.ed = e;
    RSA_KeyPair.privateKey.n = n;
    RSA_KeyPair.privateKey.ed = d;
    return RSA_KeyPair;
}


RSA_key_t* encryptMessage(char* message, RSA_Key key)
{
    RSA_key_t* encryptedMessage = (RSA_key_t*)malloc(sizeof(RSA_key_t) * strlen(message));
    int i;
    for (i = 0; i < strlen(message); i++)
    {
        encryptedMessage[i] = encrypt(message[i], key);
    }
    return encryptedMessage;
}

char* decryptMessage(RSA_key_t* encryptedMessage, RSA_key_t size, RSA_Key key)
{
    char* message = (char*)malloc(sizeof(char) * size);
    int i;
    for (i = 0; i < size; i++)
    {
        message[i] = decrypt(encryptedMessage[i], key);
    }
    return message;
}

//////////////////USEFULL FUNCTIONS//////////////////////////

int receiveDecryptedMessage(char **decryptedMessage, int socket,RSA_Key Key)
{
    // receive message size in clear
    char message[MAX_MESSAGE_SIZE];
    strcpy(message, "");

    int bytesReceived = recv(socket, message, MAX_MESSAGE_SIZE, 0);
    if (bytesReceived < 1)
    {
        perror("error receiving reply");
        return 0;
    }
    message[bytesReceived] = '\0';
    // send ack
    int bytesSend = send(socket, "1", 1, 0);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        return 0;
    }
    // receive message
    int messageSize = atoi(message);
    strcpy(message, "");
    char *encryptedMessageString = (char *)malloc(messageSize * sizeof(RSA_key_t));
    bytesReceived = recv(socket, encryptedMessageString, messageSize * sizeof(RSA_key_t), 0);
    if (bytesReceived < 1)
    {
        perror("error receiving reply");
        return 0;
    }
    RSA_key_t* encryptedMessage = (RSA_key_t*)encryptedMessageString;
    // decrypt the message
    *decryptedMessage = decryptMessage(encryptedMessage, messageSize, Key);
    return messageSize;
}

int sendEncryptedMessage(char *message, int socket, RSA_Key Key)
{
    // encrypt the message
    RSA_key_t *encryptedMessage = encryptMessage(message, Key);
    // send message size in clear
    char *messageSizeString;
    itoa(strlen(message), &messageSizeString);
    int bytesSend = send(socket, messageSizeString, strlen(messageSizeString), 0);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        return 0;
    }
    // receive ack
    char *ack = (char *)malloc(1);
    int bytesReceived = recv(socket, ack, 1, 0);
    if (bytesReceived < 1)
    {
        perror("error receiving reply");
        return 0;
    }
    if(strcmp(ack,"1"))
    {
        printf("Client did not receive message size\n");
        return 0;
    }
    // send message
    bytesSend = send(socket, encryptedMessage, strlen(message)*sizeof(RSA_key_t), 0);
    if (bytesSend < 1)
    {
        perror("error sending reply");
        return 0;
    }
    return 1;
}
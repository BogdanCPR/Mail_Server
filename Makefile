all : Server_Demo Client_Demo
.PHONY: clean

MailServer.o : MailServer.c
	gcc -c MailServer.c -o MailServer.o

MailClient.o : MailClient.c 
	gcc -c MailClient.c -o MailClient.o

MailServer_shared.o : MailServer.c
	gcc -fPIC -c MailServer.c -o MailServer_shared.o

MailClient_shared.o : MailClient.c 
	gcc -fPIC -c MailClient.c -o MailClient_shared.o

lib_mail.so : MailServer_shared.o MailClient_shared.o
	gcc -shared MailServer_shared.o MailClient_shared.o -o lib_mail.so

Server_Demo : server_demo.c lib_mail.so
	gcc server_demo.c -o Server_Demo lib_mail.so -L . -Wl,-rpath=.

Client_Demo : client_demo.c lib_mail.so
	gcc client_demo.c -o Client_Demo lib_mail.so -L . -Wl,-rpath=.

clean:
	rm *.o *.so
all : Server_Demo Client_Demo

.PHONY: clean resetDB

MailServer_shared.o : MailServer.c
	gcc -fPIC -c MailServer.c -o MailServer_shared.o

MailClient_shared.o : MailClient.c 
	gcc -fPIC -c MailClient.c -o MailClient_shared.o

RSA_shared.o : RSA.c
	gcc -fPIC -c RSA.c -o RSA_shared.o

lib_mail.so : MailServer_shared.o MailClient_shared.o RSA_shared.o
	gcc -shared MailServer_shared.o MailClient_shared.o RSA_shared.o -o lib_mail.so

Server_Demo : server_demo.c lib_mail.so
	gcc -g server_demo.c -o Server_Demo lib_mail.so -L . -Wl,-rpath=.

Client_Demo : client_demo.c lib_mail.so
	gcc -g client_demo.c -o Client_Demo lib_mail.so -L . -Wl,-rpath=.

clean:
	rm *.o *.so Server_Demo Client_Demo

resetDB: 
	rm *.db
	touch clients.db mails.db ids.db


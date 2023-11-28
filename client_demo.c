#include "mail_server.h"

int main(int argc, char *argv[])
{
	int client_fd = connect_to_server("127.0.0.1");
	
	press_enter_to_continue();

	if (client_fd != -1)
	{

		set_client_fd(client_fd);
		// AICI POTI TRIMITE COMENZI///

		// MENU ----------------------------------------------------------------
		clear_console();

		while (1)
		{
			char *_mail = (char *)malloc(sizeof(char) * 30);
			char *_password = (char *)malloc(sizeof(char) * 20);

			if (show_login_register(&_mail, &_password))
			{
				int aux = 1;
				while (aux)
				{
					aux = show_menu(_mail, _password);
				}
			}
		}
		// ---------------------------------------------------------------------

		// closing the connected socket
		close(client_fd);
	}

	return 0;
}
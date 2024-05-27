#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include <string.h>

int turn = 0;

int main(int arc, char *argv[])
{
	int client_socket, server_socket;
	sockaddr_in server_address, client_address;
	struct hostent *h;
	
	h = gethostbyname(argv[1]);
	server_address.sin_family = h->h_addrtype;
	memcpy((char *) &server_address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	server_address.sin_port = htons(8080);
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		std::cerr << "Error: Nie udało się utworzyć gniazda!\n";
		exit(0);
	}
	
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = htonl(INADDR_ANY);
	client_address.sin_port = htons(0);
	
	server_socket = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if (server_socket == -1)
	{
		std::cerr << "Error: Nie udało się nawiązać połączenia z serwerem!\n";
		exit(0);
	}

	// KOMUNIKACJA
	char buffer[4096];
	while (true)
	{
		if(turn == 0)
		{
			// NADAWANIE DO SERWERA
			std::string user_input;
			std::cout << "> ";
			std::getline(std::cin, user_input);
			if (user_input == "exit")
			{
				std::cout << "Klient kończy połączenie!" << "\n";
				send(client_socket, user_input.c_str(), user_input.size() + 1, 0);
				close(client_socket);
				break;
			}
			else if(user_input == "1" || user_input == "2" || user_input == "3" || user_input == "4" || user_input == "5" || user_input == "6" || user_input == "7" || user_input == "8" || user_input == "9")
			{
				int send_result = send(client_socket, user_input.c_str(), user_input.size() + 1, 0);
				if (send_result == -1)
				{
					std::cerr << "Error: Błąd podczas nadawania!\n";
					break;
				}
				turn = 1;
			}
			else
			{
				std::cout << "Invalid input! Please enter a number from 1-9 range or exit to terminate game." << "\n";
			}
		}
		if(turn == 1)
		{
			// ODBIERANIE OD SERWERA
			memset(buffer, '\0', 4096);
			int bytes_received = recv(client_socket, buffer, 4096, 0);
			if (bytes_received == -1)
			{
				std::cerr << "Error: Błąd podczas odbierania!\n";
				break;
			}
			std::cout << "Server: " << std::string(buffer, bytes_received) << "\n";
	
			// SPRAWDZENIE, CZY SERWER SIĘ ROZŁĄCZA
			if (std::string(buffer) == "You won! The game will now quit." || std::string(buffer) == "You lost! The game will now quit.")
			{
				close(client_socket);
				break;
			}
			turn = 0;
		}
	}

	return 0;
}

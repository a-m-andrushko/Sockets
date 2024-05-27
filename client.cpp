#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int turn = 0;

int main(int arc, char *argv[])
{
	// UTWORZENIE GNIAZDA
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		std::cerr << "Error: Nie udało się utworzyć gniazda!\n";
		exit(0);
	}

	// BINDING GNIAZDA DO PORTU IP SERWERA
	sockaddr_in server_address;
	//server_address.sin_family = AF_INET;
	
	struct hostent *h;
	h = gethostbyname(argv[1]);
	server_address.sin_family = h->h_addrtype;
	
	server_address.sin_port = htons(8080);   // JAK W SERWERZE
	server_address.sin_addr.s_addr = INADDR_ANY;
	if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
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
			if (user_input == "exit_client")
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
				turn = 0;
			}
			else
			{
				std::cout << "Invalid input! Please enter a number from 1-9 range or exit_client to terminate game." << "\n";
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
			if (std::string(buffer) == "exit_server")
			{
				std::cout << "Serwer się rozłączył!\n";
				close(client_socket);
				break;
			}
			turn = 0;
		}
	}

	return 0;
}

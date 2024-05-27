#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int turn = 0;
int gameState = 0; 
int grid[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // SIATKA GRY

std::string dataRead(int data) // ZAMIANA int NA string
{
	switch(data)
	{
		case -1: {return "X";}
		case 1: {return "O";}
		default: {return " ";}
	}
}

void printGrid() // WYŚWIETLENIE GRY
{
	std::string sGrid = "\n "+dataRead(grid[0][0])+"|"+dataRead(grid[1][0])+"|"+dataRead(grid[2][0])+" \n — — — \n "+dataRead(grid[0][1])+"|"+dataRead(grid[1][1])+"|"+dataRead(grid[2][1])+" \n — — — \n "+dataRead(grid[0][2])+"|"+dataRead(grid[1][2])+"|"+dataRead(grid[2][2])+" \n";
	std::cout << sGrid;
}

void sendGrid(int client_socket) // PRZESŁANIE GRY
{
	std::string sGrid = "\n "+dataRead(grid[0][0])+"|"+dataRead(grid[1][0])+"|"+dataRead(grid[2][0])+" \n — — — \n "+dataRead(grid[0][1])+"|"+dataRead(grid[1][1])+"|"+dataRead(grid[2][1])+" \n — — — \n "+dataRead(grid[0][2])+"|"+dataRead(grid[1][2])+"|"+dataRead(grid[2][2])+" \n";
	send(client_socket, sGrid.c_str(), sGrid.size() + 1, 0);
}

void clearGrid()
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			grid[i][j] = 0;
		}
	}
}

void checkEnd()
{
	int tmp = 0;
	for(int i = 0; i < 3; i++)
	{
		if(std::abs(grid[i][0] + grid[i][1] + grid[i][2]) == 3)
		{
			tmp = grid[i][0];
			break;
		}
		else if(std::abs(grid[0][i] + grid[1][i] + grid[2][i]) == 3)
		{
			tmp = grid[0][i];
			break;
		}
	}
	if(tmp == 0)
	{
		if(std::abs(grid[0][0] + grid[1][1] + grid[2][2]) == 3 || std::abs(grid[0][2] + grid[1][1] + grid[2][0]) == 3)
		{
			tmp = grid[1][1];
		}
	}
	
	gameState = tmp;
}

int main(int arc, char *argv[])
{
	// UTWORZENIE GNIAZDA
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);   // socket(x, SOCK_STREAM, x) -- SYGNALIZUJE TCP
	if (server_socket == -1)
	{
		std::cerr << "Error: Nie udało się utworzyć gniazda!\n";
		exit(0);
	}

	// BINDING GNIAZDA DO PORTU IP
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8080);   // DOWOLNY SWOBODNY PORT
	server_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Error: Nie udało się powiązać gniazda do portu IP!\n";
		exit(0);
	}

	// NASŁUCHIWANIE
	if (listen(server_socket, SOMAXCONN) == -1)
	{
		std::cerr << "Error: Nasłuchiwanie nie powiodło się!\n";
		exit(0);
	}

	// POŁĄCZENIE
	sockaddr_in client_address;
	socklen_t client_size = sizeof(client_address);
	int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
	if (client_socket == -1)
	{
		std::cerr << "Error: Połączenie nie zostało nawiązane!\n";
		exit(0);
	}

	// KOMUNIKACJA
	char buffer[4096];
	while (true)
	{
		if(turn == 0)
		{
			// ODBIERANIE OD KLIENTA
			memset(buffer, '\0', 4096);
			int bytes_received = recv(client_socket, buffer, 4096, 0);
			if (bytes_received == -1) // SPRAWDZENIE, CZY WIADOMOŚĆ OD KLIENTA ZOSTAŁA ODEBRANA
			{
				std::cerr << "Error: Błąd podczas odbierania!\n";
				break;
			}
			else if (bytes_received == 0) // SPRAWDZENIE, CZY KLIENT NADAJE ODPOWIEDŹ
			{
				std::cout << "Klient został odłączony!\n";
			}
			else if (std::string(buffer) == "exit_client") // SPRAWDZENIE, CZY KLIENT SIĘ ROZŁĄCZA
			{
				std::cout << "Klient się rozłączył!\n";
				close(client_socket);
				break;
			}
			else
			{
				std::cout << "Client: " << std::string(buffer, bytes_received) << "\n";
				if(std::string(buffer) == "1"){grid[0][0] = -1; printGrid();}
				else if(std::string(buffer) == "2"){grid[1][0] = -1; printGrid();}
				else if(std::string(buffer) == "3"){grid[2][0] = -1; printGrid();}
				else if(std::string(buffer) == "4"){grid[0][1] = -1; printGrid();}
				else if(std::string(buffer) == "5"){grid[1][1] = -1; printGrid();}
				else if(std::string(buffer) == "6"){grid[2][1] = -1; printGrid();}
				else if(std::string(buffer) == "7"){grid[0][2] = -1; printGrid();}
				else if(std::string(buffer) == "8"){grid[1][2] = -1; printGrid();}
				else if(std::string(buffer) == "9"){grid[2][2] = -1; printGrid();}
				
				checkEnd();
				if(gameState == -1)
				{
					std::cout<<"You lost! The game will now reset."<<"\n";
					std::string msg = "You won! The game will now reset.";
					send(client_socket, msg.c_str(), msg.size() + 1, 0);
					clearGrid();
					gameState = 0;
				}
				
				turn = 1;
			}
		}
		if(turn == 1)
		{
			// NADAWANIE DO KLIENTA
			std::string user_input;
			std::cout << "> ";
			std::getline(std::cin, user_input);
			if (user_input == "exit_server")
			{
				std::cout << "Serwer kończy połączenie!" << "\n";
				send(server_socket, user_input.c_str(), user_input.size() + 1, 0);
				close(client_socket);
				close(server_socket);
				break;
			}
			else if(user_input == "1" || user_input == "2" || user_input == "3" || user_input == "4" || user_input == "5" || user_input == "6" || user_input == "7" || user_input == "8" || user_input == "9")
			{
				int send_result;
				if(user_input == "1"){grid[0][0] = 1; sendGrid(client_socket);}
				else if(user_input == "2"){grid[1][0] = 1; sendGrid(client_socket);}
				else if(user_input == "3"){grid[2][0] = 1; sendGrid(client_socket);}
				else if(user_input == "4"){grid[0][1] = 1; sendGrid(client_socket);}
				else if(user_input == "5"){grid[1][1] = 1; sendGrid(client_socket);}
				else if(user_input == "6"){grid[2][1] = 1; sendGrid(client_socket);}
				else if(user_input == "7"){grid[0][2] = 1; sendGrid(client_socket);}
				else if(user_input == "8"){grid[1][2] = 1; sendGrid(client_socket);}
				else if(user_input == "9"){grid[2][2] = 1; sendGrid(client_socket);}
				else {send_result = send(client_socket, user_input.c_str(), user_input.size() + 1, 0);}
				
				if (send_result == -1)
				{
					std::cerr << "Error: Błąd podczas nadawania!\n";
					break;
				}
				
				checkEnd();
				if(gameState == 1)
				{
					std::cout<<"You won! The game will now reset."<<"\n";
					std::string msg = "You lost! The game will now reset.";
					send(client_socket, msg.c_str(), msg.size() + 1, 0);
					clearGrid();
					gameState = 0;
				}
				
				turn = 0;
			}
			else
			{
				std::cout << "Invalid input! Please enter a number from 1-9 range or exit_server to terminate game." << "\n";
			}
		}
	}

	// ZAMKNIĘCIE GNIAZDA KLIENTA
	if (client_socket != -1)
	{
		close(client_socket);
	}

	return 0;
}

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

std::string readGrid() // ZAMIANA int[][] NA string
{
	std::string sGrid = "\n "+dataRead(grid[0][0])+"|"+dataRead(grid[1][0])+"|"+dataRead(grid[2][0])+" \n — — — \n "+dataRead(grid[0][1])+"|"+dataRead(grid[1][1])+"|"+dataRead(grid[2][1])+" \n — — — \n "+dataRead(grid[0][2])+"|"+dataRead(grid[1][2])+"|"+dataRead(grid[2][2])+" \n";
	return sGrid;
}

void sendGrid(int client_socket) // PRZESŁANIE GRY
{
	std::string sGrid = readGrid();
	send(client_socket, sGrid.c_str(), sGrid.size() + 1, 0);
}

void clearGrid() // RESET GRY
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			grid[i][j] = 0;
		}
	}
}

void checkOverlap(int tile) // SPRAWDZENIE NADPISYWANIA
{
	if(turn == 0)
	{
		switch(tile)
		{
			case 1: {if(grid[0][0] == 0){grid[0][0] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 2: {if(grid[1][0] == 0){grid[1][0] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 3: {if(grid[2][0] == 0){grid[2][0] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 4: {if(grid[0][1] == 0){grid[0][1] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 5: {if(grid[1][1] == 0){grid[1][1] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 6: {if(grid[2][1] == 0){grid[2][1] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 7: {if(grid[0][2] == 0){grid[0][2] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 8: {if(grid[1][2] == 0){grid[1][2] = -1; gameState = 0; break;} else {gameState = 2; break;}}
			case 9: {if(grid[2][2] == 0){grid[2][2] = -1; gameState = 0; break;} else {gameState = 2; break;}}
		}
	}
	else if(turn == 1)
	{
		switch(tile)
		{
			case 1: {if(grid[0][0] == 0){grid[0][0] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 2: {if(grid[1][0] == 0){grid[1][0] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 3: {if(grid[2][0] == 0){grid[2][0] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 4: {if(grid[0][1] == 0){grid[0][1] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 5: {if(grid[1][1] == 0){grid[1][1] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 6: {if(grid[2][1] == 0){grid[2][1] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 7: {if(grid[0][2] == 0){grid[0][2] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 8: {if(grid[1][2] == 0){grid[1][2] = 1; gameState = 0; break;} else {gameState = 2; break;}}
			case 9: {if(grid[2][2] == 0){grid[2][2] = 1; gameState = 0; break;} else {gameState = 2; break;}}
		}
	}
}

void checkEnd() // SPRAWDZENIE WYGRANEJ GRY
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
	if(tmp == 0)
	{
		int full = 0;
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				if(grid[i][j] == 0) {break;}
				else {full++;}
			}
		}
		if(full == 9) {gameState = -2; return;}
	}
	
	gameState = tmp;
}

int main(int argc, char *argv[])
{
	// UTWORZENIE GNIAZDA
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);   // socket(x, SOCK_STREAM, x) -- SYGNALIZUJE TCP
	if (server_socket == -1)
	{
		std::cerr << "Error: Socket could not be created!\n";
		exit(0);
	}

	// BINDING GNIAZDA DO PORTU IP
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(8080);   // DOWOLNY SWOBODNY PORT	
	if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cerr << "Error: Socket could not be binded to IP port!\n";
		exit(0);
	}

	// NASŁUCHIWANIE
	if (listen(server_socket, 1) == -1)
	{
		std::cerr << "Error: Listening failed!\n";
		exit(0);
	}

	int client_socket;
	while (true)
	{
		// POŁĄCZENIE
		sockaddr_in client_address;
		socklen_t client_size = sizeof(client_address);
		client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
	
		if (client_socket != -1)
		{	
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
						std::cerr << "Error: Receiving failure!\n";
						break;
					}
					else if (bytes_received == 0) // SPRAWDZENIE, CZY KLIENT NADAJE ODPOWIEDŹ
					{
						std::cout << "No response from client!\n";
						break;
					}
					else if (std::string(buffer) == "exit") // SPRAWDZENIE, CZY KLIENT SIĘ ROZŁĄCZA
					{
						std::cout << "Client disconnected!\n";
						clearGrid();
						gameState = 0;
						close(client_socket);
						break;
					}
					else
					{
						std::cout << "Client: " << std::string(buffer, bytes_received) << "\n";
						if(std::string(buffer) == "1"){checkOverlap(1);}
						else if(std::string(buffer) == "2"){checkOverlap(2);}
						else if(std::string(buffer) == "3"){checkOverlap(3);}
						else if(std::string(buffer) == "4"){checkOverlap(4);}
						else if(std::string(buffer) == "5"){checkOverlap(5);}
						else if(std::string(buffer) == "6"){checkOverlap(6);}
						else if(std::string(buffer) == "7"){checkOverlap(7);}
						else if(std::string(buffer) == "8"){checkOverlap(8);}
						else if(std::string(buffer) == "9"){checkOverlap(9);}
						
						if(gameState == 2)
						{
							std::string msg = "Invalid move! Please, choose an empty tile."+readGrid();
							send(client_socket, msg.c_str(), msg.size() + 1, 0);
						}
						else
						{
							checkEnd();
							if(gameState == -1)
							{
								std::cout << readGrid()+"You lost! The client will now quit.\n"+"Waiting for the next game.\n";
								std::string msg = "You won! The game will now quit.";
								send(client_socket, msg.c_str(), msg.size() + 1, 0);
								clearGrid();
								gameState = 0;
								break;
							}
							else if(gameState == -2)
							{
								std::cout << readGrid()+"It's a tie! The client will now quit.\n"+"Waiting for the next game.\n";
								std::string msg = "It's a tie! The game will now quit.";
								send(client_socket, msg.c_str(), msg.size() + 1, 0);
								clearGrid();
								gameState = 0;
								break;
							}
							else
							{
								std::cout << readGrid();
								turn = 1;
							}
						}
					}
				}

				if(turn == 1)
				{
					// NADAWANIE DO KLIENTA
					std::string user_input;
					std::cout << "> ";
					std::getline(std::cin, user_input);
					if(user_input == "1" || user_input == "2" || user_input == "3" || user_input == "4" || user_input == "5" || user_input == "6" || user_input == "7" || user_input == "8" || user_input == "9")
					{
						int send_result;
						if(user_input == "1"){checkOverlap(1);}
						else if(user_input == "2"){checkOverlap(2);}
						else if(user_input == "3"){checkOverlap(3);}
						else if(user_input == "4"){checkOverlap(4);}
						else if(user_input == "5"){checkOverlap(5);}
						else if(user_input == "6"){checkOverlap(6);}
						else if(user_input == "7"){checkOverlap(7);}
						else if(user_input == "8"){checkOverlap(8);}
						else if(user_input == "9"){checkOverlap(9);}
						
						if(gameState == 2)
						{
							std::cout<<"Invalid move! Please, choose an empty tile."+readGrid();
						}
						else
						{
							if (send_result == -1)
							{
								std::cerr << "Error: Sending failure!\n";
								break;
							}
							checkEnd();
							if(gameState == 1)
							{
								std::cout<< readGrid()+"You won! The client will now quit.\n"+"Waiting for the next game.\n";
								std::string msg = "You lost! The game will now quit.";
								send(client_socket, msg.c_str(), msg.size() + 1, 0);
								clearGrid();
								gameState = 0;
								turn = 0;
								break;
							}
							else if(gameState == -2)
							{
								std::cout << readGrid()+"It's a tie! The client will now quit.\n"+"Waiting for the next game.\n";
								std::string msg = "It's a tie! The game will now quit.";
								send(client_socket, msg.c_str(), msg.size() + 1, 0);
								clearGrid();
								gameState = 0;
								turn = 0;
								break;
							}
							else
							{
								sendGrid(client_socket);
								turn = 0;
							}
						}
					}
					else
					{
						std::cout << "Invalid input! Please, enter a number from 1-9 or exit to terminate the game.\n";
					}
				}
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
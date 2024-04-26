#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
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
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8880);   // JAK W SERWERZE
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

        int send_result = send(client_socket, user_input.c_str(), user_input.size() + 1, 0);
        if (send_result == -1)
        {
            std::cerr << "Error: Błąd podczas nadawania!\n";
            break;
        }

        // ODBIERANIE OD SERWERA
        memset(buffer, 0, 4096);
        int bytes_received = recv(client_socket, buffer, 4096, 0);
        if (bytes_received == -1)
        {
            std::cerr << "Error: Błąd podczas odbierania!\n";
            break;
        }
        std::cout << "Server: " << std::string(buffer, bytes_received) << "\n";

        if (std::string(buffer, bytes_received) == "exit_server")
        {
            close(client_socket);
        }
    }

    // ZAMKNIĘCIE GNIAZDA KLIENTA
    if (client_socket != -1)
    {
        close(client_socket);
    }

    return 0;
}
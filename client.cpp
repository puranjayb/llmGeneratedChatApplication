#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <stdexcept>

#define PORT 8080

int main(){
    int clientSocket = 0;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0}; // Allocate memory for incoming message

    try {
        // Create a socket file descriptor
        if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw std::runtime_error("Client Socket Creation Failed");
        }

        serverAddr.sin_family = AF_INET; // IPv4
        serverAddr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
            throw std::runtime_error("Invalid Address/Address Not Supported");
        }

        // Connect to the server
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            throw std::runtime_error("Connection Failed");
        }

        while (true) {
            std::cout << "You: ";
            std::cin.getline(buffer, 1024); // Read message from user

            if (strlen(buffer) == 0) {
                std::cerr << "Empty message. Please enter a valid message." << std::endl;
                continue;
            }

            send(clientSocket, buffer, strlen(buffer), 0); // Send message to server

            memset(buffer, 0, sizeof(buffer)); // Clear the buffer
            int valRead = read(clientSocket, buffer, 1024); // Read incoming message from server
            if (valRead > 0) {
                std::cout << "Server: " << buffer << std::endl;
            } else {
                throw std::runtime_error("Read Error or Empty Response from Server");
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown Error occurred in client.cpp" << std::endl;
    }

    close(clientSocket);

    return 0;
}

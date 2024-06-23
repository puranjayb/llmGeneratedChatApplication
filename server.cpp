#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>

#define PORT 8080
#define LLM_PORT 9090

std::string getLLMResponse(const std::string &message){
    int llmSocket;
    struct sockaddr_in llmAddr;
    char buffer[1024] = {0};

    try {
        if((llmSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            throw std::runtime_error("LLM Socket Creation Failed");
        }

        llmAddr.sin_family = AF_INET;
        llmAddr.sin_port = htons(LLM_PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &llmAddr.sin_addr) <= 0) {
            throw std::runtime_error("Invalid Address/Address Not Supported for LLM");
        }

        if (connect(llmSocket, (struct sockaddr *)&llmAddr, sizeof(llmAddr)) < 0) {
            throw std::runtime_error("LLM Connection Failed");
        }

        send(llmSocket, message.c_str(), message.length(), 0);
        int valRead = read(llmSocket, buffer, 1024);
        close(llmSocket);

        if(valRead > 0){
            return std::string(buffer);
        } else {
            throw std::runtime_error("LLM Response Error");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return "Error";
    } catch (...) {
        std::cerr << "Unknown Error occurred in getLLMResponse" << std::endl;
        return "Error";
    }
}

int main(){
    int serverFileDescriptor, serverSocket;
    struct sockaddr_in serverAddr;

    int opt = 1;
    int addrLen = sizeof(serverAddr);
    char buffer[1024] = {0};

    try {
        if((serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            throw std::runtime_error("Server Socket Creation Failed");
        }

        if(setsockopt(serverFileDescriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            throw std::runtime_error("Socket Option Failed");
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if(bind(serverFileDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
            throw std::runtime_error("Bind to sockaddr Failed");
        }

        if(listen(serverFileDescriptor, 1) < 0){
            throw std::runtime_error("Cannot Listen");
        }

        std::cout << "Server now listening on port " << PORT << std::endl;

        if((serverSocket = accept(serverFileDescriptor, (struct sockaddr *)&serverAddr, (socklen_t *)&addrLen)) < 0 ){
            throw std::runtime_error("Failed to Accept");
        }

        while(true){
            memset(buffer, 0, sizeof(buffer));
            int valRead = read(serverSocket, buffer, 1024);

            if(valRead > 0){
                std::cout << "Client: " << buffer << std::endl;
            }

            std::string response = getLLMResponse(buffer);
            send(serverSocket, response.c_str(), response.length(), 0);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown Error occurred in server.cpp" << std::endl;
    }

    close(serverSocket);
    close(serverFileDescriptor);

    return 0;
}

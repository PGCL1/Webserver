/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 16:41:24 by glacroix          #+#    #+#             */
/*   Updated: 2024/10/04 16:21:15 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdio>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <fcntl.h>          // For non-blocking mode
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>     // For select()
#include <arpa/inet.h>
#include <vector>
#include <set>

#define PORT 3333
#define BACKLOG 10

#if 0

std::string getContentType(const std::string &path)
{
    if (path.rfind(".html"))
        return "text/html";
    else if (path.rfind(".css"))
        return "text/css";
    else if (path.rfind(".js"))
        return "application/javascript";
    else
        return "text/plain"; // Default
}

void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("Error getting socket flags");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("Error setting socket to non-blocking");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Allow reuse of the address
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    server_addr.sin_port = htons(PORT);       // Use port 3333

    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Error listening");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Set the server socket to non-blocking mode
    setNonBlocking(server_fd);

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    // Set of connected clients
    std::set<ClientInfo> clients;

    fd_set readfds;
    int max_sd, client_fd, activity;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sockets[FD_SETSIZE]; // FD_SETSIZE is the maximum number of file descriptors select() can handle

    for (int i = 0; i < FD_SETSIZE; i++) {
        client_sockets[i] = -1;
    }

    while (true) {
        FD_ZERO(&readfds);

        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_sd) {
                    max_sd = client_sockets[i];
                }
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Error in select()");
            continue;
        }

        // Check if the activity is on the server socket (new connection)
        if (FD_ISSET(server_fd, &readfds)) {
            client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd < 0) {
                perror("Error accepting new connection");
                continue;
            }

            // Get the client's IP address and port
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(client_addr.sin_port);

            ClientInfo new_client = {std::string(client_ip), client_port};

            // Check if the client is new
            if (clients.find(new_client) == clients.end()) {
                std::cout << "New client added: IP = " << client_ip << ", Port = " << client_port << std::endl;
                clients.insert(new_client); // Add to the set of known clients

                setNonBlocking(client_fd);

                for (int i = 0; i < FD_SETSIZE; i++) {
                    if (client_sockets[i] == -1) {
                        client_sockets[i] = client_fd;
                        break;
                    }
                }
            } else {
                std::cout << "Client already connected, ignoring connection from IP = " << client_ip << ", Port = " << client_port << std::endl;
                close(client_fd);
            }
        }

        // Check for I/O on client sockets
        for (int i = 0; i < FD_SETSIZE; i++) {
            client_fd = client_sockets[i];

            if (client_fd > 0 && FD_ISSET(client_fd, &readfds)) {
                char buffer[1024];
                int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received <= 0) {
                    // Connection closed or error
                    std::cout << "Client " << client_fd << " disconnected" << std::endl;
                    close(client_fd);
                    client_sockets[i] = -1; // Mark slot as empty
                } else {
                    // Null-terminate the buffer and print the received message
                    buffer[bytes_received] = '\0';
                    std::cout << "Received message: " << buffer << std::endl;

                    // Echo the message back to the client
                    send(client_fd, buffer, bytes_received, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}


#else
std::string readFileContent(std::string path)
{
    std::stringstream ss;
    std::ifstream file(path);
    if (file.is_open())
    {
        ss << file.rdbuf();
    }
    file.close();
    return ss.str();
}

void respondClient(int client_fd, std::vector<char>& dynamic_buffer)
{
    dynamic_buffer.push_back('\0'); 
    std::cout << "Received request:\n" << dynamic_buffer.data() << std::endl;
    
    // Prepare an HTTP response
    std::stringstream ss;
    ss << dynamic_buffer.data();

    std::string line;
    std::getline(ss, line);
    std::stringstream ss2(line);

    std::string path;
    ss2 >> path;
    ss2 >> path;

    std::cout << "`" << path << "` | size = " << path.size()<< std::endl;
    if (path.size() == 1) 
        path = "/index.html";

    std::string new_path =  "." + path;
    std::cout << "new_path = " << new_path << std::endl;


    std::string html = readFileContent(new_path);

    std::stringstream convert;
    convert << html.size();
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + convert.str() + "\r\n\r\n" + html;

    // Send the response to the client
    send(client_fd, response.c_str(), response.size(), 0);
}

int main() {
    // Create a TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Allow reuse of the address
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
    server_addr.sin_port = htons(3333);       // Use port 3333

    // Bind the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Error listening");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port 3333..." << std::endl;

    std::vector<char> dynamic_buffer;
    int client_fd;
    const size_t buffer_increment = 1024;
    ssize_t bytes_received;
    while (true) 
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) 
        {
            perror("Error accepting connection");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "Client " << client_fd << " connected" << std::endl;
        char temp_buffer[buffer_increment];  
        bytes_received = recv(client_fd, temp_buffer, sizeof(temp_buffer), 0);

        if (bytes_received < 0) 
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                break;
            else
            {
                perror("Error receiving data");
                close(client_fd);
                close(server_fd);
                exit(EXIT_FAILURE);
            }
        }
        /*
        else if (bytes_received == 0)
        {
            std::cout << "Client " << client_fd << " disconnected." << std::endl;
            break;
        }*/
        else 
        {
            // Append the received data to the dynamic buffer
            dynamic_buffer.insert(dynamic_buffer.end(), temp_buffer, temp_buffer + bytes_received);
            respondClient(client_fd, dynamic_buffer);
        }
    }


    // Close remaining fds;
    close(server_fd);

    std::cout << "Response sent and connection closed." << std::endl;

    return 0;
}
#endif

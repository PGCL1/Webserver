/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 16:56:24 by glacroix          #+#    #+#             */
/*   Updated: 2024/10/04 19:04:43 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <map>    // To track clients
#include <arpa/inet.h>
#include <vector> // To handle dynamic data
#include <sstream>
#include <fstream>

#define MAX_EVENTS 10
#define PORT 3333

// Client structure to store additional information about each client
struct ClientInfo {
    int fd;                 // Client's socket file descriptor
    std::vector<char> buffer; // Buffer to store data from the client
};

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

// Set the socket to non-blocking mode
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

void respondClient(int event_fd, char *buffer)
{
    
    std::cout << "Received request:\n" << buffer << std::endl;
    
    // Prepare an HTTP response
    std::stringstream ss;
    ss << buffer;

    std::string line;
    std::getline(ss, line);
    std::stringstream ss2(line);

    std::string path;
    ss2 >> path;
    ss2 >> path;

    std::cout << "\n`" << path << "` | size = " << path.size() << "\n"<< std::endl;
    if (path.size() == 1) 
        path = "/index.html";

    std::string new_path =  "." + path;
    std::cout << "new_path = " << new_path << std::endl;
    std::string contentType = getContentType(new_path);
    std::cout << "content-type: " << contentType << std::endl;

    std::string file = readFileContent(new_path);
    std::stringstream convert;
    convert << file.size();
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\n"
        "Content-Length: " + convert.str() + "\r\n\r\n" + file ;

    // Send the response to the event
    send(event_fd, response.c_str(), response.size(), 0);
    close(event_fd);
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

    //setNonBlocking(server_fd);

    // Create an epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("Error creating epoll instance");
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the epoll instance
    struct epoll_event event;
    event.events = EPOLLIN; // Monitor for incoming connections
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        perror("Error adding server socket to epoll instance");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Data structure to track connected clients by their file descriptor
    std::map<int, ClientInfo> clients;

    // Array to hold events from epoll
    struct epoll_event events[MAX_EVENTS];

    char buffer[1024];
    int bytes_received;
    int flag = 0;
    while (true)
    {
        // Wait for events on the monitored file descriptors
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds < 0) {
            perror("Error in epoll_wait");
            exit(EXIT_FAILURE);
        }

        // Loop through triggered events
        for (int i = 0; i < num_fds; i++)
        {
            int event_fd = events[i].data.fd;
            if (event_fd == server_fd)
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd < 0)
                {
                    perror("Error accepting new connection");
                    continue;
                }
                // Set the client socket to non-blocking mode
               // setNonBlocking(client_fd);

                // Add the client socket to the epoll instance
                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLOUT; // Monitor for incoming data from the client
                client_event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0) {
                    perror("Error adding client socket to epoll instance");
                    close(client_fd);
                    continue;
                }

                // Store the new client's information
                ClientInfo client_info;
                client_info.fd = client_fd;
                clients[client_fd] = client_info;

                std::cout << "New client connected: " << client_fd << std::endl;

            }
            else
            {
                if (events[i].data.fd & EPOLLIN)
                {
                    //EPOLLIN | EPOLLOUT
                    // Data available on a client socket
                    bytes_received = recv(event_fd, buffer, sizeof(buffer), 0);

                    if (bytes_received < 0)
                    {
                        perror("Error reading from client");
                        close(event_fd);
                        clients.erase(event_fd);
                        continue;
                    }
                    else if (bytes_received == 0)
                    {
                        // Client disconnected
                        std::cout << "Client disconnected: " << event_fd << std::endl;
                        close(event_fd);
                        clients.erase(event_fd);
                        continue;
                    }
                    std::cout << "Test:" << buffer << std::endl;
                    clients[event_fd].buffer.insert(clients[event_fd].buffer.end(), buffer, buffer + bytes_received);
                    //need to find out the content length of the body of the client request. 
                    //first look for \r\n\r\n to find the start of the body then check content-length header for the size of the body
                    /*if (clients[event_fd].buffer.size() == ContentLength)
                    {
                        clients[event_fd].buffer[bytes_received] = '\0';
                        flag = 1;
                        continue;
                    }*/
                }

                else if (flag == 1 && events[i].data.fd & EPOLLOUT) 
                {
                    respondClient(event_fd, clients[event_fd].buffer.data()); 
                }
            }
        }
    }

    // Close the server socket
    close(server_fd);
    return 0;
}


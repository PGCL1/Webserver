/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simple_server.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 16:41:24 by glacroix          #+#    #+#             */
/*   Updated: 2024/10/02 17:53:58 by glacroix         ###   ########.fr       */
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

    // Accept an incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Error accepting connection");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Buffer to hold incoming request data
    for (size_t i = 0; i < 3; i += 1)
    {
        char buffer[1024];
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("Error receiving data");
            close(client_fd);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Null-terminate the received data and print the request
        buffer[bytes_received] = '\0';
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

        std::string new_path =  "." + path;
        std::cout << "new_path = " << new_path << std::endl;


        std::string html = readFileContent(new_path);
        //"<html></html>\n<head></head><link rel=\"stylesheet\" href=\"style.css\">\n<body><h1>Hello</h1>\n<script src=\"index.js\"></script></body>";

        std::stringstream convert;
        convert << html.size();
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + convert.str() + "\r\n\r\n" + html;

        // Send the response to the client
        send(client_fd, response.c_str(), response.size(), 0);
    }

    // Close the connection
    close(client_fd);
    close(server_fd);

    std::cout << "Response sent and connection closed." << std::endl;

    return 0;
}


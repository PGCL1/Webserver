/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 15:36:28 by glacroix          #+#    #+#             */
/*   Updated: 2024/09/05 13:37:22 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * getaddrinfo();
 * socket();
 * bind();
 * listen();
 * accept()*/ 

#define MYPORT "6666"
#define BACKLOG 10

int main(int argc, char **argv)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int status, err;
    int sockfd, newfd;

    //load up address struct with getaddrinfo()
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //fill in my IP for me

    status = getaddrinfo("localhost", MYPORT, &hints, &res);
    if (status != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return 2;
    }

    //make a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    std::cout << "sockfd: " << sockfd << std::endl;

    //bind it to the port we passed in to getaddrinfo()
    err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (err != 0)
        std::cerr << "bind: " << gai_strerror(err) << std::endl;
    
    //connect to a remote host
/*  err = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (err == -1)
        std::cerr << "Error: " << strerror(errno) << std::endl; 
*/

    err = listen(sockfd, BACKLOG);
    if (err == -1)
        std::cerr << "listen: " << gai_strerror(errno) << std::endl; 

    addr_size = sizeof(their_addr);
    newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (newfd == -1)
        std::cerr << "accept: " << gai_strerror(errno) << std::endl;
    std::cout << "newfd: " << newfd << std::endl;

    void *buf;
    err = recv(sockfd, buf, 1000, 0);
    std::cout << "Text: " << (char *)buf << std::endl;

    close(sockfd);
    freeaddrinfo(res);
    return 0;
}

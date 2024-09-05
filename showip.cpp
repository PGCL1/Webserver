/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   showip.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 15:36:28 by glacroix          #+#    #+#             */
/*   Updated: 2024/09/04 17:38:31 by glacroix         ###   ########.fr       */
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

int main(int argc, char **argv)
{
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        std::cerr << "usage: showip hostname" << std::endl;
        return 1;
    }
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(argv[1], NULL, &hints, &res);
    if (status != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return 2;
    }
    std::cout << "\nIP addresses for: " << argv[1] << "\n" << std::endl;
    for (p = res; p != NULL; p = p->ai_next)
    {
        void  *addr;
        std::string ipver;

        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(ipv4->sin_addr);
        ipver = "IPv4";

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        std::cout << ipver << ": " << ipstr << std::endl;
    }
    freeaddrinfo(res);
    return 0;
}

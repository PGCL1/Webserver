HTTP provides a uniform interface for interacting with a resource -- regardless of its type, nature, or implementation -- by sending messages that manipulate or transfer representations.

Each message is either a request or a response.
A client constructs request messages that communicate its intentions and routes those messages toward an identified origin server.
A server listens for requests, parses each message received, interprets the message semantics in relation to the identified target resource, and responds to that request with one or more response messages.
The client examines received responses to see if its intentions were carried out, determining what to do next based on the status codes and content received.

HTTP messages sent from web clients to web servers are called request messages.
Messages from servers to clients are called response messages. There are no other
kinds of HTTP messages. The formats of HTTP request and response messages are
very similar.

A socket is a way to speak to other programs using standard Unix fds. There are two main types of sockets (not true, there's a bunch more, like Raw Sockets):
    - Stream Sockets (SOCK_STREAM) are reliable two-way connected communication streams, the data that is inputted in one end, comes out the other end error-free and in the same order. They use a protocol called TCP(Transmission Control Protocol) to maintain a high level of data transmission.
    - Datagram Sockets (SOCK_DGRAM) don't need to maintain an open connection, they just send out packets to a specific IP, no connection needed. They are used in cases where speed is paramount and loosing a few packets doesn't change much the experience. They make use of UDP(User Datagram Protocol) protcol

Example:
If you’re sending chat messages, TCP is great; if you’re sending 40 positional updates per second of the players in the world, maybe it doesn’t matter so much if one or two get dropped, and UDP is a good choice

literatures:
Online -- 
* [Guide to Network Programming](https://beej.us/guide/bgnet/html/#audience)
* [RFC HTTP Server](https://datatracker.ietf.org/doc/html/rfc2616#autoid-1)
* [HTTP Semantics](https://httpwg.org/specs/rfc9110.html)
* [Mozilla - What is a webser](https://developer.mozilla.org/en-US/docs/Learn/Common_questions/Web_mechanics/What_is_a_web_server)

Books --
HTTP The Definitive Guide
HTTP Pocket Reference


allowed functions:
- known functions
    execve, dup, dup2, pipe, strerror,errno, dup, dup2, fork, fcntl, close, read, write, waitpid, kill,
    open, opendir, readdir, closedir

- new functions 
    socketpair

       int socketpair(int domain, int type, int protocol, int sv[2]);
       The  socketpair()  call creates an unnamed pair of connected sockets in the specified domain, of the specified
       type, and using the optionally specified protocol.  For further details of these arguments, see socket(2).

       The file descriptors used in referencing the new sockets are returned in sv[0] and sv[1].  The two sockets are
       indistinguishable.

       On success, zero is returned.  On error, -1 is returned, errno is set appropriately, and sv is left unchanged
    htons

       uint16_t htons(uint16_t hostshort);
       The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    htonl

        uint32_t htonl(uint32_t hostlong);
        The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
    ntohs

       uint16_t ntohs(uint16_t netshort);
       The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.
    ntohl

       uint32_t ntohl(uint32_t netlong);
       The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
    select
    
       int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
       select() allows a program to monitor multiple file descriptors, waiting
       until one or more of the file descriptors become "ready" for some class
       of I/O operation (e.g., input possible).  A file descriptor is  consid‐
       ered  ready  if it is possible to perform a corresponding I/O operation
       (e.g., read(2), or a sufficiently small write(2)) without blocking.

       select() can monitor only file descriptors numbers that are  less  than
       FD_SETSIZE; 
    poll

        int poll(struct pollfd *fds, nfds_t nfds, int timeout)
        poll()  performs a similar task to select(2): it waits for one of a set of file descriptors to
        become ready to perform I/O.
    epoll (epoll_create, epoll_ctl, epoll_wait)
        
        The following system calls are provided to create and manage  an  epoll instance:

       • epoll_create(2)  creates  a new epoll instance and returns a file de‐
         scriptor referring to that instance.   (The  more  recent  epoll_cre‐
         ate1(2) extends the functionality of epoll_create(2).)

       • Interest  in  particular  file  descriptors  is  then  registered via
         epoll_ctl(2), which adds items to the interest list of the epoll  in‐
         stance.

       • epoll_wait(2) waits for I/O events, blocking the calling thread if no
         events are currently available.  (This system call can be thought  of
         as fetching items from the ready list of the epoll instance.)

        The epoll API performs a similar task to poll(2): monitoring multiple file descriptors to see if I/O is 
        possible on any of them. The epoll API can be used either as an edge-triggered or a level-triggered interface 
        and scales well to large numbers of watched file descriptors.
        
        The central concept of the epoll API is the epooll instance, an in-kernel data structure which, from a
        user-space perspective, can be considered as a container for two lists:
            - The interest list (sometimes also called the epoll set): the set of file descriptors that the process 
            has registered an interest in monitoring
            - The ready list: the set of file descriptors that are "ready" for I/O. The ready list is a subset of 
            (or, more precisely, a set of references to) the file descriptors in the interest list. The ready list
            is dynamically popullated by the kernel as a result of I/O activity on those file descriptors.
    kqueue (kqueue, kevent)

        
    socket

        int socket(int domain, int type, int protocol);
        
        socket() creates an endpoint for communication and returns a file descriptor that refers to that endpoint.
        The file descriptor returned by a successfull call willl be the lowest-numbered file descriptor 
        not currently open for the process.

        The domain argument specifies a communication domain; this selects the protocol family which will be used
        for communication. check domains formats in socket(2);
    accept (read further...)
        
        int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    
        The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET). It extracts
        the first connection request on the queue of pending connections for the listening socket, sockfd, creates a
        connected socket, and returns a new file descriptor reffering to that socket. The newly created socket is not
        in the listening state. The original socket sockfd is unaffected by this call.
        The argument sockfd is a socket created with socket(2) boundd to a local address with bind(2), and is listening
        for connections after a listen(2);
    listen

        int listen(int sockfd, int backlog);

        listen()  marks  the  socket referred to by sockfd as a passive socket,
        that is, as a socket that will be used to  accept  incoming  connection
        requests using accept(2).

        The  sockfd  argument  is  a file descriptor that refers to a socket of
        type SOCK_STREAM or SOCK_SEQPACKET.

        The backlog argument defines the maximum length to which the  queue  of
        pending  connections  for sockfd may grow.  If a connection request ar‐
        rives when the queue is full, the client may receive an error  with  an
        indication  of ECONNREFUSED or, if the underlying protocol supports re‐
        transmission, the request may be ignored so that a later  reattempt  at
        connection succeeds.

        On  success,  zero is returned.  On error, -1 is returned, and errno is
        set appropriately.

    send

        ssize_t send(int sockfd, const void* buf, size_t len, int flags);

        The send call may be used only when the socket is in a connected state (so that the intented recipient is known).
        The only different between send and write is the presence of flags. With a zero flags, argument, send is equivalent to write.
        The argument sockfd is the file descriptor of the send socket
    recv
    chdir
    bind
    connect
    getaddrinfo
    freeaddrinfo
    setsockopt
    getsockname
    getprotobyname
    signal
    access
    stat
    gai_strerror
    
libraries needed:

    #include <sys/types.h>          /* See NOTES */
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    #include <poll.h>
    #include <sys/epoll.h>





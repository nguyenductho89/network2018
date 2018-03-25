//
//  main.c
//  MultiplexingServer09
//
//  Created by Nguyễn Đức Thọ on 3/24/18.
//  Copyright © 2018 Nguyễn Đức Thọ. All rights reserved.
//

//
//  main.c
//  Server08
//
//  Created by Nguyễn Đức Thọ on 3/24/18.
//  Copyright © 2018 Nguyễn Đức Thọ. All rights reserved.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int createSocketAtPort(unsigned short port){
    int sockfd;
    struct sockaddr_in saddr;
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
        return 0;
    }
    //setsockopt() - reuse address/
    setsockopt(sockfd, SOL_SOCKET,
               SO_REUSEADDR, &(int){ 1 },
               sizeof(int));
    //fcntl() - nonblocking
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    if ((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)) {
        printf("Error binding\n");
        return 0;
    }
    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
        return 0;
    }
    return sockfd;
}
void *input(int *clientfds){
    char message[256];
    ssize_t messageSize;
    scanf("%s",message) ;
    for (int i = 0; i < 100; i++) {
        if (clientfds[i] > 0) {
            messageSize = send(clientfds[i], message, strlen(message), 0);
            printf("############# Server send to the number %d client: %s\n",i,message);
        }
    }
    return 0;
}

int main()
{
    
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in caddr;
    
    char message[256];
    //Create socket at port
    sockfd = createSocketAtPort(8784);
    
    //Clients Array
    int clientfds[100]; // list of connected clients, >0 if valid
    //Create a new thread for input
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, input, clientfds);
    memset(clientfds, 0, sizeof(clientfds));
    while(1){
        fd_set set; // declaration of the set
        FD_ZERO(&set); // clear the set
        FD_SET(sockfd, &set); // add listening sockfd to set
        int maxfd = sockfd; // a required value to pass to select()
        for (int i = 0; i < 100; i++) {
            // add connected client sockets to set
            if (clientfds[i] > 0) FD_SET(clientfds[i], &set);
            if (clientfds[i] > maxfd) maxfd = clientfds[i];
        }
        // poll and wait, blocked indefinitely
        select(maxfd+1, &set, NULL, NULL, NULL);
        // a «listening» socket?
        if (FD_ISSET(sockfd, &set)) {
            clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);
            // make it nonblocking
            int fl = fcntl(clientfd, F_GETFL, 0);
            fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);
            // add it to the clientfds array
            for (int i = 0; i < 100; i++) {
                if (clientfds[i] == 0) {
                    clientfds[i] = clientfd;
                    break;
                }
            }
        }
        
        for (int i = 0; i < 100; i++) {
            if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set)) {
                if (read(clientfds[i], message, sizeof(message)) > 0) {
                    printf("client %d says: %s\nserver>", clientfds[i], message);
                }
                else {
                    // some error. remove it from the "active" fd array
                    printf("client %d has disconnected.\n", clientfds[i]);
                    clientfds[i] = 0;
                }
            }
        }
    }
    return 0;
}




//
//  main.c
//  Server
//
//  Created by nguyen.duc.tho on 3/26/18.
//  Copyright © 2018 nguyen.duc.tho. All rights reserved.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define ServerPort          8784
#define NumberOfClients        4
int clientList[NumberOfClients] ;
/* Open socket at port */
int createSocket() {
    int socketFileDescriptor;
    if ((socketFileDescriptor=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
        return -1;
    }
    /* Reuse address */
    setsockopt(socketFileDescriptor, SOL_SOCKET,
               SO_REUSEADDR, &(int){ 1 },
               sizeof(int));
    /* Nonblocking */
    int socketFlag = fcntl(socketFileDescriptor, F_GETFL, 0);
    socketFlag |= O_NONBLOCK;
    fcntl(socketFileDescriptor, F_SETFL, socketFlag);
    return socketFileDescriptor;
}
/* Server address setup */
struct sockaddr_in createSocketAddress(int port){
    struct sockaddr_in serverSocketAddress;
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverSocketAddress.sin_port = htons(port);
    return serverSocketAddress;
}

void *input(int *clientfds){
    char message[256];
    ssize_t messageSize;
    scanf("%s",message) ;
    for (int i = 0; i < NumberOfClients; i++) {
        if (clientfds[i] > 0) {
            messageSize = send(clientfds[i], message, strlen(message), 0);
            printf("############# Server send to the number %d client: %s\n",i,message);
        }
    }
    return 0;
}
int acceptAnListeningClientAtSocket(int socketDescription){
    socklen_t socketlen;
    struct sockaddr_in socketAdress;
    int clientFileDescription = accept(socketDescription, (struct sockaddr *) &socketAdress, &socketlen);
    int socketFlag = fcntl(clientFileDescription, F_GETFL, 0);
    socketFlag = fcntl(clientFileDescription, F_GETFL, 0);
    socketFlag |= O_NONBLOCK;
    fcntl(clientFileDescription, F_SETFL, socketFlag);
    return clientFileDescription;
}
int addAnAcceptedClientToClientList(int client){
    for (int i = 0; i < NumberOfClients; i++) {
        if (clientList[i] == 0) {
            clientList[i] = client;
        }
    }
    return 0;
}

int main(){
    /* Create socket*/
    int socketDescription = createSocket();
    /* Create socket address */
    struct sockaddr_in socketAdress = createSocketAddress(ServerPort);
    /* Binding and listening to clients */
    if ((bind(socketDescription, (struct sockaddr *) &socketAdress, sizeof(socketAdress)) < 0)) {
        printf("Error binding\n");
        return -1;
    }
    if (listen(socketDescription, 5) < 0) {
        printf("Error listening\n");
        return -1;
    }
    /* Initialize list of clients */
    memset(clientList, 0, sizeof(clientList));
    /* Create a new thread for input */
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, input, &clientList);
    
    /**/
    char message[10];
    /*Create clients list*/
    fd_set fileDescriptionList;
    FD_ZERO(&fileDescriptionList);
    //Add socket to client list
    FD_SET(socketDescription, &fileDescriptionList);
    int maxFileDescriptor = socketDescription;
    
    for (;;) {
        // a required value to pass to select()
        for (int i = 0; i < NumberOfClients; i++) {
            // add connected client sockets to set
            if (clientList[i] > 0) {
               // printf("client %d",i);
                FD_SET(clientList[i], &fileDescriptionList);
            }
            if (clientList[i] > maxFileDescriptor) {
                maxFileDescriptor = clientList[i];
            }
        }
        // poll and wait, blocked indefinitely
        select(maxFileDescriptor+1, &fileDescriptionList, NULL, NULL, NULL);

        if (FD_ISSET(socketDescription, &fileDescriptionList)) {
            socklen_t socketlen;
            struct sockaddr_in socketAdress;
            int clientfd = accept(socketDescription, (struct sockaddr *) &socketAdress, &socketlen);
            // make it nonblocking
            int fl = fcntl(clientfd, F_GETFL, 0);
            fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);
            // add it to the clientfds array
            for (int i = 0; i < 100; i++) {
                if (clientList[i] == 0) {
                    clientList[i] = clientfd;
                    break;
                }
            }
//            int acceptedClient = acceptAnListeningClientAtSocket(socketDescription);
//             addAnAcceptedClientToClientList(acceptedClient);
//            printf("Client %d connected!\n",acceptedClient-socketDescription-1);
        }
        
        /* Read received message */
        for (int i = 0; i < NumberOfClients; i++) {
            if (clientList[i] > 0 && FD_ISSET(clientList[i], &fileDescriptionList)) {
                if (read(clientList[i], message, sizeof(message)) > 0) {
                    printf("client %d says: %s\nserver>\n", i, message);
                    memset(message, 0, sizeof(message));
                }
                else {
                    // some error. remove it from the "active" fd array
                    printf("client %d has disconnected.\n",i);
                    clientList[i] = 0;
                }
            }
        }
    }
}

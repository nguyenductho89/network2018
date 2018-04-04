//
//  main.c
//  Client
//
//  Created by nguyen.duc.tho on 3/26/18.
//  Copyright © 2018 nguyen.duc.tho. All rights reserved.
//
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#define ServerHostName     ((char *)"localhost")
#define ServerPort          8784
/* Open socket at port */
int createSocketAt(int port) {
    int socketFileDescriptor;
    /* Create tcp stream socket */
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketFileDescriptor == -1 ) {
        perror("Init soket fail");
        return -1;
    }
    return socketFileDescriptor;
}
/* Get server info from host name */
struct hostent *getServerInfo(char* hostName){
    struct hostent* host;
    host = gethostbyname(hostName);
    if (host == NULL) {
        perror("Server host not found!!!\n");
        return NULL;
    }
    if (host->h_addr_list[0] == NULL) {
        perror("Server host not found!!!\n");
        return NULL;
    }
    return host;
}
/* Server address setup */
struct sockaddr_in registServerSocketAdress(int port, char*hostName){
    struct sockaddr_in serverSocketAddress;
    memset(&serverSocketAddress, 0, sizeof(struct sockaddr_in));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(port);
    struct hostent *server = getServerInfo(hostName);
    memcpy((char *) &serverSocketAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    return serverSocketAddress;
}
/* Main*/
int main( int argc, char *argv[] ){
    //Create client socket
    int clientSocketFileDescriptor = createSocketAt(ServerPort);
    //Regist server socket
    struct sockaddr_in serverSocket = registServerSocketAdress(ServerPort, ServerHostName);
    //Connect client and server
    if (connect(clientSocketFileDescriptor, (struct sockaddr *)&serverSocket, sizeof(serverSocket))) {
        /*Connect fail*/
        perror("Could not establish connection between client and server");
        return -1;
    }
    /*Connect success*/
    printf("Connected\n");
    //Address reusable
    setsockopt(clientSocketFileDescriptor, SOL_SOCKET,
               SO_REUSEADDR, &(int){ 1 },
               sizeof(int));
    //Socket non blocking
    int fl = fcntl(clientSocketFileDescriptor, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(clientSocketFileDescriptor, F_SETFL, fl);
    
    /* Start conversation*/
    ssize_t messageSize;
    char message[256];
    for (;;) {
        printf("Type message to send to server\n");
        scanf("%s",message) ;
        if((messageSize = send(clientSocketFileDescriptor, message, strlen(message), 0)) >0){
        printf("@@@@@@@@@@@@ Me: %s\n",message);
    }
//        //Receive message
//        if ((messageSize = recv(clientSocketFileDescriptor, message, sizeof(message), 0))>0) {
//        printf("############# Server: %s\n",message);
//        }
    }
    return 0;
}


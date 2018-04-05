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
#define MessageSize         10
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
    char message[MessageSize];
    for (;;) {
//        printf(">: ");
//        bzero(message,MessageSize);
//        fgets(message,MessageSize-1,stdin);
//        if((messageSize = send(clientSocketFileDescriptor, message, strlen(message), 0)) >0){
//            printf("@@@@@@@@@@@@ Me: %s\n",message);
//        }
        
        //Receive message
        if ((messageSize = recv(clientSocketFileDescriptor, message, sizeof(message), 0))>0) {
            printf("############# Server: %s\n",message);
        }
    }
    return 0;
}



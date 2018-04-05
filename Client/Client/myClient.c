//
//  myClient.c
//  Client
//
//  Created by Nguyễn Đức Thọ on 4/5/18.
//  Copyright © 2018 nguyen.duc.tho. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
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
    struct hostent *server = getServerInfo(hostName);
    struct sockaddr_in serverSocketAddress;
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(port);
    memcpy((char *) &serverSocketAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    return serverSocketAddress;
}

void *stdInput(void *sockfd)
{
    int fd = *((int *)sockfd);
    char msg[50];
    while (1)
    {
        printf("Client: ");
        scanf("%s", msg);
        if (fd > 0)
        {
            send(fd, msg, strlen(msg), 0);
        }
    }
}

int main(int argc, char **argv)
{
    
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
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, stdInput, &clientSocketFileDescriptor);
    
    char msg[50];
    while (1)
    {
        while (recv(clientSocketFileDescriptor, msg, sizeof(msg), 0) > 0)
        {
            printf("Server: %s\n", msg);
        }
        memset(&msg, 0, sizeof(msg));
    }

    return 0;
}




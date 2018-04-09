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
#define ServerPort          8989
#define MessageSize         256
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
/* Address reuseable and non-blocking socket*/
void configSocketMultiplex(void *clientSocketFileDescriptor) {
    int fd = *((int *)clientSocketFileDescriptor);
    //Address reusable
    setsockopt(fd, SOL_SOCKET,
               SO_REUSEADDR, &(int){ 1 },
               sizeof(int));
    //Socket non blocking
    int clientSocketFlag = fcntl(fd, F_GETFL, 0);
    clientSocketFlag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, clientSocketFlag);
}
void *handleInputMessageFromKeyboard(void *clientSocketFileDescriptor)
{
    int fd = *((int *)clientSocketFileDescriptor);
    char sendingMessage[MessageSize];
    while (1)
    {
        fgets(sendingMessage,MessageSize-1,stdin);
        send(fd, sendingMessage, strlen(sendingMessage), 0);
        printf("Me: %s\n",sendingMessage);
    }
}
void handleReceivingMessageAtSocket(void *clientSocketFileDescriptor){
    int fd = *((int *)clientSocketFileDescriptor);
    char receivingMessage[MessageSize];
    for(;;)
    {
        while (recv(fd, receivingMessage, sizeof(receivingMessage), 0) > 0)
        {
            printf("Server: %s\n", receivingMessage);
        }
        memset(&receivingMessage, 0, sizeof(receivingMessage));
    }
}
int main(int argc, char **argv)
{
    /* 1. Create client socket */
    int clientSocketFileDescriptor = createSocketAt(ServerPort);
    if (clientSocketFileDescriptor == -1) return -1;
    /* 2. Regist server socket */
    struct sockaddr_in serverSocket = registServerSocketAdress(ServerPort, ServerHostName);
    /* 3. Connect client and server */
    if (connect(clientSocketFileDescriptor, (struct sockaddr *)&serverSocket, sizeof(serverSocket))) {
        perror("Could not establish connection between client and server");
        return -1;
    }
    /* 4. Connect success*/
    printf("Connected\n");
    /* 5. Config multiplex socket */
    configSocketMultiplex(&clientSocketFileDescriptor);
    /* 6. Handle sending message in a new thread */
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, handleInputMessageFromKeyboard, &clientSocketFileDescriptor);
    /* 7. Handle receiving message */
    handleReceivingMessageAtSocket(&clientSocketFileDescriptor);
    return 0;
}

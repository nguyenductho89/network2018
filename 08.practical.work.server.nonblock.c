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
//1 socket()...
//2 setsockopt()... // reuse address
//3 fcntl()... // nonblocking
//4 bind()...
//5 listen()...
//while (1) {
//    clientfd = accept();
//    if (clientfd > 0) {
//        fcntl()... // nonblocking client
//        while (1) {
//            if (read()... > 0) printf()...
//                if (poll()...) {
//                    scanf()...
//                    write()...
//                }
//        }
//    }
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
int main()
{
    
    int sockfd, clientfd;
    socklen_t clen;
    struct sockaddr_in caddr;
    //Create socket at port
    sockfd = createSocketAtPort(8784);
    while (1) {
        //Client accept
        clen=sizeof(caddr);
        if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) > 0) {
            ssize_t messageSize;
            char message[256];
            while(clientfd > 0){
                printf("Start session...\n");
                // non blocking client
                int fl = fcntl(clientfd, F_GETFL, 0);
                fl |= O_NONBLOCK;
                fcntl(clientfd, F_SETFL, fl);
                //Receive message
                messageSize = recv(clientfd, message, sizeof(message), 0);
                printf("@@@@@@@@@@@@ Client: %s\n",message);
                //Send message
                printf("...\n");
                scanf("%s",message) ;
                messageSize = send(clientfd, message, strlen(message), 0);
                printf("############# Server: %s\n",message);
            }
        }
    }
    return 0;
}

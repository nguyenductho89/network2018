//
//  main.c
//  Client08
//
//  Created by Nguyễn Đức Thọ on 3/24/18.
//  Copyright © 2018 Nguyễn Đức Thọ. All rights reserved.
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
//socket()...
//setsockopt()... // reuse address
//fcntl()... // nonblocking
//gethostbyname()...
//connect()...
//while (1) {
//    if (read()... > 0) printf()...
//        if (poll()...) {
//            scanf()...
//            write()...
//        }
int connectToServer(char *hostname, unsigned short port) {
    struct hostent* h;
    h = gethostbyname(hostname);
    if (h == NULL) {
        printf("Unknown host\n");
        return 0;
    }
    printf( "IPAddress of :%s",hostname );
    if (h->h_addr_list[0] != NULL) {
        printf( "\n %s\n ", inet_ntoa( *( struct in_addr*)( h -> h_addr_list[0])));
        struct sockaddr_in saddr;
        int sockfd;
        if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Error creating socket\n");
        }
        memset(&saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
        saddr.sin_port = htons(port);
        if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
            printf("Cannot connect\n");
            return 0;
        }
        printf("Connect successfully!\n");
        //2 setsockopt()
        setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR, &(int){ 1 },
                   sizeof(int));
        //3 fcntl()
        int fl = fcntl(sockfd, F_GETFL, 0);
        fl |= O_NONBLOCK;
        fcntl(sockfd, F_SETFL, fl);
        return sockfd;
    }else {return 0;}
}
void startConversation(int sockfd) {
    ssize_t messageSize;
    char message[256];
    //Send message
    printf("...\n");
    scanf("%s",message) ;
    messageSize = send(sockfd, message, strlen(message), 0);
    printf("@@@@@@@@@@@@ Client: %s\n",message);
    
    //Receive message
    messageSize = recv(sockfd, message, sizeof(message), 0);
    printf("############# Server: %s\n",message);
}

int main( int argc, char *argv[] ){
    char hostName[100];
    printf("Nhap hostname\n");
    scanf("%s",hostName);
    int sockfd = connectToServer(hostName,8784);
    while (1) {
        startConversation(sockfd);
    }
    return 0;
}

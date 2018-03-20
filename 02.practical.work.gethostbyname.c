//
//  main.c
//  getHostByName
//
//  Created by Nguyễn Đức Thọ on 3/20/18.
//  Copyright © 2018 Nguyễn Đức Thọ. All rights reserved.
//

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
int main(int argc, const char * argv[]) {
    // insert code here...
    while (1) {
        printf("What domain name are you looking for?\n");
        char str1[256];
        scanf("%s",str1);
        struct hostent *lh = gethostbyname(str1);
        unsigned int i=1;
        if (lh != NULL) {
            while ( lh -> h_addr_list[i] != NULL) {
                printf( "Answer: %s's ip address is:  %s \n",str1, inet_ntoa( *( struct in_addr*)( lh -> h_addr_list[i])));
                i++;
            }
        }else {
            printf("Sorry,I could not find out. Please ask for another\n");
        }
    }
    return 0;
}


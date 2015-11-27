//
//  client.c
//  Multithreaded Bank
//
//  Created by Vince Xie on 11/27/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int port = 6798;
    
    char buffer[256];
    if (argc < 2) {
        printf("Error: No hostname inputted\n");
        exit(0);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Error opening socket.\n");
        exit(0);
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Error: no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("Error connecting to server\n");
        exit(0);
    }
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0){
        printf("Error writing to socket\n");
        exit(0);
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0){
        printf("Error reading from socket\n");
        exit(0);
    }
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}

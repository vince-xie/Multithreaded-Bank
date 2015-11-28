//
//  server.c
//  Multithreaded Bank
//
//  Created by Vince Xie on 11/27/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/time.h>
#include "server.h"

Bank *bank;

static void set_timer( int first, int next )
{
    struct itimerval    interval;
    
    interval.it_interval.tv_sec = next;
    interval.it_interval.tv_usec = 0;
    interval.it_value.tv_sec = first;
    interval.it_value.tv_usec = 0;
    setitimer( ITIMER_REAL, &interval, 0 );
}

void printInfo(int signo){
    int i;
    printf("\nBank Info\n");
    for(i = 0; i < 20; i++){
        if(bank->accounts[i].open == 1){
            printf("Account number: %d Name: %s Balance: %.2f ", i, bank->accounts[i].name, bank->accounts[i].balance);
            if(bank->accounts[i].session == 1){
                printf("In-session: Yes\n");
            } else {
                printf("In-session: No\n");
            }
        }
    }
}

void setUpBank(){
    bank = (Bank *)malloc(sizeof(struct BankInfo));
    int i;
    for(i = 0; i < 20; i++){
        bank->accounts[i].open = 0;
    }
}

void addAccount(int index, char *name){
    bank->accounts[index].open = 1;
    strcpy(bank->accounts[index].name, name);
    bank->accounts[index].balance = 0;
    bank->accounts[index].session = 1;
}

int main(int argc, char *argv[])
{
    setUpBank();
    int sockfd, newsockfd;
    int port = 6798;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    long n;
    printf("Setting up server...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Error opening socket\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("Error on binding\n");
        exit(1);
    }
    printf("Server ready for connections\n");
    
    signal(14, printInfo);
    set_timer(20, 20);
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        printf("Error on accept.\n");
        exit(1);
    }
    bzero(buffer,256);
    
    n = read(newsockfd,buffer,255);
    if (n < 0){
        printf("Error reading from socket\n");
        exit(1);
    }
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0){
        printf("Error writing to socket\n");
        exit(1);
    }
    
    close(newsockfd);
    close(sockfd);
    return 0;
}


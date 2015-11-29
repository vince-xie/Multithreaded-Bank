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
#include <pthread.h>

Bank *bank;
pthread_t thread;

static void set_timer( int first, int next )
{
    struct itimerval interval;
    
    interval.it_interval.tv_sec = next;
    interval.it_interval.tv_usec = 0;
    interval.it_value.tv_sec = first;
    interval.it_value.tv_usec = 0;
    setitimer( ITIMER_REAL, &interval, 0 );
}

void *new_session(void *arg) {
    long n;
    thread_data *data = (thread_data *)arg;
    while(1){
        char buffer[256];
        bzero(buffer,256);
        n = read(data->newsockfd,buffer,255);
        if (n < 0){
            printf("Error reading from socket\n");
            pthread_exit(NULL);
        }
        
        if(strncasecmp(buffer, "Exit", 4) == 0){
            n = write(data->newsockfd, "Exiting...", 10);
            close(data->newsockfd);
            pthread_exit(NULL);
        }
        if(strncasecmp(buffer, "Open", 4) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Open",4);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Start", 5) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Start",5);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Credit", 6) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Credit",6);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Debit", 5) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Debit",5);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Balance", 7) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Balance",7);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Finish", 6) == 0){
            printf("Here is the command: %s\n",buffer);
            n = write(data->newsockfd,"Finish",6);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else {
            printf("Invalid command\n");
            n = write(data->newsockfd,"Invalid commmand", 16);
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        }
    }
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
    printf("\n");
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

void creditAccount(int index, int amount){
    bank->accounts[index].balance += amount;
}

void debitAccount(int index, int amount){
    bank->accounts[index].balance -= amount;
}

void printBalance(int index){
    printf("%.2f", bank->accounts[index].balance);
}

int new_thread(int sockfd, int newsockfd){
    
    int rc;
    thread_data thr_data;
    thr_data.sockfd = sockfd;
    thr_data.newsockfd = newsockfd;
    if((rc = pthread_create(&thread, NULL, new_session, &thr_data))){
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
    }
    
    pthread_join(thread, NULL);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    //setup
    setUpBank();
    int sockfd, newsockfd;
    int port = 6799;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    
    //setting up server
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
    
    //set up printing of bank info every 20 seconds
    signal(14, printInfo);
    set_timer(20, 20);
    
    //listen for new connections
    while(1){
        listen(sockfd,5); //check backlog
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            printf("Error on accept.\n");
            exit(1);
        }
        new_thread(sockfd, newsockfd);
    }
    close(sockfd);
    return 0;
}


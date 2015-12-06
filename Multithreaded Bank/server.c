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

#define NUMTHREADS 100

Bank *bank;
pthread_t acceptor;
//pthread_t threads[20];

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
    
	connection_t * conn;
	char acctName[100];
	char buffer[256]; 
	char *buf;
	long n;
	//long addr = 0;

	if (!arg) pthread_exit(0); 
	conn = (connection_t *)arg;
	
	
    while(1){
		int x;
		double amount;
        bzero(buffer,256);
		bzero(acctName,100);
        n = read(conn->sock,buffer,255);
        if (n < 0){
            printf("Error reading from socket\n");
            pthread_exit(NULL);
        }
        
        if(strncasecmp(buffer, "Exit", 4) == 0){
			printf("Client %d exited\n", conn->sock);
            n = write(conn->sock, "Exiting...", 10);
            close(conn->sock);
			free(conn);
            pthread_exit(NULL);
        }
        if(strncasecmp(buffer, "Open", 4) == 0){
            //n = write(conn->sock,"Open",4);
			
			buf = buffer + 4;
			
			sscanf(buf, "%s", acctName);
			
			if(addAccount(acctName) == 0){
				printf("Client %d added account: %s\n", conn->sock, acctName);
				n = write(conn->sock,"Added account\n", 30);
			}else{
				printf("Client %d could not add account: %s. The account already exists or there is no space available.\n", conn->sock, acctName);
				n = write(conn->sock, "Error: could not add account. The account already exists or there is no space available.\n", 100);
			}
			
			if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
			
			
			
        } else if(strncasecmp(buffer, "Start", 5) == 0){
            //printf("Here is the command: %s\n",buffer);
            //n = write(conn->sock,"Start",5);
			
			if(conn->session != -1){
				printf("Client %d tried to start an account but client is already in session\n", conn->sock);
				n = write(conn->sock,"Client is already in session\n", 30);
				if (n < 0){
					printf("Error writing to socket\n");
					pthread_exit(NULL);
				}
				continue;
			}
			
			buf = buffer + 5;
			
			sscanf(buf, "%s", acctName);
			
			x = startAccount(acctName);
			
			if(x > -1){
				printf("Client %d started account: %s\n", conn->sock, acctName);
				n = write(conn->sock,"Started account\n", 30);
				conn->session = x;
			}else{
				printf("Client %d could not start account: %s. Account does not exist or is already in session.\n", conn->sock, acctName);
				n = write(conn->sock, "Error: could not start account. Account does not exist or is already in session.\n", 100);
			}
			
			
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Credit", 6) == 0){
            //printf("Here is the command: %s\n",buffer);
            //n = write(conn->sock,"Credit",6);
			
			if(conn->session == -1){
				printf("Client %d tried to credit account, but client is not in session.\n", conn->sock);
				n = write(conn->sock,"Client is not in session.\n", 30);
				if (n < 0){
					printf("Error writing to socket\n");
					pthread_exit(NULL);
				}
				continue;
			}
			
			buf = buffer + 6;
			
			sscanf(buf, "%lf", &amount);
			
			creditAccount(conn->session, amount);
			
			printf("Client %d deposited $%.2f\n", conn->sock, amount);
			n = write(conn->sock, "Credited account.\n", 30);
			
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Debit", 5) == 0){
            //printf("Here is the command: %s\n",buffer);
            //n = write(conn->sock,"Debit",5);
			
			if(conn->session == -1){
				printf("Client %d tried to debit account, but client is not in session.\n", conn->sock);
				n = write(conn->sock,"Client is not in session.\n", 30);
				if (n < 0){
					printf("Error writing to socket\n");
					pthread_exit(NULL);
				}
				continue;
			}
			
			buf = buffer + 5;
			
			sscanf(buf, "%lf", &amount);
			
			x = debitAccount(conn->session, amount);
			
			if(x == 0){
				printf("Client %d withdrew $%.2f\n", conn->sock, amount);
				n = write(conn->sock, "Debited account.\n", 30);
			}else{
				printf("Client %d tried to debit account, but does not have enough funds.\n", conn->sock, amount);
				n = write(conn->sock, "Not enough funds.\n", 30);
			}	
			
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
            
        } else if(strncasecmp(buffer, "Balance", 7) == 0){
            //printf("Here is the command: %s\n",buffer);
            //n = write(conn->sock,"Balance",7);
			
			if(conn->session == -1){
				printf("Client %d requested balance, but client is not in session.\n", conn->sock);
				n = write(conn->sock,"Client is not in session.\n", 30);
				if (n < 0){
					printf("Error writing to socket\n");
					pthread_exit(NULL);
				}
				continue;
			}
			
			buf = buffer;
			
			sprintf(buf, "Balance for account %s is $%.2f\n", &(bank->accounts[conn->session].name), &(bank->accounts[conn->session].balance)); 
			
			printf("Client %d requested balance.\n", conn->sock);
			n = write(conn->sock, buf, 100);
			
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else if(strncasecmp(buffer, "Finish", 6) == 0){
            //printf("Here is the command: %s\n",buffer);
            //n = write(conn->sock,"Finish",6);
			
			if(conn->session == -1){
				printf("Client %d tried to finish session, but client is not in session.\n", conn->sock);
				n = write(conn->sock,"Client is not in session.\n", 30);
				if (n < 0){
					printf("Error writing to socket\n");
					pthread_exit(NULL);
				}
				continue;
			}
			
			bank->accounts[conn->session].session = -1;
			
			conn->session = -1;
			
			
			printf("Client %d finished session.\n", conn->sock);
			n = write(conn->sock, "Finished session\n", 30);
			
            if (n < 0){
                printf("Error writing to socket\n");
                pthread_exit(NULL);
            }
        } else {
            printf("Invalid command\n");
            n = write(conn->sock,"Invalid commmand", 16);
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
            printf("Account number: %d\tName: %s\tBalance: %.2f\t", i, bank->accounts[i].name, bank->accounts[i].balance);
            if(bank->accounts[i].session == 1){
                printf("\tIN SERVICE\n");
            }else{
				printf("\n");
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

/*void setUpThreads(){
	int i;
    for(i = 0; i < NUMTHREADS; i++){
        threads[i] = NULL;
    }
}*/

int addAccount(char *name){
	
	int i, index;
	
	index = -1;
	
	for(i = 0; i < 20; i++){
		if(bank->accounts[i].open == 0){
			index = i;
			break;
		}
		
		if(strcmp(bank->accounts[i].name, name) == 0){
			return -1;
		}
	}
	
	if(index == -1){
		return -1;
	}
	
    bank->accounts[index].open = 1;
    strcpy(bank->accounts[index].name, name);
    bank->accounts[index].balance = 0;
    bank->accounts[index].session = 0;
	
	return 0;
}

int startAccount(char *name){
	
	int i, index;
	
	index = -1;
	
	for(i = 0; i < 20; i++){
		if(bank->accounts[i].open == 1 && strcmp(bank->accounts[i].name,name) == 0){
			index = i;
			break;
		}
	}
	
	if(index == -1){
		return -1;
	}
	
	if(bank->accounts[index].session == 1){
		return -1;
	}
	
    bank->accounts[index].session = 1;
	
	return index;
}

void creditAccount(int index, double amount){
    bank->accounts[index].balance += amount;
}

int debitAccount(int index, double amount){
	if(bank->accounts[index].balance - amount >= 0){
		bank->accounts[index].balance -= amount;
	}else{
		return -1;
	}
	
	return 0;
   
}

/*void printBalance(int index){
    printf("%.2f", bank->accounts[index].balance);
}*/

/*int new_thread(int sockfd, int newsockfd){
    int i;
    int rc;
    thread_data thr_data;
    thr_data.sockfd = sockfd;
    thr_data.newsockfd = newsockfd;
	
	for(i = 0; i < NUMTHREADS; i++){
		if(threads[i] == NULL){
			break;
		}
	}
	
    if((rc = pthread_create(&acceptor, NULL, new_session, &thr_data))){
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
    }
    
    pthread_join(acceptor, NULL);
    return EXIT_SUCCESS;
}*/

int main(int argc, char *argv[])
{
    //setup
    setUpBank();
	//setUpThreads();
    int sockfd, newsockfd;
    int port = 6799;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
	connection_t * connection;
    
    //setting up server
    printf("Setting up server...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Error opening socket\n");
        exit(1);
    }
	
	//fix for bind error
	int reuse;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1){
		printf("Reuse port Error\n");
	}
	
	
	//configure socket
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
	
	listen(sockfd,5); //check backlog
    
    //listen for new connections
    while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sockfd, &connection->address, &connection->addr_len);
		connection->session = -1;
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			printf("Client %d connected\n", connection->sock);
			pthread_create(&acceptor, 0, new_session, (void *)connection);
			pthread_detach(acceptor);
		}
	}
	
    close(sockfd);
    return 0;
}


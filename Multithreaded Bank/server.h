//
//  server.h
//  Multithreaded Bank
//
//  Created by Vince Xie on 11/28/15.
//  Copyright © 2015 Vince Xie. All rights reserved.
//

#ifndef server_h
#define server_h

typedef struct Account {
    int open;
    char name[101];
    double balance;
    int session;
} Account;

typedef struct BankInfo {
    Account accounts[20];
} Bank;

void printInfo(int signo);

static void set_timer( int first, int next );

void setUpBank();

void addAccount(int index, char *name);

#endif /* server_h */

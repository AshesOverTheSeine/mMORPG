#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ncurses.h> 
#include <iostream>
#include <time.h>

// These are the values the server reads
#define UP 'u'
#define DOWN 'd'
#define LEFT 'l'
#define RIGHT 'r'
#define SELECT 'y'
#define BACK 'n'
#define EXAMINE 'e'
#define MESSAGE 'm'   // TODO: Don't think I need this one?
#define QUIT 'x'

// These are the values the client checks with input from getch()
int north = 119;    // w
int south = 115;    // s
int west = 97;      // a
int east = 100;     // d
int sel = 101;      // e
int back = 113;     // q
int examine = 102;  // f
int message = 10;   // Enter
int quit = 27;      // Esc

using namespace std;

// Prints the given message(s) to the chat window
void printString(string msg, WINDOW * chat) {
    wmove(chat, 2, 2);
    for (int i = 0; i < msg.length(); i++)
        waddch(chat, msg[i]);
    wrefresh(chat);
}

// Error handling for client socket
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// Allows the user to send chat messages or make a move
// Returns true if the player has decided to quit the game
bool action(int sockfd) {
    char buffer[141];
    bzero(buffer,141);
    int ch = getch();
    if (ch == message) {
        nocbreak();
        fgets(buffer, 141, stdin);
        cbreak();
    } 
    // These must be else ifs and not switches since north, south, etc. are not constants
    else if (ch == north) buffer[0] = UP;
    else if (ch == south) buffer[0] = DOWN;
    else if (ch == west) buffer[0] = LEFT;
    else if (ch == east) buffer[0] = RIGHT;
    else if (ch == sel) buffer[0] = SELECT;
    else if (ch == back) buffer[0] = BACK;
    else if (ch == examine) buffer[0] = EXAMINE;
    else if (ch == quit) buffer[0] = QUIT;

    if (write(sockfd,buffer,141) < 0) error("ERROR writing to socket");

    if (ch == quit) return true;
    else return false;
}

int main(int argc, char *argv[])
{
    bool quit = false;

    // Create stdscrow
    initscr();

    // Game screen
    // TODO: Allow for changing size
    WINDOW * game = newwin(30,144,0,0);

    // Chat screen
    // TODO: Allow for changing size
    WINDOW * chat = newwin(15,144,30,0);


    // Don't display user input
    noecho();

    // Seed RNG
    srand (time(NULL));

    // Allow special character input
    keypad(stdscr, TRUE);

    // Allow color schemes
    start_color();
    // Enemy/You Lose text
    init_pair(1, COLOR_RED, COLOR_BLACK);
    // Target/You Win text
    init_pair(2, COLOR_BLACK, COLOR_GREEN);

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];

    portno = 666;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // The player makes all their moves here
    while (!quit) {
        wborder(game, 0, 0, 0, 0, 0, 0, 0, 0);
        wborder(chat, 0, 0, 0, 0, 0, 0, 0, 0);
        wrefresh(game);
        wrefresh(chat);

        quit = action(sockfd); 
        bzero(buffer, 1024);
        if (read(sockfd, buffer, 1024) < 0) error("ERROR reading from socket");
        printString(buffer, chat);
    }
    close(sockfd);
    endwin();
    return 0;
}


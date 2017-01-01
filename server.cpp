/* 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "Player.cpp"

using namespace std;

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

/*
   string map = 
   "##########"
   "#        #"
   "#        #"
   "#        #"
   "#        #"
   "#        #"
   "#        #"
   "#        #"
   "#        #"
   "##########"
   */

void showMap(char map[10][10]) {
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10; i++)
            cout << map[i][j];
        cout << '\n';
    }
}


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char map[10][10] ;

    for (int j = 0; j < 10; j++)
        for (int i = 0; i < 10; i++)
            if (i == 0 || i == 9 || j == 0 || j == 9) map[i][j] = '#';
            else map[i][j] = ' ';

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 666;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Credit to merogg
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            error("setsockopt(SO_REUSEADDR) failed");

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR binding socket");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    Player player = Player("player");

    do {
        bzero(buffer,256);
        n = read(newsockfd,buffer,256);

        bool move = false;

        if (n < 0) error("ERROR reading from socket");

        if (strlen(buffer) > 1) printf("Recieved message: %s\n",buffer);
        else
            switch(buffer[0]) {
                // Check desired directions, then move the player if allowed
                case UP: 
                    if (map[player.getXPos()][player.getYPos()-1] == ' ') move = true;
                    player.turn(buffer[0]);
                    break;
                case DOWN: 
                    if (map[player.getXPos()][player.getYPos()+1] == ' ') move = true;
                    player.turn(buffer[0]);
                    break;
                case LEFT: 
                    if (map[player.getXPos()-1][player.getYPos()] == ' ') move = true;
                    player.turn(buffer[0]);
                    break;
                case RIGHT: 
                    if (map[player.getXPos()+1][player.getYPos()] == ' ') move = true;
                    player.turn(buffer[0]);
                    break;
                case SELECT: printf("Accepted!\n");
                             break;
                case BACK: printf("Went back!\n");
                           break;
                case EXAMINE: printf("Nothing to examine!\n");
                              break;
                case QUIT: printf("You quit!\n");  // TODO: I don't think this matters?
                           break;
            }

        map[player.getXPos()][player.getYPos()] = ' ';
        if (move) player.move(buffer[0]);
        map[player.getXPos()][player.getYPos()] = player.render();
        showMap(map);

        if (write(newsockfd,buffer,256) < 0) error("ERROR writing to socket");

    } while(buffer[0] != QUIT || strlen(buffer) != 1);
    close(newsockfd);
    close(sockfd);
    return 0; 
}


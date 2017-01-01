#include <string>
using namespace std;

class Player {
    char icon;
    string name;
    int gold, exp, xPos, yPos;

    public:

    // Create a new player
    Player(string name) {
        this->icon = '^';
        this->name = name;
        this->gold = 0;
        this->exp = 0;
        this->xPos = 5;
        this->yPos = 5;
    }

    char render() {
        return icon;
    }

    void turn(char dir) {
        switch(dir){
            case 'u':
                icon = '^';
                break;
            case 'd':
                icon = 'v';
                break;
            case 'l':
                icon = '<';
                break;
            case 'r':
                icon = '>';
                break;
        }
    }

    // Moves the player in the desired direction. Vacancy of the new position must be verified before calling this
    bool move(char dir) {
        switch(dir) {
            case 'u':
                yPos--;
                break;
            case 'd':
                yPos++;
                break;
            case 'l':
                xPos--;
                break;
            case 'r':
                xPos++;
                break;
        }
    }

    int getXPos() {
        return xPos;
    }

    int getYPos() {
        return yPos;
    }
};

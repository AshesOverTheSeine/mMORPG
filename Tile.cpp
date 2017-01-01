#include <string>
using namespace std;

class Tile {
    char icon;
    string info;

    public:

    Tile(char icon) {
        this->icon = icon;
        switch(icon) {
            case '#':
                this->info = "A solid wall.";
                break;
            case ' ':
                this->info = "There's nothing here.";
                break;
        }
    }

    char render() {
        return icon;
    }

    string examine() {
        return info;
    }
};

// Might not bother with the player being a tile
/*
class Player:Tile {
    string name;

    public:

    Player(string name, Tile tile) {
        this->name = name;
        this->icon = tile.render();
        this->info = tile.examine();
    }

    string getName() {
        return name;
    }
};
*/

#ifndef GAME_H
#define GAME_H
#include "gametypes.h"
#include <array>
class game
{
public:
    gameboard board;
    game(int rows, int cols);
    void gamecreate(int difficulty);
    void randomlyrotate();
    void update();
    bool arebulbslit();
    void rotatenode(int row, int col);
    void print();

private:
    gameboard gamecreateempty(int, int);
    node* getnodeat(int row, int col);
    void unpowernodes();
    void recursiveupdate(int row, int col);
    void createnode(nodetype type, int row, int col, std::array<bool,4> sides);
};

#endif // GAME_H

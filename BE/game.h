#ifndef GAME_H
#define GAME_H
#include "gametypes.h"
class game
{
public:
    game(int rows, int cols);
    void gamecreate(int difficulty);
    void randomlyrotate();
    void update();
    bool arebulbslit();
    void rotatenode(int row, int col);

private:
    gameboard board;
    gameboard gamecreateempty(int, int);
    node* getnode(int row, int col);
};

#endif // GAME_H

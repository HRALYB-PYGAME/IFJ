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

private:
    gameboard board;
    gameboard gamecreateempty(int, int);
};

#endif // GAME_H

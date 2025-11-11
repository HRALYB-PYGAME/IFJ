#ifndef GAME_H
#define GAME_H
#include "gametypes.h"
#include <array>
#include <string>
#include <QWidget>
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
    node* getnodeat(int row, int col);
    QPixmap getimage(int row, int col);
    void rotate(int row, int col);

    void savegame();
private:
    gameboard gamecreateempty(int, int);
    void unpowernodes();
    void recursiveupdate(int row, int col, side mustbe);
    void createnode(nodetype type, int row, int col, std::array<bool,4> sides);
    QPixmap cachedImages[14][4];
    void rebuildCache();
};

#endif // GAME_H

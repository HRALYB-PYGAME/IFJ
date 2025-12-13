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
    bool editing;
    game(int rows, int cols, bool editing=false);
    int moveCount;
    void gamecreate(int difficulty);
    void randomlyrotate();
    void update();
    bool arebulbslit();
    void print();
    node* getnodeat(int row, int col);
    QPixmap getimage(int row, int col);
    void rotate(int row, int col);

    void savegame(QString filename = "hra");
    void loadgame(QString filename);
    bool deletegame(QString filename);
    bool renamegame(QString filenamebefore, QString filenameafter);
    void createnode(nodetype type, int row, int col, std::array<bool,4> sides);
    void rotateby(int row, int col, int rotation);
    void resetMoveCount();
private:
    gameboard gamecreateempty(int, int);
    void unpowernodes();
    void recursiveupdate(int row, int col, side mustbe);
    QPixmap cachedImages[14][4];
    void rebuildCache();
};

#endif // GAME_H

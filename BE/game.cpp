#include "game.h"
#include "gametypes.h"
#include <vector>

game::game(int rows, int cols) {
    this->board = this->gamecreateempty(rows, cols);
}

gameboard game::gamecreateempty(int rows, int cols){
    gameboard newgame;

    if (rows <= 0 || cols <= 0) return newgame;

    newgame.cols = cols;
    newgame.rows = rows;
    newgame.nodes = std::vector<node>(rows*cols, {bulb, 0, false});

    return newgame;
}

void game::gamecreate(int difficulty){

}

void game::randomlyrotate(){

}

void game::update(){

}

bool game::arebulbslit(){
    return false;
}

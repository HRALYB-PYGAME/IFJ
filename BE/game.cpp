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

// creates a new level configuration
void game::gamecreate(int difficulty){

}

// randomly rotates every node on the game board
void game::randomlyrotate(){

}

// updates the game board by powering only nodes connected to source
void game::update(){

}

// checks if all bulbs are lit
// returns true when all bulbs are lit, false otherwise
bool game::arebulbslit(){
    return false;
}

// rotates a node on [row, col] coordinates clockwise by 90 degrees
void game::rotatenode(int row, int col){

}

node* game::getnode(int row, int col){
    return nullptr;
}

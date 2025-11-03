#include "game.h"
#include "gametypes.h"
#include <vector>
#include <cstdlib>
#include <array>
#include <iostream>
#include <QWidget>

game::game(int rows, int cols) {
    this->board = this->gamecreateempty(rows, cols);
    rebuildCache();
}


void game::rebuildCache() {
    std::array<std::string, 14> imagenames = {":/Empty.png", ":/linkD.png", ":/linkDpowered.png", ":/linkI.png", ":/linkIpowered.png", ":/linkL.png", ":/linkLpowered.png", ":/linkT.png", ":/linkTpowered.png", ":/linkX.png", ":/linkXpowered.png", ":/bulb.png", ":/bulbpowered.png", ":/power.png"};
    for (int image = 0; image < 14; image++) {
        QPixmap baseimage(imagenames[image].c_str());
        for (int rot = 0; rot < 4; ++rot) {
            QTransform t;
            t.rotate(90 * rot);
            cachedImages[image][rot] = baseimage.transformed(t, Qt::FastTransformation);
        }
    }
}

gameboard game::gamecreateempty(int rows, int cols){
    gameboard newgame;

    if (rows <= 0 || cols <= 0) return newgame;

    newgame.cols = cols;
    newgame.rows = rows;
    newgame.nodes = std::vector<node>(rows*cols, {empty, { false, false, false, false }, o, 0, false});

    return newgame;
}

// creates a new random level configuration
void game::gamecreate(int difficulty){
    int powerrow = rand() % this->board.rows;
    int powercol = rand() % this->board.cols;
    int rows = this->board.rows;
    int cols = this->board.cols;
    this->board.powerrow = powerrow;
    this->board.powercol = powercol;

    // all types of node configurations
    std::array<bool, 4> dsides = {true, false, false, false};
    std::array<bool, 4> isides = {true, false, true, false};
    std::array<bool, 4> lsides = {true, true, false, false};
    std::array<bool, 4> tsides = {true, true, true, false};
    std::array<bool, 4> xsides = {true, true, true, true};

    // create the power node
    createnode(power, powerrow, powercol, dsides);

    std::vector<position> path;
    path.insert(path.end(), {powerrow, powercol});
    std::vector<std::vector<bool>> occupiednodes(rows, std::vector<bool>(cols, false));
    occupiednodes[powerrow][powercol] = true;

    float probability = 0.02;

    std::cout << "before loop" << std::endl;
    // path generation
    while (((double) rand() / (RAND_MAX)) > probability){
        int direction = rand() % 4;

        if (direction == up && powerrow != 0){
            if(occupiednodes[powerrow-1][powercol] == false){
                powerrow -= 1;
                occupiednodes[powerrow][powercol] = true;
                path.insert(path.end(), {powerrow, powercol});
            }
        }

        if (direction == right && powercol != cols-1){
            if(occupiednodes[powerrow][powercol+1] == false){
                powercol += 1;
                occupiednodes[powerrow][powercol] = true;
                path.insert(path.end(), {powerrow, powercol});
            }
        }

        if (direction == down && powerrow != rows-1){
            if(occupiednodes[powerrow+1][powercol] == false){
                powerrow += 1;
                occupiednodes[powerrow][powercol] = true;
                path.insert(path.end(), {powerrow, powercol});
            }
        }

        if (direction == left && powercol != 0){
            if(occupiednodes[powerrow][powercol-1] == false){
                powercol -= 1;
                occupiednodes[powerrow][powercol] = true;
                path.insert(path.end(), {powerrow, powercol});
            }
        }
    }

    std::cout << "after loop" << std::endl;

    for(int i=1; i < path.size() - 1; i++){
        position previous = path[i-1];
        position current = path[i];
        position next = path[i+1];

        std::array<bool, 4> sides = {false, false, false, false};
        if(current.col - previous.col == 1) sides[left] = true;
        if(current.col - previous.col == -1) sides[right] = true;
        if(current.row - previous.row == 1) sides[up] = true;
        if(current.row - previous.row == -1) sides[down] = true;
        if(next.col - current.col == 1) sides[right] = true;
        if(next.col - current.col == -1) sides[left] = true;
        if(next.row - current.row == 1) sides[down] = true;
        if(next.row - current.row == -1) sides[up] = true;
        createnode(link, current.row, current.col, sides);
    }

    std::cout << "after second loop" << std::endl;
    position current = path[path.size()-1];
    position previous = path[path.size()-2];
    std::array<bool, 4> sides = {false, false, false, false};
    if(current.col - previous.col == 1) sides[left] = true;
    if(current.col - previous.col == -1) sides[right] = true;
    if(current.row - previous.row == 1) sides[up] = true;
    if(current.row - previous.row == -1) sides[down] = true;
    createnode(bulb, current.row, current.col, sides);

    std::cout << "before random rotate" << std::endl;
    randomlyrotate();
    std::cout << "after random rotate" << std::endl;
}

void game::print(){
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            node* currentnode = getnodeat(row, col);
            if (currentnode->type != empty){
                std::cout << "row:" << row << "; col:" << col << std::endl;
                std::cout << "type:" << currentnode->type << std::endl;
                std::cout << "borders:" << currentnode->sides[0] << currentnode->sides[1] << currentnode->sides[2] << currentnode->sides[3] << std::endl;
            }
        }
    }
}

void game::createnode(nodetype type, int row, int col, std::array<bool,4> sides){
    node* currentnode = getnodeat(row, col);
    currentnode->type = type;
    for(int i=0; i<4; i++){
        currentnode->sides[i] = sides[i];
    }

    int sidescount = sides[0] + sides[1] + sides[2] + sides[3];

    if (sidescount == 0) currentnode->shape = o;
    if (sidescount == 1){
        currentnode->shape = d;
        for(int i=0; i<4; i++) currentnode->sides[i] = false;
        currentnode->sides[up] = true;
    }
    if (sidescount == 2){
        if (sides[0] == sides[2] || sides[1] == sides[3]){
            currentnode->shape = i;
            for(int i=0; i<4; i++) currentnode->sides[i] = false;
            currentnode->sides[up] = true;
            currentnode->sides[down] = true;
        }
        else{
            currentnode->shape = l;
            for(int i=0; i<4; i++) currentnode->sides[i] = false;
            currentnode->sides[up] = true;
            currentnode->sides[right] = true;
        }
    }
    if (sidescount == 3){
        currentnode->shape = t;
        for(int i=0; i<4; i++) currentnode->sides[i] = true;
        currentnode->sides[left] = false;
    }
    if (sidescount == 4) currentnode->shape = x;

    currentnode->rotation = 0;
}

// randomly rotates every node on the game board
void game::randomlyrotate(){
    return;
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            node* currentnode = getnodeat(row, col);
            currentnode->rotation = rand() % 4;
            std::array<bool, 4> newsides;
            for(int i=0; i<4; i++) newsides[i] = currentnode->sides[i-currentnode->rotation]%4;
            for(int i=0; i<4; i++) currentnode->sides[i] = newsides[i];
        }
    }
}

QPixmap game::getimage(int row, int col){
    node* currentnode = getnodeat(row, col);
    nodetype buttontype = currentnode->type;
    int buttonrotation = currentnode->rotation;
    bool powered = currentnode->powered;

    int idx = 0;  // Empty

    switch(buttontype){
    case empty:  idx = 0;  break;
    case power:  idx = 13; break;
    case bulb:   idx = powered ? 12 : 11; break;
    case link:
        switch(currentnode->shape){
        case d: idx = powered ? 2 : 1; break;
        case i: idx = powered ? 4 : 3; break;
        case l: idx = powered ? 6 : 5; break;
        case t: idx = powered ? 8 : 7; break;
        case x: idx = powered ? 10 : 9; break;
        default: idx = 0;
        }
        break;
    }

    return cachedImages[idx][buttonrotation];  // ← ALREADY ROTATED!
}

// updates the game board by powering only nodes connected to source
void game::update(){
    unpowernodes();
    recursiveupdate(this->board.powerrow, this->board.powercol);
}

void game::rotate(int row, int col){
    node* activenode = getnodeat(row, col);
    activenode->rotation = (activenode->rotation + 1)%4;
    std::array<bool, 4> updated = {false, false, false, false};
    updated[0] = activenode->sides[3];
    updated[1] = activenode->sides[0];
    updated[2] = activenode->sides[1];
    updated[3] = activenode->sides[2];
    for(int i=0; i<4; i++){
        activenode->sides[i] = updated[i];
    }
}

void game::recursiveupdate(int row, int col){
    if (row < 0 || col < 0 || row >= this->board.rows || col >= this->board.cols)
        return;
    if (getnodeat(row, col)->powered)
        return;

    std::cout << "powering node" << row << col << std::endl;

    node* currentnode = getnodeat(row, col);
    currentnode->powered = true;
    if (currentnode->sides[up])
        recursiveupdate(row-1, col);
    if (currentnode->sides[right])
        recursiveupdate(row, col+1);
    if (currentnode->sides[down])
        recursiveupdate(row+1, col);
    if (currentnode->sides[left])
        recursiveupdate(row, col-1);
}

void game::unpowernodes(){
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            node* currentnode = getnodeat(row, col);
            currentnode->powered = false;
        }
    }
}

// checks if all bulbs are lit
// returns true when all bulbs are lit, false otherwise
bool game::arebulbslit(){
    bool lit = true;
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            node* currentnode = getnodeat(row, col);
            if (currentnode->type == bulb && currentnode->powered == false) lit = false;
        }
    }
    return lit;
}

// rotates a node on [row, col] coordinates clockwise by 90 degrees
void game::rotatenode(int row, int col){
    getnodeat(row, col)->rotation += 1;
    getnodeat(row, col)->rotation %= 4;
}

node* game::getnodeat(int row, int col){
    return &this->board.nodes[col*this->board.cols + row];
}

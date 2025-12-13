#include "game.h"
#include "gametypes.h"
#include <vector>
#include <cstdlib>
#include <array>
#include <iostream>
#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDataStream>

game::game(int rows, int cols, bool editing) {
    this->board = this->gamecreateempty(rows, cols);
    this->editing = editing;
    this->moveCount = 0;
    rebuildCache();
}

void game::loadgame(QString filename){
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> board.rows;
        in >> board.cols;
        in >> board.powerrow;
        in >> board.powercol;
        board.nodes.clear();
        for(int i=0; i<board.rows*board.cols; i++){
            int rotation;
            board.nodes.push_back(node());
            in >> board.nodes[i].type;
            in >> board.nodes[i].shape;
            in >> rotation;
            switch(board.nodes[i].shape){
            case nodeshape::o:
                board.nodes[i].sides = {false, false, false, false};
                break;
            case nodeshape::d:
                board.nodes[i].sides = {true, false, false, false};
                rotateby(i/board.cols, i%board.cols, rotation);
                break;
            case nodeshape::i:
                board.nodes[i].sides = {true, false, true, false};
                rotateby(i/board.cols, i%board.cols, rotation);
                break;
            case nodeshape::l:
                board.nodes[i].sides = {true, true, false, false};
                rotateby(i/board.cols, i%board.cols, rotation);
                break;
            case nodeshape::t:
                board.nodes[i].sides = {true, true, true, false};
                rotateby(i/board.cols, i%board.cols, rotation);
                break;
            case nodeshape::x:
                board.nodes[i].sides = {true, true, true, true};
                break;
            }
        }
        file.close();
    }
    this->moveCount = 0;
}

bool game::deletegame(QString filename){
    removerecord(filename.chopped(5));
    bool success = QFile::remove(QString("save/%1").arg(filename));
    return success;
}

int game::getrecordtime(QString levelname){
    QString filename = "records.json";
    QJsonObject recordsObj;

    QFile file(filename);
    if (!file.exists()) return 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            recordsObj = doc.object();
        }
    }

    if (recordsObj.contains(levelname))
        return recordsObj[levelname].toObject()["time"].toInt();
    return 0;
}

int game::getrecordsteps(QString levelname){
    QString filename = "records.json";
    QJsonObject recordsObj;

    QFile file(filename);
    if (!file.exists()) return 0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            recordsObj = doc.object();
        }
    }

    if (recordsObj.contains(levelname))
        return recordsObj[levelname].toObject()["steps"].toInt();
    return 0;
}

bool game::iscompleted(QString levelname){
    QString filename = "records.json";
    QJsonObject recordsObj;

    QFile file(filename);
    if (!file.exists()) return false;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            recordsObj = doc.object();
        }
    }

    if (recordsObj.contains(levelname)) return true;
    return false;
}

void game::removerecord(QString levelname){
    QString filename = "records.json";
    QJsonObject recordsObj;

    QFile file(filename);
    if (!file.exists()) return;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            recordsObj = doc.object();
        }
    }

    if (recordsObj.contains(levelname)) {
        recordsObj.remove(levelname);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonDocument doc(recordsObj);
            file.write(doc.toJson());
            file.close();
        }
    }

}


void game::addrecord(QString levelname, int time, int steps){
    QString filename = "records.json";
    QJsonObject recordsObj;

    QFile file(filename);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            recordsObj = doc.object();
        }
    }

    if (recordsObj.contains(levelname)) {
        QJsonObject levelObj = recordsObj[levelname].toObject();
        int storedTime = levelObj.value("time").toInt();
        int storedSteps = levelObj.value("steps").toInt();
        if (time < storedTime) {
            levelObj["time"] = time;
        }
        if (steps < storedSteps) {
            levelObj["steps"] = steps;
        }
        recordsObj[levelname] = levelObj;
    } else {
        QJsonObject levelObj;
        levelObj["time"] = time;
        levelObj["steps"] = steps;
        recordsObj[levelname] = levelObj;
    }
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(recordsObj);
        file.write(doc.toJson());
        file.close();
    }
}

void game::savegame(QString filename){
    QFile file(QString("save/%1.zvaz").arg(filename));
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << board.rows;
        out << board.cols;
        out << board.powerrow;
        out << board.powercol;
        for(int i=0; i<board.rows*board.cols; i++){
            out << board.nodes[i].type;
            out << board.nodes[i].shape;
            out << board.nodes[i].rotation;
        }
        file.close();
        removerecord(filename);;
    }
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
    this->moveCount = 0;
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

    position current = path[path.size()-1];
    position previous = path[path.size()-2];
    std::array<bool, 4> sides = {false, false, false, false};
    if(current.col - previous.col == 1) sides[left] = true;
    if(current.col - previous.col == -1) sides[right] = true;
    if(current.row - previous.row == 1) sides[up] = true;
    if(current.row - previous.row == -1) sides[down] = true;
    createnode(bulb, current.row, current.col, sides);

    randomlyrotate();

    update();
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

void game::rotateby(int row, int col, int rotation){
    for(int i=0; i<rotation; i++){
        rotate(row, col);
    }
}

// randomly rotates every node on the game board
void game::randomlyrotate(){
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            int r = rand()%4;
            rotateby(row, col, r);
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
    recursiveupdate(this->board.powerrow, this->board.powercol, none);
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

void game::recursiveupdate(int row, int col, side mustbe){
    if (row < 0 || col < 0 || row >= this->board.rows || col >= this->board.cols){
        return;
    }
    if (mustbe != none && !getnodeat(row, col)->sides[mustbe]){
        return;
    }
    if (getnodeat(row, col)->powered){
        return;
    }

    node* currentnode = getnodeat(row, col);

    currentnode->powered = true;
    if (currentnode->sides[up]){
        recursiveupdate(row-1, col, down);
    }
    if (currentnode->sides[right]){
        recursiveupdate(row, col+1, left);
    }
    if (currentnode->sides[down]){
        recursiveupdate(row+1, col, up);
    }
    if (currentnode->sides[left]){
        recursiveupdate(row, col-1, right);
    }
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
    if (editing) return false;
    bool lit = true;
    for(int row=0; row<this->board.rows; row++){
        for(int col=0; col<this->board.cols; col++){
            node* currentnode = getnodeat(row, col);
            if (currentnode->type == bulb && currentnode->powered == false) lit = false;
        }
    }
    return lit;
}

node* game::getnodeat(int row, int col){
    return &this->board.nodes[row*this->board.cols + col];
}

void game::resetMoveCount() {
    this->moveCount = 0;
}

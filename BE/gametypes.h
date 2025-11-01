#ifndef GAMETYPES_H
#define GAMETYPES_H
#pragma once
#include <vector>

enum nodetype {
    empty,
    bulb,
    link,
    power
};

enum side {
    up,
    right,
    down,
    left
};

struct position {
    int row;
    int col;
};

struct node {
    nodetype type;
    bool sides[4];
    int rotation; // number from 0-3
    bool powered;
};

struct gameboard {
    int rows;
    int cols;
    std::vector<node> nodes;
    int powerrow;
    int powercol;
};

#endif // GAMETYPES_H

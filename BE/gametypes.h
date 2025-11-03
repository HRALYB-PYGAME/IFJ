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

enum nodeshape {
    o, // bez zadnych dratu
    d, // s jednim dratem nahoru
    i, // s dratem nahoru a dolu
    l, // s draty nahoru a doprava
    t, // s draty nahoru, doprava a dolu
    x  // vsude
};

enum imagename {
    Empty,
    LinkD,
    LinkDpowered,
    LinkI,
    LinkIpowered,
    LinkL,
    LinkLpowered,
    LinkT,
    LinkTpowered,
    LinkX,
    LinkXpowered,
    Bulb,
    Bulbpowered,
    Source
};

struct position {
    int row;
    int col;
};

struct node {
    nodetype type;
    bool sides[4];
    nodeshape shape;
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

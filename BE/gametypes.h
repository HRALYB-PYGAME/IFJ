#ifndef GAMETYPES_H
#define GAMETYPES_H
#pragma once
#include <vector>
#include <array>

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
    left,
    none
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
    std::array<bool, 4> sides;
    nodeshape shape;
    int rotation; // number from 0-3
    bool powered;
};

struct gameboard {
    int rows;
    int cols;
    std::vector<node> nodes;
    int powerrow = -1;
    int powercol = -1;
};

#endif // GAMETYPES_H

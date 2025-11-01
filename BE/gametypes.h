#ifndef GAMETYPES_H
#define GAMETYPES_H
#pragma once
#include <vector>

enum nodetype {
    bulb,
    link_I,
    link_L,
    link_T,
    link_X,
    source,
    source_I,
    source_L,
    source_T,
    source_X
};

struct node {
    nodetype type;
    int rotation; // number from 0-3
    bool powered;
};

struct gameboard {
    int rows;
    int cols;
    std::vector<node> nodes;
};

#endif // GAMETYPES_H

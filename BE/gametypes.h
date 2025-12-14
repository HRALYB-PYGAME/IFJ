#ifndef GAMETYPES_H
#define GAMETYPES_H
#pragma once
#include <vector>
#include <array>

/**
 * @brief Typ herního pole (žárovka, zdroj, drát a nebo prázdný)
 * @author Matyáš Hebert
 */
enum nodetype {
    empty,
    bulb,
    link,
    power
};

/**
 * @brief Strana
 * @author Matyáš Hebert
 */
enum side {
    up,
    right,
    down,
    left,
    none
};

/**
 * @brief Jazyk
 * @author Matyáš Hebert
 */
enum language {
    czech,
    english
};

/**
 * @brief Tvar
 * @author Matyáš Hebert
 */
enum nodeshape {
    o, // bez zadnych dratu
    d, // s jednim dratem nahoru
    i, // s dratem nahoru a dolu
    l, // s draty nahoru a doprava
    t, // s draty nahoru, doprava a dolu
    x  // vsude
};

/**
 * @brief Názvy obrázků
 * @author Matyáš Hebert
 */
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

/**
 * @brief Pozice
 * @author Matyáš Hebert
 */
struct position {
    int row;
    int col;
};

/**
 * @brief Herní pole
 * @author Matyáš Hebert
 */
struct node {
    nodetype type;
    std::array<bool, 4> sides;
    nodeshape shape;
    int rotation; // number from 0-3
    bool powered;
};

/**
 * @brief Herní plán
 * @author Matyáš Hebert
 */
struct gameboard {
    int rows;
    int cols;
    std::vector<node> nodes;
    int powerrow = -1;
    int powercol = -1;
};

#endif // GAMETYPES_H

/*
 * Student Name: [Your Name]
 * Student ID: [Your ID]
 * Assignment: Tetris Game Implementation - FALL 2022 BS(CS)
 * File: utils.cpp
 * Description: Implementation of global variables and utility functions
 */

#include "utils.h"

// Define global variables
int gameGrid[M][N] = {0};
int point_1[4][2], point_2[4][2];

//---Check Uncertain Conditions (Collision Detection)---//
bool anamoly() {
    for (int i = 0; i < 4; i++) {
        // Check if piece is out of bounds
        if (point_1[i][0] < 0 || point_1[i][0] >= N || point_1[i][1] >= M)
            return false;
        // Check if piece collides with existing blocks
        if (gameGrid[point_1[i][1]][point_1[i][0]] != 0)
            return false;
    }
    return true;
}

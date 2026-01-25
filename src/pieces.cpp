/*
 * Student Name: [Your Name]
 * Student ID: [Your ID]
 * Assignment: Tetris Game Implementation - FALL 2022 BS(CS)
 * File: pieces.cpp
 * Description: Implementation of tetromino pieces with all 7 shapes
 */

#include <cstdlib>
#include "pieces.h"

// BLOCKS array - 7 tetromino pieces
// Each piece has 4 coordinates encoded as: row*2 + col
// Pieces: I (light blue), J (dark blue), L (orange), O (yellow), S (green), Z (red), T (magenta)
int BLOCKS[7][4] = {
    // I piece - vertical line (light blue)
    {1, 3, 5, 7},    // positions: (0,1), (1,1), (2,1), (3,1)
    
    // J piece - (dark blue)
    {2, 4, 5, 7},    // positions: (1,0), (2,0), (2,1), (3,1)
    
    // L piece - (orange)
    {3, 5, 4, 6},    // positions: (1,1), (2,1), (2,0), (3,0)
    
    // O piece - square (yellow)
    {2, 3, 4, 5},    // positions: (1,0), (1,1), (2,0), (2,1)
    
    // S piece - (green)
    {3, 5, 4, 6},    // positions: (1,1), (2,1), (2,0), (3,0)
    
    // Z piece - (red)
    {2, 4, 5, 7},    // positions: (1,0), (2,0), (2,1), (3,1)
    
    // T piece - (magenta)
    {3, 4, 5, 7}     // positions: (1,1), (2,0), (2,1), (3,1)
};

// Initialize pieces (called at game start)
void initializePieces() {
    // Pieces are already initialized in the BLOCKS array
    // This function can be used for any additional initialization if needed
}

// Get a random piece number
// In beginner mode: returns 0-3 (I, J, L, O)
// In advanced mode: returns 0-6 (all 7 pieces)
int getRandomPiece(bool isBeginnerMode) {
    if (isBeginnerMode) {
        // Beginner mode: only 4 pieces (I, J, L, O)
        return rand() % 4;
    } else {
        // Advanced mode: all 7 pieces
        return rand() % 7;
    }
}

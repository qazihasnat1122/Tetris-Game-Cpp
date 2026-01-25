
#ifndef PIECES_H
#define PIECES_H

// BLOCKS array - 7 pieces, each with 4 coordinates
// Each piece is defined as 4 points, where each point is encoded as: row*2 + col
extern int BLOCKS[7][4];

// Function to initialize random piece generation
void initializePieces();

// Function to get a random piece number (0-6 for advanced, 0-3 for beginner)
int getRandomPiece(bool isBeginnerMode);

#endif // PIECES_H

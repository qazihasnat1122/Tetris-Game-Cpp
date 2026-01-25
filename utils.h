

#ifndef UTILS_H
#define UTILS_H


const char title[] = "Tetris Game ";

//---Height and Width of the Actual Interactive Game---//
const int M = 20;   // Number of rows for a piece to cover on the screen
const int N = 10;   // Number of columns for a piece to cover on the screen

//---The Actual Interactive Game Grid - Built Over (MxN)---//
extern int gameGrid[M][N];

//---To Hold the Coordinates of the Piece---//
extern int point_1[4][2], point_2[4][2];

//---Check Uncertain Conditions (Collision Detection)---//
bool anamoly();

#endif // UTILS_H
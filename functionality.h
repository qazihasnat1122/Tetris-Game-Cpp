
#include <SFML/Graphics.hpp>
#include <string>

// Global game state variables
extern int score;
extern int linesCleared;
extern bool gameOver;
extern bool isBeginnerMode;
extern float gameStartTime;
extern int frozenRows;

//---Piece Starts to Fall When Game Starts---//
void fallingPiece(float& timer, float& delay, int& colorNum);

//---Move piece horizontally (left/right)---//
void movePiece(int delta_x);

//---Rotate piece---//
void rotatePiece();

//---Check for line completion and clear lines---//
void checkLines();

//---Calculate shadow position (where piece will land)---//
void calculateShadow(int shadow[4][2]);

//---Update difficulty (freeze bottom row every 5 minutes)---//
void updateDifficulty(float currentTime);

//---Reset game state for new game---//
void resetGame();

//---Get current drop delay based on level---//
float getDropDelay();

// Menu and UI functions
int showMainMenu(sf::RenderWindow& window, sf::Font& font);
int showPauseMenu(sf::RenderWindow& window, sf::Font& font);
void showHelp(sf::RenderWindow& window, sf::Font& font);
bool selectLevel(sf::RenderWindow& window, sf::Font& font);
void displayHighScores(sf::RenderWindow& window, sf::Font& font);
void drawGameInfo(sf::RenderWindow& window, sf::Font& font);

// High score functions
void addHighScore(const std::string& name, int score);

/////////////////////////////////////////////
///*** START CODING YOUR FUNTIONS HERE ***///
// Add any additional helper functions you need


///*** YOUR FUNCTIONS END HERE ***///
/////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <time.h>
#include <sstream>
#include "utils.h"
#include "pieces.h"
#include "functionality.h"

using namespace sf;

int main() {
    srand(time(0));
    
    // Create window with tighter size to fit frame better
    RenderWindow window(VideoMode({480, 450}), title);
    
    // Load textures from assets/img directory
    Texture textureTiles, textureBackground, textureFrame;
    textureTiles.loadFromFile("assets/img/tiles.png");
    textureBackground.loadFromFile("assets/img/background.jpg");
    textureFrame.loadFromFile("assets/img/frame.png");
    
    // Create sprites
    Sprite spriteTiles(textureTiles);
    Sprite spriteBackground(textureBackground);
    // restrict background to frame area
    spriteBackground.setTextureRect(IntRect({0, 0}, {250, 450}));
    Sprite spriteFrame(textureFrame);
    
    // Load font for text display
    Font font;
    font.openFromFile("C:/Windows/Fonts/arial.ttf");
    
    // Initialize pieces
    initializePieces();
    
    // Game variables
    int delta_x = 0;
    int colorNum = 1;
    float timer = 0;
    float delay = 0.3;
    bool rotate = false;
    Clock clock;
    Clock gameTimeClock;
    bool inGame = false;
    bool isPaused = false;
    
    ///////////////////////////////////////////////
    ///*** MAIN MENU LOOP ***///
    ///////////////////////////////////////////////
    
    while (window.isOpen()) {
        // Show game menu
        int menuChoice = showMainMenu(window, font);
        
        if (menuChoice == 1) {
            // Start new game and select level first
            isBeginnerMode = selectLevel(window, font);
            
            //  for reset game
            resetGame();
            delay = getDropDelay();
            
            // initialize first piece
            int n = getRandomPiece(isBeginnerMode);
            for (int i = 0; i < 4; i++) {
                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;
            }
            colorNum = 1 + rand() % 7;
            
            inGame = true;
            isPaused = false;
            gameTimeClock.restart();
            
            ///////////////////////////////////////////////
            ///*** GAME LOOP ***///
            ///////////////////////////////////////////////
            
            while (inGame && window.isOpen()) {
                float time = clock.getElapsedTime().asSeconds();
                clock.restart();
                timer += time;
                
                // Update game time for difficulty
                if (!isPaused) {
                    gameStartTime = gameTimeClock.getElapsedTime().asSeconds();
                    updateDifficulty(gameStartTime);
                }
                
                //---Event Listening Part (SFML 3.x style)---//
                while (auto event = window.pollEvent()) {
                    if (event->is<Event::Closed>()) {
                        window.close();
                        return 0;
                    }
                    
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        // Pause key
                        if (keyPressed->code == Keyboard::Key::P && !gameOver) {
                            isPaused = true;
                            int pauseChoice = showPauseMenu(window, font);
                            
                            if (pauseChoice == 1) {
                                inGame = false; // Start new game
                            } else if (pauseChoice == 2) {
                                displayHighScores(window, font);
                            } else if (pauseChoice == 3) {
                                showHelp(window, font);
                            } else if (pauseChoice == 4) {
                                window.close();
                                return 0;
                            } else if (pauseChoice == 5) {
                                isPaused = false; // Continue
                            }
                        }
                        
                        if (!isPaused && !gameOver) {
                            if (keyPressed->code == Keyboard::Key::Up) {
                                rotate = true;
                            } else if (keyPressed->code == Keyboard::Key::Left) {
                                delta_x = -1;
                            } else if (keyPressed->code == Keyboard::Key::Right) {
                                delta_x = 1;
                            }
                        }
                        
                        // Game over - press enter to return to menu
                        if (gameOver && keyPressed->code == Keyboard::Key::Enter) {
                            addHighScore("Player", score);
                            inGame = false;
                        }
                    }
                }
                
                // Fast drop with down arrow
                if (!isPaused && !gameOver) {
                    if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
                        delay = 0.05;
                    } else {
                        delay = getDropDelay();
                    }
                }
                
                ///////////////////////////////////////////////
                ///*** START CALLING YOUR FUNCTIONS HERE ***///
                
                if (!isPaused && !gameOver) {
                    // Move piece horizontally
                    if (delta_x != 0) {
                        movePiece(delta_x);
                        delta_x = 0;
                    }
                    
                    // Rotate piece
                    if (rotate) {
                        rotatePiece();
                        rotate = false;
                    }
                    
                    // Falling piece
                    fallingPiece(timer, delay, colorNum);
                }
                
                ///*** YOUR CALLING STATEMENTS END HERE ***///
                //////////////////////////////////////////////
                
                // Drawing
                window.clear(Color::Black);
                window.draw(spriteBackground);
                
                // Draw game grid
                for (int i = 0; i < M; i++) {
                    for (int j = 0; j < N; j++) {
                        if (gameGrid[i][j] == 0)
                            continue;
                        spriteTiles.setTextureRect(IntRect({gameGrid[i][j] * 18, 0}, {18, 18}));
                        spriteTiles.setPosition(Vector2f(j * 18.f, i * 18.f));
                        spriteTiles.move(Vector2f(28.f, 31.f)); // Offset
                        window.draw(spriteTiles);
                    }
                }
                
                // Draw shadow (projection of where piece will land)
                if (!gameOver) {
                    int shadow[4][2];
                    calculateShadow(shadow);
                    for (int i = 0; i < 4; i++) {
                        RectangleShape shadowBlock(Vector2f(18.f, 18.f));
                        shadowBlock.setPosition(Vector2f(shadow[i][0] * 18.f + 28.f, shadow[i][1] * 18.f + 31.f));
                        shadowBlock.setFillColor(Color(100, 100, 100, 100));
                        shadowBlock.setOutlineColor(Color(150, 150, 150, 150));
                        shadowBlock.setOutlineThickness(1);
                        window.draw(shadowBlock);
                    }
                }
                
                // Draw current piece
                if (!gameOver) {
                    for (int i = 0; i < 4; i++) {
                        spriteTiles.setTextureRect(IntRect({colorNum * 18, 0}, {18, 18}));
                        spriteTiles.setPosition(Vector2f(point_1[i][0] * 18.f, point_1[i][1] * 18.f));
                        spriteTiles.move(Vector2f(28.f, 31.f));
                        window.draw(spriteTiles);
                    }
                }
                
                // Draw frame
                window.draw(spriteFrame);
                
                // Draw game info (score, level, lines)
                drawGameInfo(window, font);
                
                // Draw pause indicator
                if (isPaused) {
                    Text pauseText(font);
                    pauseText.setString("PAUSED - Press P");
                    pauseText.setCharacterSize(20);
                    pauseText.setFillColor(Color::Red);
                    pauseText.setPosition(Vector2f(220.f, 250.f));
                    window.draw(pauseText);
                }
                
                // Game over display
                if (gameOver) {
                    Text gameOverText(font);
                    gameOverText.setString("GAME OVER!");
                    gameOverText.setCharacterSize(28);
                    gameOverText.setFillColor(Color::Red);
                    gameOverText.setPosition(Vector2f(200.f, 250.f));
                    window.draw(gameOverText);
                    
                    std::ostringstream oss;
                    oss << "Score: " << score;
                    Text scoreText(font);
                    scoreText.setString(oss.str());
                    scoreText.setCharacterSize(20);
                    scoreText.setFillColor(Color::Yellow);
                    scoreText.setPosition(Vector2f(210.f, 290.f));
                    window.draw(scoreText);
                    
                    Text enterText(font);
                    enterText.setString("Press ENTER");
                    enterText.setCharacterSize(18);
                    enterText.setFillColor(Color::White);
                    enterText.setPosition(Vector2f(200.f, 330.f));
                    window.draw(enterText);
                }
                
                // Display window
                window.display();
            }
            
        } else if (menuChoice == 2) {
            // High scores
            displayHighScores(window, font);
            
        } else if (menuChoice == 3) {
            // Help
            showHelp(window, font);
            
        } else if (menuChoice == 4) {
            // Exit
            window.close();
        }
    }
    
    return 0;
}

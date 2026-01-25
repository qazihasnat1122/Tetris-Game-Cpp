

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "functionality.h"
#include "utils.h"
#include "pieces.h"

// Global game state variables
int score = 0;
int linesCleared = 0;
bool gameOver = false;
bool isBeginnerMode = true;
float gameStartTime = 0;
int frozenRows = 0;

// High Score structure
struct HighScore {
    std::string name;
    int score;
};

//---Helper function to draw text---//
void drawText(sf::RenderWindow& window, const std::string& text, int x, int y, 
              int size, sf::Color color, sf::Font& font) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition(sf::Vector2f(x, y));
    window.draw(sfText);
}

//---Piece Starts to Fall When Game Starts---//
void fallingPiece(float& timer, float& delay, int& colorNum) {
    if (timer > delay) {
        // Save current position to point_2
        for (int i = 0; i < 4; i++) {
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1; // Move piece down by 1 unit
        }
        
        // Check if the new position is valid
        if (!anamoly()) {
            // Position is invalid, lock the piece
            for (int i = 0; i < 4; i++) {
                gameGrid[point_2[i][1]][point_2[i][0]] = colorNum;
            }
            
            // Check for completed lines
            checkLines();
            
            // Generate new random piece
            int n = getRandomPiece(isBeginnerMode);
            
            // Set new piece position at top center
            for (int i = 0; i < 4; i++) {
                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;
            }
            
            // Generate random color (1-7 for different colors)
            colorNum = 1 + rand() % 7;
            
            // Check if game is over (new piece collides immediately)
            if (!anamoly()) {
                gameOver = true;
            }
        }
        
        timer = 0; // Reset timer
    }
}

//---Move piece horizontally---//
void movePiece(int delta_x) {
    // Save current position
    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
    }
    
    // Try to move piece
    for (int i = 0; i < 4; i++) {
        point_1[i][0] += delta_x;
    }
    
    // Check if new position is valid
    if (!anamoly()) {
        // Invalid position, restore previous position
        for (int i = 0; i < 4; i++) {
            point_1[i][0] = point_2[i][0];
            point_1[i][1] = point_2[i][1];
        }
    }
}

//---Rotate piece---//
void rotatePiece() {
    // Save current position
    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
    }
    
    // Calculate center point (pivot) - use second block as reference
    int centerX = point_1[1][0];
    int centerY = point_1[1][1];
    
    // Rotate each block 90 degrees clockwise around center
    for (int i = 0; i < 4; i++) {
        int x = point_1[i][1] - centerY; // Relative to center
        int y = point_1[i][0] - centerX;
        point_1[i][0] = centerX - x; // Rotate
        point_1[i][1] = centerY + y;
    }
    
    // Check if new position is valid
    if (!anamoly()) {
        // Invalid position, restore previous position
        for (int i = 0; i < 4; i++) {
            point_1[i][0] = point_2[i][0];
            point_1[i][1] = point_2[i][1];
        }
    }
}

//---Check for completed lines and clear them---//
void checkLines() {
    int completedLines = 0;
    
    // Check each row from bottom to top (excluding frozen rows)
    for (int i = M - 1; i >= 0; i--) {
        // Skip frozen rows at the bottom
        if (i >= M - frozenRows) {
            continue;
        }
        
        // Check if row is complete
        bool isComplete = true;
        for (int j = 0; j < N; j++) {
            if (gameGrid[i][j] == 0) {
                isComplete = false;
                break;
            }
        }
        
        // If row is complete, clear it
        if (isComplete) {
            completedLines++;
            
            // Move all rows above down by one
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < N; j++) {
                    gameGrid[k][j] = gameGrid[k - 1][j];
                }
            }
            
            // Clear top row
            for (int j = 0; j < N; j++) {
                gameGrid[0][j] = 0;
            }
            
            // Check the same row again (since rows moved down)
            i++;
        }
    }
    
    // Update score if lines were cleared
    if (completedLines > 0) {
        int levelMultiplier = (isBeginnerMode) ? 1 : 2;
        
        switch (completedLines) {
            case 1:
                score += 10 * levelMultiplier;
                break;
            case 2:
                score += 30 * levelMultiplier;
                break;
            case 3:
                score += 60 * levelMultiplier;
                break;
            case 4:
                score += 100 * levelMultiplier;
                break;
            default:
                score += completedLines * 10 * levelMultiplier;
                break;
        }
        
        linesCleared += completedLines;
    }
}

//---Calculate shadow position (where piece will land)---//
void calculateShadow(int shadow[4][2]) {
    // Copy current piece position to shadow
    for (int i = 0; i < 4; i++) {
        shadow[i][0] = point_1[i][0];
        shadow[i][1] = point_1[i][1];
    }
    
    // Move shadow down until it collides
    bool canMoveDown = true;
    while (canMoveDown) {
        // Try to move shadow down
        for (int i = 0; i < 4; i++) {
            shadow[i][1] += 1;
        }
        
        // Check if shadow position is valid
        bool valid = true;
        for (int i = 0; i < 4; i++) {
            // Check boundaries
            if (shadow[i][0] < 0 || shadow[i][0] >= N || shadow[i][1] >= M) {
                valid = false;
                break;
            }
            // Check collision with existing blocks
            if (gameGrid[shadow[i][1]][shadow[i][0]] != 0) {
                valid = false;
                break;
            }
        }
        
        if (!valid) {
            // Move shadow back up one position
            for (int i = 0; i < 4; i++) {
                shadow[i][1] -= 1;
            }
            canMoveDown = false;
        }
    }
}

//---Update difficulty (freeze bottom row every 5 minutes)---//
void updateDifficulty(float currentTime) {
    // Every 5 minutes (300 seconds), freeze one more row from bottom
    int newFrozenRows = (int)(currentTime / 300.0);
    
    if (newFrozenRows > frozenRows && newFrozenRows < M - 5) {
        frozenRows = newFrozenRows;
        
        // Fill the newly frozen row with random blocks
        int rowToFreeze = M - frozenRows;
        for (int j = 0; j < N; j++) {
            if (gameGrid[rowToFreeze][j] == 0) {
                gameGrid[rowToFreeze][j] = 1 + rand() % 7;
            }
        }
    }
}

//---Reset game state---//
void resetGame() {
    // Clear game grid
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            gameGrid[i][j] = 0;
        }
    }
    
    // Reset game state
    score = 0;
    linesCleared = 0;
    gameOver = false;
    frozenRows = 0;
    gameStartTime = 0;
}

//---Get drop delay based on level---//
float getDropDelay() {
    if (isBeginnerMode) {
        return 0.5; // Slower for beginner
    } else {
        return 0.3; // Faster for advanced
    }
}

//---Load high scores from file---//
void loadHighScores(std::vector<HighScore>& scores) {
    scores.clear();
    std::ifstream file("highscores.txt");
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line) && scores.size() < 10) {
            std::istringstream iss(line);
            HighScore hs;
            if (std::getline(iss, hs.name, ',') && iss >> hs.score) {
                scores.push_back(hs);
            }
        }
        file.close();
    }
}

//---Save high scores to file---//
void saveHighScores(const std::vector<HighScore>& scores) {
    std::ofstream file("highscores.txt");
    
    if (file.is_open()) {
        // Save top 10 scores
        int count = 0;
        for (const auto& hs : scores) {
            if (count >= 10) break;
            file << hs.name << "," << hs.score << "\n";
            count++;
        }
        file.close();
    }
}

//---Add a new high score---//
void addHighScore(const std::string& name, int score) {
    std::vector<HighScore> scores;
    loadHighScores(scores);
    
    // Add new score
    HighScore newScore;
    newScore.name = name;
    newScore.score = score;
    scores.push_back(newScore);
    
    // Sort scores in descending order
    std::sort(scores.begin(), scores.end(), 
              [](const HighScore& a, const HighScore& b) {
                  return a.score > b.score;
              });
    
    // Keep only top 10
    if (scores.size() > 10) {
        scores.resize(10);
    }
    
    saveHighScores(scores);
}

//---Display high scores screen---//
void displayHighScores(sf::RenderWindow& window, sf::Font& font) {
    std::vector<HighScore> scores;
    loadHighScores(scores);
    
    bool viewing = true;
    while (viewing && window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape || 
                    keyPressed->code == sf::Keyboard::Key::Enter) {
                    viewing = false;
                }
            }
        }
        
        window.clear(sf::Color::Black);
        
        // Title
        drawText(window, "HIGH SCORES", 80, 50, 30, sf::Color::Yellow, font);
        
        // Display scores
        int y = 120;
        for (size_t i = 0; i < scores.size() && i < 10; i++) {
            std::ostringstream oss;
            oss << (i + 1) << ". " << scores[i].name << " - " << scores[i].score;
            drawText(window, oss.str(), 60, y, 20, sf::Color::White, font);
            y += 30;
        }
        
        // Instructions
        drawText(window, "Press ESC or ENTER to return", 40, 450, 16, 
                 sf::Color::Green, font);
        
        window.display();
    }
}

//---Show main menu---//
int showMainMenu(sf::RenderWindow& window, sf::Font& font) {
    int selectedOption = 1;
    bool menuActive = true;
    
    while (menuActive && window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 4; // Exit
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Up) {
                    selectedOption--;
                    if (selectedOption < 1) selectedOption = 4;
                } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                    selectedOption++;
                    if (selectedOption > 4) selectedOption = 1;
                } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    return selectedOption;
                }
            }
        }
        
        window.clear(sf::Color::Black);
        
        // Title
        drawText(window, "TETRIS GAME", 80, 80, 35, sf::Color::Cyan, font);
        
        // Menu options
        sf::Color color1 = (selectedOption == 1) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color2 = (selectedOption == 2) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color3 = (selectedOption == 3) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color4 = (selectedOption == 4) ? sf::Color::Yellow : sf::Color::White;
        
        drawText(window, "1. Start New Game", 80, 200, 24, color1, font);
        drawText(window, "2. High Scores", 80, 250, 24, color2, font);
        drawText(window, "3. Help", 80, 300, 24, color3, font);
        drawText(window, "4. Exit", 80, 350, 24, color4, font);
        
        // Instructions
        drawText(window, "Use UP/DOWN arrows and ENTER", 40, 420, 16, 
                 sf::Color::Green, font);
        
        window.display();
    }
    
    return 4; // Exit if window closed
}

//---Show pause menu---//
int showPauseMenu(sf::RenderWindow& window, sf::Font& font) {
    int selectedOption = 5;
    bool menuActive = true;
    
    while (menuActive && window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 4; // Exit
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Up) {
                    selectedOption--;
                    if (selectedOption < 1) selectedOption = 5;
                } else if (keyPressed->code == sf::Keyboard::Key::Down) {
                    selectedOption++;
                    if (selectedOption > 5) selectedOption = 1;
                } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    return selectedOption;
                } else if (keyPressed->code == sf::Keyboard::Key::P) {
                    return 5; // Continue
                }
            }
        }
        
        window.clear(sf::Color(0, 0, 0, 200));
        
        // Title
        drawText(window, "PAUSED", 120, 80, 35, sf::Color::Red, font);
        
        // Menu options
        sf::Color color1 = (selectedOption == 1) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color2 = (selectedOption == 2) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color3 = (selectedOption == 3) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color4 = (selectedOption == 4) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color5 = (selectedOption == 5) ? sf::Color::Yellow : sf::Color::White;
        
        drawText(window, "1. Start New Game", 70, 180, 22, color1, font);
        drawText(window, "2. High Scores", 70, 220, 22, color2, font);
        drawText(window, "3. Help", 70, 260, 22, color3, font);
        drawText(window, "4. Exit", 70, 300, 22, color4, font);
        drawText(window, "5. Continue", 70, 340, 22, color5, font);
        
        // Instructions
        drawText(window, "Press P to continue", 70, 400, 16, 
                 sf::Color::Green, font);
        
        window.display();
    }
    
    return 4; // Exit if window closed
}

//---Show help screen---//
void showHelp(sf::RenderWindow& window, sf::Font& font) {
    bool viewing = true;
    
    while (viewing && window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape || 
                    keyPressed->code == sf::Keyboard::Key::Enter) {
                    viewing = false;
                }
            }
        }
        
        window.clear(sf::Color::Black);
        
        // Title
        drawText(window, "HOW TO PLAY", 80, 30, 28, sf::Color::Cyan, font);
        
        // Rules
        drawText(window, "RULES:", 30, 80, 22, sf::Color::Yellow, font);
        drawText(window, "- Stack falling blocks to clear lines", 30, 110, 18, 
                 sf::Color::White, font);
        drawText(window, "- Clear lines to score points", 30, 135, 18, 
                 sf::Color::White, font);
        drawText(window, "- Game ends when blocks reach top", 30, 160, 18, 
                 sf::Color::White, font);
        
        // Controls
        drawText(window, "CONTROLS:", 30, 200, 22, sf::Color::Yellow, font);
        drawText(window, "LEFT ARROW  - Move left", 30, 230, 18, sf::Color::White, font);
        drawText(window, "RIGHT ARROW - Move right", 30, 255, 18, sf::Color::White, font);
        drawText(window, "UP ARROW    - Rotate piece", 30, 280, 18, sf::Color::White, font);
        drawText(window, "DOWN ARROW  - Fast drop", 30, 305, 18, sf::Color::White, font);
        drawText(window, "P           - Pause game", 30, 330, 18, sf::Color::White, font);
        
        // Scoring
        drawText(window, "SCORING:", 30, 370, 22, sf::Color::Yellow, font);
        drawText(window, "1 line: 10 x level", 30, 400, 18, sf::Color::White, font);
        drawText(window, "2 lines: 30 x level", 30, 420, 18, sf::Color::White, font);
        drawText(window, "3 lines: 60 x level", 30, 440, 18, sf::Color::White, font);
        drawText(window, "4 lines: 100 x level", 30, 460, 18, sf::Color::White, font);
        
        // Back instruction
        drawText(window, "Press ESC or ENTER to return", 40, 500, 16, 
                 sf::Color::Green, font);
        
        window.display();
    }
}

//---Select game level---//
bool selectLevel(sf::RenderWindow& window, sf::Font& font) {
    int selectedLevel = 1;
    bool selecting = true;
    
    while (selecting && window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return true; // Default to beginner
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::Down) {
                    selectedLevel = (selectedLevel == 1) ? 2 : 1;
                } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    return (selectedLevel == 1);
                }
            }
        }
        
        window.clear(sf::Color::Black);
        
        // Title
        drawText(window, "SELECT LEVEL", 80, 100, 30, sf::Color::Cyan, font);
        
        // Level options
        sf::Color color1 = (selectedLevel == 1) ? sf::Color::Yellow : sf::Color::White;
        sf::Color color2 = (selectedLevel == 2) ? sf::Color::Yellow : sf::Color::White;
        
        drawText(window, "1. Beginner", 100, 200, 24, color1, font);
        drawText(window, "(4 pieces, slower)", 80, 230, 18, color1, font);
        
        drawText(window, "2. Advanced", 100, 300, 24, color2, font);
        drawText(window, "(7 pieces, faster)", 80, 330, 18, color2, font);
        
        // Instructions
        drawText(window, "Use UP/DOWN and ENTER", 60, 420, 16, 
                 sf::Color::Green, font);
        
        window.display();
    }
    
    return true; // Default to beginner
}

//---Draw game information (score, level, lines)---//
void drawGameInfo(sf::RenderWindow& window, sf::Font& font) {
    std::ostringstream oss;
    
    // Score
    oss << "Score: " << score;
    drawText(window, oss.str(), 300, 100, 18, sf::Color::White, font);
    
    // Level
    oss.str("");
    oss << "Level: " << (isBeginnerMode ? "Beginner" : "Advanced");
    drawText(window, oss.str(), 300, 140, 18, sf::Color::White, font);
    
    // Lines cleared
    oss.str("");
    oss << "Lines: " << linesCleared;
    drawText(window, oss.str(), 300, 180, 18, sf::Color::White, font);
}

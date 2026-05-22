#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>

const int WINDOW_SIZE = 600;
const int CELL_SIZE = WINDOW_SIZE / 3;

class Board {
private:
    std::vector<std::vector<char>> grid;

public:
    Board() : grid(3, std::vector<char>(3, ' ')) {}

    char getCell(int r, int c) const { return grid[r][c]; }

    bool makeMove(int row, int col, char symbol) {
        if (row >= 0 && row < 3 && col >= 0 && col < 3 && grid[row][col] == ' ') {
            grid[row][col] = symbol;
            return true;
        }
        return false;
    }

    void undoMove(int row, int col) {
        grid[row][col] = ' ';
    }

    bool checkWin(char symbol) {
        for (int i = 0; i < 3; ++i) {
            if ((grid[i][0] == symbol && grid[i][1] == symbol && grid[i][2] == symbol) ||
                (grid[0][i] == symbol && grid[1][i] == symbol && grid[2][i] == symbol))
                return true;
        }
        if ((grid[0][0] == symbol && grid[1][1] == symbol && grid[2][2] == symbol) ||
            (grid[0][2] == symbol && grid[1][1] == symbol && grid[2][0] == symbol))
            return true;

        return false;
    }

    bool isFull() {
        for (const auto& row : grid) {
            for (char cell : row) {
                if (cell == ' ') return false;
            }
        }
        return true;
    }

    void reset() {
        grid = std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
    }
};

class AIPlayer {
private:
    char aiSymbol;
    char humanSymbol;

    int minimax(Board& board, int depth, bool isMax) {
        if (board.checkWin(aiSymbol)) return 10 - depth;
        if (board.checkWin(humanSymbol)) return depth - 10;
        if (board.isFull()) return 0;

        if (isMax) {
            int bestScore = -1000;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (board.makeMove(i, j, aiSymbol)) {
                        int score = minimax(board, depth + 1, false);
                        board.undoMove(i, j);
                        bestScore = std::max(score, bestScore);
                    }
                }
            }
            return bestScore;
        } else {
            int bestScore = 1000;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (board.makeMove(i, j, humanSymbol)) {
                        int score = minimax(board, depth + 1, true);
                        board.undoMove(i, j);
                        bestScore = std::min(score, bestScore);
                    }
                }
            }
            return bestScore;
        }
    }

public:
    AIPlayer(char aiSym, char humanSym) : aiSymbol(aiSym), humanSymbol(humanSym) {}

    std::pair<int, int> getBestMove(Board& board) {
        int bestScore = -1000;
        std::pair<int, int> bestMove = {-1, -1};

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board.makeMove(i, j, aiSymbol)) {
                    int score = minimax(board, 0, false);
                    board.undoMove(i, j);
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = {i, j};
                    }
                }
            }
        }
        return bestMove;
    }
};

class GameGUI {
private:
    sf::RenderWindow window;
    Board board;
    AIPlayer ai;
    char humanSymbol;
    char aiSymbol;
    bool isHumanTurn;
    bool gameOver;

    void drawGrid() {
        
        window.clear(sf::Color(44, 62, 80)); 

       
        sf::RectangleShape line(sf::Vector2f(WINDOW_SIZE, 5));
        line.setFillColor(sf::Color(52, 73, 94));

       
        line.setPosition(0, CELL_SIZE);
        window.draw(line);
        line.setPosition(0, CELL_SIZE * 2);
        window.draw(line);

        
        line.setSize(sf::Vector2f(5, WINDOW_SIZE));
        line.setPosition(CELL_SIZE, 0);
        window.draw(line);
        line.setPosition(CELL_SIZE * 2, 0);
        window.draw(line);
    }

    void drawMarks() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                char cell = board.getCell(i, j);
                float centerX = j * CELL_SIZE + CELL_SIZE / 2.0f;
                float centerY = i * CELL_SIZE + CELL_SIZE / 2.0f;

                if (cell == 'X') {
                   
                    sf::RectangleShape l1(sf::Vector2f(100, 10));
                    l1.setOrigin(50, 5);
                    l1.setPosition(centerX, centerY);
                    l1.setRotation(45);
                    l1.setFillColor(sf::Color(231, 76, 60)); /

                    sf::RectangleShape l2 = l1;
                    l2.setRotation(-45);

                    window.draw(l1);
                    window.draw(l2);
                } else if (cell == 'O') {
                   
                    sf::CircleShape outerCircle(50);
                    outerCircle.setOrigin(50, 50);
                    outerCircle.setPosition(centerX, centerY);
                    outerCircle.setFillColor(sf::Color(46, 204, 113)); 

                    sf::CircleShape innerCircle(40);
                    innerCircle.setOrigin(40, 40);
                    innerCircle.setPosition(centerX, centerY);
                    innerCircle.setFillColor(sf::Color(44, 62, 80)); 

                    window.draw(outerCircle);
                    window.draw(innerCircle);
                }
            }
        }
    }

public:
    GameGUI() : window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Unbeatable Tic-Tac-Toe AI"),
                ai('O', 'X'), humanSymbol('X'), aiSymbol('O'), isHumanTurn(true), gameOver(false) {}

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && isHumanTurn && !gameOver) {
                    int col = event.mouseButton.x / CELL_SIZE;
                    int row = event.mouseButton.y / CELL_SIZE;

                    if (board.makeMove(row, col, humanSymbol)) {
                        if (board.checkWin(humanSymbol)) {
                            gameOver = true;
                        } else if (board.isFull()) {
                            gameOver = true;
                        } else {
                            isHumanTurn = false; 
                        }
                    }
                }

               
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    board.reset();
                    isHumanTurn = true;
                    gameOver = false;
                }
            }

            
            if (!isHumanTurn && !gameOver) {
                std::pair<int, int> aiMove = ai.getBestMove(board);
                board.makeMove(aiMove.first, aiMove.second, aiSymbol);

                if (board.checkWin(aiSymbol) || board.isFull()) {
                    gameOver = true;
                }
                isHumanTurn = true;
            }

            drawGrid();
            drawMarks();
            window.display();
        }
    }
};

int main() {
    GameGUI game;
    game.run();
    return 0;
}
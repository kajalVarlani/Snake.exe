#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <cctype>
using namespace std;

// Platform-specific headers
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

// ---------- Cross-Platform Utilities ----------
#ifndef _WIN32
int _kbhit() {
    termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void Sleep(int ms) { usleep(ms * 1000); }
void clearScreen() { system("clear"); }
#else
void clearScreen() { system("cls"); }
#endif

// ---------- Colors ----------
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define WHITE "\033[37m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"

// ---------- Classes ----------
class Point {
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point &other) const { return x == other.x && y == other.y; }
};

enum Direction { UP, DOWN, LEFT, RIGHT };

class Food {
    Point position;
    bool active;

public:
    Food() : active(false) {}
    void generate(const deque<Point> &snakeBody, int width, int height) {
        vector<Point> valid;
        for (int i = 1; i < height - 1; i++) {
            for (int j = 1; j < width - 1; j++) {
                Point candidate(i, j);
                bool onSnake = false;
                for (auto &seg : snakeBody) {
                    if (seg == candidate) { onSnake = true; break; }
                }
                if (!onSnake) valid.push_back(candidate);
            }
        }
        if (!valid.empty()) {
            int idx = rand() % valid.size();
            position = valid[idx];
            active = true;
        }
    }
    Point getPos() const { return position; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
};

class Snake {
    deque<Point> body;
    Direction dir;

public:
    Snake(int width, int height) {
        dir = RIGHT;
        int startX = height / 2;
        int startY = width / 2;
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX, startY - 1));
        body.push_back(Point(startX, startY - 2));
    }

    void changeDir(Direction newDir) {
        if ((dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP) ||
            (dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT))
            return;
        dir = newDir;
    }

    bool move(Food &food, int &score, int width, int height) {
        Point head = body.front();
        switch (dir) {
            case UP: head.x--; break;
            case DOWN: head.x++; break;
            case LEFT: head.y--; break;
            case RIGHT: head.y++; break;
        }

        // Wall collision
        if (head.x < 0 || head.x >= height - 1 || head.y < 1 || head.y >= width - 2)
            return false;

        // Self collision
        for (auto &seg : body)
            if (head == seg) return false;

        body.push_front(head);

        if (food.isActive() && head == food.getPos()) {
            score++;
            food.deactivate();
        } else {
            body.pop_back();
        }
        return true;
    }

    const deque<Point> &getBody() const { return body; }
    Point getHead() const { return body.front(); }
    Direction getDir() const { return dir; }
};

class Game {
    int width, height;
    Snake snake;
    Food food;
    int score, highScore;
    bool gameOver;

public:
    Game(int w = 40, int h = 20) : width(w), height(h), snake(w, h) {
        srand(time(0));
        score = 0;
        highScore = 0;
        gameOver = false;
        food.generate(snake.getBody(), width, height);
    }

    void draw() {
        // Move cursor to top-left
#ifdef _WIN32
        COORD coord = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
        cout << "\033[H";
#endif
        string buffer;
        buffer += WHITE;
        buffer += " ";
        for (int j = 0; j < width - 2; j++) buffer += "_";
        buffer += " \n";

        for (int i = 0; i < height - 1; i++) {
            buffer += WHITE;
            buffer += "|";
            for (int j = 0; j < width - 2; j++) {
                Point cur(i, j + 1);
                if (snake.getHead() == cur)
                    buffer += string(GREEN) + "O" + WHITE;
                else {
                    bool isBody = false;
                    for (auto &seg : snake.getBody()) {
                        if (seg == cur && !(seg == snake.getHead())) {
                            buffer += string(GREEN) + "o" + WHITE;
                            isBody = true;
                            break;
                        }
                    }
                    if (!isBody) {
                        if (food.isActive() && cur == food.getPos())
                            buffer += string(RED) + "@" + WHITE;
                        else buffer += " ";
                    }
                }
            }
            buffer += "|\n";
        }

        buffer += " ";
        for (int j = 0; j < width - 2; j++) buffer += "_";
        buffer += " \n";

        string scoreLine = " Score: " + to_string(score) + " | High Score: " + to_string(highScore);
        int pad = max(0, (width - (int)scoreLine.size()) / 2);
        buffer += string(YELLOW) + string(pad, ' ') + scoreLine + "\n" + RESET;

        if (gameOver) {
            string msg = " GAME OVER! Press R to restart or Q to quit ";
            int pad2 = max(0, (width - (int)msg.size()) / 2);
            buffer += string(RED) + string(pad2, ' ') + msg + "\n" + RESET;
        }

        cout << buffer;
    }

    void input() {
        if (_kbhit()) {
            int key = _getch();
            switch (tolower(key)) {
                case 'w': snake.changeDir(UP); break;
                case 's': snake.changeDir(DOWN); break;
                case 'a': snake.changeDir(LEFT); break;
                case 'd': snake.changeDir(RIGHT); break;
                case 'r': if (gameOver) reset(); break;
                case 'q': if (gameOver) exit(0); break;
            }
        }
    }

    void update() {
        if (!gameOver) {
            if (!snake.move(food, score, width, height)) {
                gameOver = true;
                if (score > highScore) highScore = score;
            } else if (!food.isActive()) {
                food.generate(snake.getBody(), width, height);
            }
        }
    }

    void reset() {
        snake = Snake(width, height);
        score = 0;
        food.generate(snake.getBody(), width, height);
        gameOver = false;
    }

    void run() {
        while (true) {
            draw();
            input();
            update();
            int speed = max(40, 150 - score * 5);
            Sleep(speed);
        }
    }
};

// ---------- Title Screen ----------
void showTitleScreen() {
    clearScreen();
    cout << GREEN << R"(

   ____   _   _    ___    _   _   _____     ____        _      __  __   _____ 
  / ___| | \ | |  / _ \  | \ | | | ____|   / ___|      / \    |  \/  | | ____|
  \___ \ |  \| | | | | | |  \| | |  _|     | |  _     / _ \   | |\/| | |  _|  
   ___) || |\  | | |_| | | |\  | | |___    | |_| |   / ___ \  | |  | | | |___ 
  |____/ |_| \_|  \___/  |_| \_| |_____|    \_____|  /_/   \_\ |_|  |_| |_____|
    )" << RESET << endl;

    cout << YELLOW;
    cout << "\n Controls:\n";
    cout << "   W = Move Up\n";
    cout << "   S = Move Down\n";
    cout << "   A = Move Left\n";
    cout << "   D = Move Right\n";
    cout << "   R = Restart (after Game Over)\n";
    cout << "   Q = Quit (after Game Over)\n";
    cout << RESET;

    cout << CYAN << "\nPress any key to start the game..." << RESET << endl;
    _getch();
    clearScreen();
}

int main() {
    showTitleScreen();
    Game game(90, 25);
    game.run();
    return 0;
}

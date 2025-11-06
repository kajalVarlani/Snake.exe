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
int _kbhit()
{
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
    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int _getch()
{
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
#define LIGHT_GREEN "\033[92m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define BOLD "\033[1m"
#define NEON_GREEN "\033[38;5;46m"
#define NEON_CYAN "\033[38;5;51m"
#define NEON_PURPLE "\033[38;5;165m"
#define NEON_YELLOW "\033[38;5;226m"
#define GOLD "\033[38;5;220m"
#define BRIGHT_CYAN "\033[38;5;87m"
#define PINK "\033[38;5;213m"
#define NEON_RED "\033[38;5;196m"
#define DIM_CYAN "\033[38;5;36m"
#define BRIGHT_GREEN "\033[38;5;82m"
#define ORANGE "\033[38;5;208m"

// ---------- Classes ----------
class Point
{
public:
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point &other) const { return x == other.x && y == other.y; }
};

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Food
{
    Point position;
    bool active;

public:
    Food() : active(false) {}
    void generate(const deque<Point> &snakeBody, int width, int height)
    {
        vector<Point> valid;
        for (int i = 1; i < height - 2; i++)
        {
            for (int j = 1; j < width - 2; j++)
            {
                Point candidate(i, j);
                bool onSnake = false;
                for (auto &seg : snakeBody)
                {
                    if (seg == candidate)
                    {
                        onSnake = true;
                        break;
                    }
                }
                if (!onSnake)
                    valid.push_back(candidate);
            }
        }
        if (!valid.empty())
        {
            int idx = rand() % valid.size();
            position = valid[idx];
            active = true;
        }
    }

    Point getPos() const { return position; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
};

class Snake
{
    deque<Point> body;
    Direction dir;

public:
    Snake(int width, int height)
    {
        dir = RIGHT;
        int startX = height / 2;
        int startY = width / 2;
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX, startY - 1));
        body.push_back(Point(startX, startY - 2));
    }

    void changeDir(Direction newDir)
    {
        if ((dir == UP && newDir == DOWN) || (dir == DOWN && newDir == UP) ||
            (dir == LEFT && newDir == RIGHT) || (dir == RIGHT && newDir == LEFT))
            return;
        dir = newDir;
    }

    bool move(Food &food, int &score, int width, int height)
    {
        Point head = body.front();
        switch (dir)
        {
        case UP:
            head.x--;
            break;
        case DOWN:
            head.x++;
            break;
        case LEFT:
            head.y--;
            break;
        case RIGHT:
            head.y++;
            break;
        }

        // Wall collision
        if (head.x < 0 || head.x >= height - 1 || head.y <= 0 || head.y >= width - 1)
            return false;

        // Self collision
        for (auto &seg : body)
            if (head == seg)
                return false;

        body.push_front(head);

        if (food.isActive() && head == food.getPos())
        {
            score++;
            food.deactivate();
        }
        else
        {
            body.pop_back();
        }
        return true;
    }

    const deque<Point> &getBody() const { return body; }
    Point getHead() const { return body.front(); }
};

class Game
{
    int width, height;
    Snake snake;
    Food food;
    int score, highScore;
    bool gameOver;

public:
    Game(int w = 40, int h = 20) : width(w), height(h), snake(w, h)
    {
        srand(time(0));
        score = 0;
        highScore = 0;
        gameOver = false;
        food.generate(snake.getBody(), width, height);
    }

    void draw()
    {
        // Build entire frame in a string buffer first
        string buffer;
        buffer.reserve(10000); // Pre-allocate memory

        // Top border
        buffer += BRIGHT_CYAN;
        buffer += BOLD;
        buffer += " ╔";
        for (int j = 0; j < width - 2; j++)
            buffer += "═";
        buffer += "╗\n";
        buffer += RESET;

        // Game area
        for (int i = 0; i < height - 1; i++)
        {
            buffer += BRIGHT_CYAN;
            buffer += BOLD;
            buffer += " ║";
            buffer += RESET;
            
            for (int j = 0; j < width - 2; j++)
            {
                Point cur(i, j + 1);
                if (snake.getHead() == cur)
                {
                    buffer += BRIGHT_GREEN;
                    buffer += BOLD;
                    buffer += "●";
                    buffer += RESET;
                }
                else
                {
                    bool isBody = false;
                    for (auto &seg : snake.getBody())
                    {
                        if (seg == cur && !(seg == snake.getHead()))
                        {
                            buffer += NEON_GREEN;
                            buffer += "●";
                            buffer += RESET;
                            isBody = true;
                            break;
                        }
                    }
                    if (!isBody)
                    {
                        if (food.isActive() && cur == food.getPos())
                        {
                            buffer += NEON_RED;
                            buffer += BOLD;
                            buffer += "◆";
                            buffer += RESET;
                        }
                        else
                            buffer += " ";
                    }
                }
            }
            buffer += BRIGHT_CYAN;
            buffer += BOLD;
            buffer += "║";
            buffer += RESET;
            buffer += "\n";
        }

        // Bottom border
        buffer += BRIGHT_CYAN;
        buffer += BOLD;
        buffer += " ╚";
        for (int j = 0; j < width - 2; j++)
            buffer += "═";
        buffer += "╝\n";
        buffer += RESET;

        // Score display
        buffer += GOLD;
        buffer += BOLD;
        buffer += " > ";
        buffer += RESET;
        buffer += YELLOW;
        buffer += "Score: ";
        buffer += BOLD;
        buffer += to_string(score);
        buffer += RESET;
        buffer += GOLD;
        buffer += BOLD;
        buffer += "  |  ";
        buffer += RESET;
        buffer += ORANGE;
        buffer += "High Score: ";
        buffer += BOLD;
        buffer += to_string(highScore);
        buffer += RESET;
        buffer += "\n";

        if (gameOver)
        {
            buffer += NEON_RED;
            buffer += BOLD;
            buffer += " > GAME OVER! ";
            buffer += RESET;
            buffer += CYAN;
            buffer += "Press R to restart or Q to quit";
            buffer += RESET;
            buffer += "\n";
        }
        else
        {
            buffer += DIM_CYAN;
            buffer += " > Use WASD or Arrow Keys to move";
            buffer += RESET;
            buffer += "\n";
        }

        // Now output everything at once with cursor reset
#ifdef _WIN32
        COORD coord = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
        cout << "\033[H";
#endif
        cout << buffer << flush;
    }

    void input()
    {
        if (_kbhit())
        {
            int key = _getch();

#ifdef _WIN32
            if (key == 224)
            {
                int arrow = _getch();
                switch (arrow)
                {
                case 72: snake.changeDir(UP); break;
                case 80: snake.changeDir(DOWN); break;
                case 75: snake.changeDir(LEFT); break;
                case 77: snake.changeDir(RIGHT); break;
                }
                return;
            }
#else
            if (key == 27)
            {
                if (_kbhit() && _getch() == 91)
                {
                    int arrow = _getch();
                    switch (arrow)
                    {
                    case 65: snake.changeDir(UP); break;
                    case 66: snake.changeDir(DOWN); break;
                    case 68: snake.changeDir(LEFT); break;
                    case 67: snake.changeDir(RIGHT); break;
                    }
                }
                return;
            }
#endif

            switch (tolower(key))
            {
            case 'w': snake.changeDir(UP); break;
            case 's': snake.changeDir(DOWN); break;
            case 'a': snake.changeDir(LEFT); break;
            case 'd': snake.changeDir(RIGHT); break;
            case 'r':
                if (gameOver) reset();
                break;
            case 'q':
                if (gameOver) exit(0);
                break;
            }
        }
    }

    void update()
    {
        if (!gameOver)
        {
            if (!snake.move(food, score, width, height))
            {
                gameOver = true;
                if (score > highScore)
                    highScore = score;
            }
            else if (!food.isActive())
            {
                food.generate(snake.getBody(), width, height);
            }
        }
    }

    void reset()
    {
        snake = Snake(width, height);
        score = 0;
        food.generate(snake.getBody(), width, height);
        gameOver = false;
    }

    void run()
    {
        while (true)
        {
            draw();
            input();
            update();
            int speed = max(40, 150 - score * 5);
            Sleep(speed);
        }
    }
};

// ---------- Title Screen ----------
void showTitleScreen()
{
    clearScreen();

    cout << BOLD << NEON_GREEN << R"(
  ╔═══════════════════════════════════════════════════════════════════════════════════════ ╗
  ║                                                                                        ║
  ║    ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗    ██████╗  █████╗ ███╗   ███╗███████╗   ║
  ║    ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝   ██╔════╝ ██╔══██╗████╗ ████║██╔════╝   ║
  ║    ███████╗██╔██╗ ██║███████║█████╔╝ █████╗     ██║  ███╗███████║██╔████╔██║█████╗     ║
  ║    ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝     ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝     ║
  ║    ███████║██║ ╚████║██║  ██║██║  ██╗███████╗   ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗   ║
  ║    ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝   ║
  ║                                                                                        ║
  ╚══════════════════════════════════════════════════════════════════════════════════════  ╝
)" << RESET
         << endl;

    cout << NEON_CYAN << BOLD << "                        THE CLASSIC ARCADE EXPERIENCE\n"
         << RESET << endl;

    cout << GOLD << BOLD << "  CONTROLS:\n" << RESET;
    cout << BRIGHT_CYAN << "     W or UP ARROW    = Move Up\n";
    cout << "     S or DOWN ARROW  = Move Down\n";
    cout << "     A or LEFT ARROW  = Move Left\n";
    cout << "     D or RIGHT ARROW = Move Right\n";
    cout << "     R = Restart Game\n";
    cout << "     Q = Quit Game\n" << RESET;

    cout << GOLD << BOLD << "\n  OBJECTIVE:\n" << RESET;
    cout << BRIGHT_CYAN << "     Eat the red food to grow longer!\n";
    cout << "     Avoid walls and your own body!\n";
    cout << "     The game speeds up as you score!\n" << RESET;

    cout << NEON_YELLOW << BOLD << "\n  Press ANY KEY to start!\n" << RESET;

    _getch();
    clearScreen();
}

// ---------- Main ----------
int main()
{
#ifdef _WIN32
    // Set UTF-8 encoding
    SetConsoleOutputCP(CP_UTF8);
    
    // Hide cursor
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
#else
    // Hide cursor on Linux
    cout << "\033[?25l";
#endif

    showTitleScreen();
    Game game(80, 30);
    game.run();
    
#ifndef _WIN32
    // Show cursor again on exit
    cout << "\033[?25h";
#endif
    
    return 0;
}

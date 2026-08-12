#include <windows.h>
#include <iostream>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <string>
using namespace std;

// Platform-specific headers
#ifdef _WIN32
#include <conio.h>
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
void clearScreen() { cout << "\033[2J\033[H"; }
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
    void generate(const deque<Point> &snake1Body, const deque<Point> &snake2Body, int width, int height)
    {
        vector<Point> valid;
        for (int i = 0; i < height - 1; i++)
        {
            for (int j = 1; j < width - 1; j++)
            {
                Point candidate(i, j);
                bool occupied = false;
                for (auto &seg : snake1Body)
                {
                    if (seg == candidate)
                    {
                        occupied = true;
                        break;
                    }
                }
                if (!occupied)
                {
                    for (auto &seg : snake2Body)
                    {
                        if (seg == candidate)
                        {
                            occupied = true;
                            break;
                        }
                    }
                }
                if (!occupied)
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
    Direction lastMovedDir;

public:
    Snake(int startX, int startY, Direction initialDir)
    {
        dir = initialDir;
        lastMovedDir = initialDir;
        body.push_back(Point(startX, startY));
        if (initialDir == RIGHT)
        {
            body.push_back(Point(startX, startY - 1));
            body.push_back(Point(startX, startY - 2));
        }
        else if (initialDir == LEFT)
        {
            body.push_back(Point(startX, startY + 1));
            body.push_back(Point(startX, startY + 2));
        }
        else if (initialDir == UP)
        {
            body.push_back(Point(startX + 1, startY));
            body.push_back(Point(startX + 2, startY));
        }
        else
        {
            body.push_back(Point(startX - 1, startY));
            body.push_back(Point(startX - 2, startY));
        }
    }

    void changeDir(Direction newDir)
    {
        if ((lastMovedDir == UP && newDir == DOWN) || (lastMovedDir == DOWN && newDir == UP) ||
            (lastMovedDir == LEFT && newDir == RIGHT) || (lastMovedDir == RIGHT && newDir == LEFT))
            return;
        dir = newDir;
    }

    Point getNextHead() const
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
        return head;
    }

    bool willCollide(Point nextHead, int width, int height, const deque<Point> &otherBody) const
    {
        // Wall collision
        if (nextHead.x < 0 || nextHead.x >= height - 1 || nextHead.y <= 0 || nextHead.y >= width - 1)
            return true;

        // Self collision
        for (auto &seg : body)
            if (nextHead == seg)
                return true;

        // Other snake collision
        for (auto &seg : otherBody)
            if (nextHead == seg)
                return true;

        return false;
    }

    bool moveForward(Point nextHead, Food &food, int &score)
    {
        lastMovedDir = dir;
        body.push_front(nextHead);

        if (food.isActive() && nextHead == food.getPos())
        {
            score++;
            return true;
        }
        else
        {
            body.pop_back();
            return false;
        }
    }

    const deque<Point> &getBody() const { return body; }
    Point getHead() const { return body.front(); }
};

class Game
{
    int width, height;
    Snake snake1;
    Snake snake2;
    Food food;
    int score1, score2, highScore;
    bool gameOver;
    bool paused;
    int loser; // 0 = none, 1 = P1, 2 = P2, 3 = Both

public:
    Game(int w = 50, int h = 20)
        : width(w), height(h),
          snake1(h / 2 - 2, 10, RIGHT),
          snake2(h / 2 + 2, w - 11, LEFT)
    {
        srand(static_cast<unsigned int>(time(0)));
        score1 = 0;
        score2 = 0;
        highScore = 0;
        gameOver = false;
        paused = false;
        loser = 0;
        food.generate(snake1.getBody(), snake2.getBody(), width, height);
    }

    void draw()
    {
        string buffer;
        buffer.reserve(8000);

        // Top border
        buffer += BRIGHT_CYAN;
        buffer += BOLD;
        buffer += "╔";
        for (int j = 0; j < width - 2; j++)
            buffer += "═";
        buffer += "╗\n";
        buffer += RESET;

        // Game area
        for (int i = 0; i < height - 1; i++)
        {
            buffer += BRIGHT_CYAN;
            buffer += BOLD;
            buffer += "║";
            buffer += RESET;

            for (int j = 0; j < width - 2; j++)
            {
                Point cur(i, j + 1);

                // Check Snake 1 Head
                if (snake1.getHead() == cur)
                {
                    buffer += BRIGHT_GREEN;
                    buffer += BOLD;
                    buffer += "●";
                    buffer += RESET;
                }
                // Check Snake 2 Head
                else if (snake2.getHead() == cur)
                {
                    buffer += NEON_PURPLE;
                    buffer += BOLD;
                    buffer += "●";
                    buffer += RESET;
                }
                else
                {
                    bool isBody = false;

                    // Check Snake 1 Body
                    for (auto &seg : snake1.getBody())
                    {
                        if (seg == cur && !(seg == snake1.getHead()))
                        {
                            buffer += NEON_GREEN;
                            buffer += "●";
                            buffer += RESET;
                            isBody = true;
                            break;
                        }
                    }

                    // Check Snake 2 Body
                    if (!isBody)
                    {
                        for (auto &seg : snake2.getBody())
                        {
                            if (seg == cur && !(seg == snake2.getHead()))
                            {
                                buffer += PINK;
                                buffer += "●";
                                buffer += RESET;
                                isBody = true;
                                break;
                            }
                        }
                    }

                    // Check Food
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
                        {
                            buffer += " ";
                        }
                    }
                }
            }
            buffer += BRIGHT_CYAN;
            buffer += BOLD;
            buffer += "║\n";
            buffer += RESET;
        }

        // Bottom border
        buffer += BRIGHT_CYAN;
        buffer += BOLD;
        buffer += "╚";
        for (int j = 0; j < width - 2; j++)
            buffer += "═";
        buffer += "╝\n";
        buffer += RESET;

        // Score display
        buffer += GOLD;
        buffer += BOLD;
        buffer += "> ";
        buffer += RESET;
        buffer += LIGHT_GREEN;
        buffer += BOLD;
        buffer += "P1 Score: ";
        buffer += to_string(score1);
        buffer += RESET;
        buffer += GOLD;
        buffer += BOLD;
        buffer += "  |  ";
        buffer += RESET;
        buffer += PINK;
        buffer += BOLD;
        buffer += "P2 Score: ";
        buffer += to_string(score2);
        buffer += RESET;
        buffer += GOLD;
        buffer += BOLD;
        buffer += "  |  ";
        buffer += RESET;
        buffer += ORANGE;
        buffer += "High Score: ";
        buffer += to_string(highScore);
        buffer += RESET;
        buffer += "          \n";

        if (gameOver)
        {
            buffer += NEON_RED;
            buffer += BOLD;
            buffer += "> GAME OVER! ";
            buffer += RESET;
            if (loser == 1)
            {
                buffer += LIGHT_GREEN;
                buffer += BOLD;
                buffer += "Player 1 Lost! (Player 2 Wins!) ";
            }
            else if (loser == 2)
            {
                buffer += PINK;
                buffer += BOLD;
                buffer += "Player 2 Lost! (Player 1 Wins!) ";
            }
            else
            {
                buffer += YELLOW;
                buffer += BOLD;
                buffer += "Both Players Lost! (Draw) ";
            }
            buffer += RESET;
            buffer += CYAN;
            buffer += "| R: Restart | Q: Quit";
            buffer += RESET;
            buffer += "    \n";
        }
        else if (paused)
        {
            buffer += NEON_YELLOW;
            buffer += BOLD;
            buffer += "> PAUSED! ";
            buffer += RESET;
            buffer += CYAN;
            buffer += "Press P to resume                        ";
            buffer += RESET;
            buffer += "\n";
        }
        else
        {
            buffer += DIM_CYAN;
            buffer += "> P1: Arrow Keys | P2: WASD | P: Pause";
            buffer += RESET;
            buffer += "          \n";
        }

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
        while (_kbhit())
        {
            int key = _getch();

#ifdef _WIN32
            if (key == 0 || key == 224)
            {
                int arrow = _getch();
                if (!paused && !gameOver)
                {
                    switch (arrow)
                    {
                    case 72:
                        snake1.changeDir(UP);
                        break;
                    case 80:
                        snake1.changeDir(DOWN);
                        break;
                    case 75:
                        snake1.changeDir(LEFT);
                        break;
                    case 77:
                        snake1.changeDir(RIGHT);
                        break;
                    }
                }
                continue;
            }
#else
            if (key == 27)
            {
                if (_kbhit() && _getch() == 91)
                {
                    int arrow = _getch();
                    if (!paused && !gameOver)
                    {
                        switch (arrow)
                        {
                        case 65:
                            snake1.changeDir(UP);
                            break;
                        case 66:
                            snake1.changeDir(DOWN);
                            break;
                        case 68:
                            snake1.changeDir(LEFT);
                            break;
                        case 67:
                            snake1.changeDir(RIGHT);
                            break;
                        }
                    }
                }
                continue;
            }
#endif

            char lowerKey = tolower(key);
            if (lowerKey == 'p' && !gameOver)
            {
                paused = !paused;
            }
            else if (lowerKey == 'r' && gameOver)
            {
                reset();
            }
            else if (lowerKey == 'q' && gameOver)
            {
                cout << "\033[?25h" << endl;
                exit(0);
            }
            else if (!paused && !gameOver)
            {
                switch (lowerKey)
                {
                case 'w':
                    snake2.changeDir(UP);
                    break;
                case 's':
                    snake2.changeDir(DOWN);
                    break;
                case 'a':
                    snake2.changeDir(LEFT);
                    break;
                case 'd':
                    snake2.changeDir(RIGHT);
                    break;
                }
            }
        }
    }

    void update()
    {
        if (!gameOver && !paused)
        {
            Point head1 = snake1.getNextHead();
            Point head2 = snake2.getNextHead();

            bool col1 = snake1.willCollide(head1, width, height, snake2.getBody());
            bool col2 = snake2.willCollide(head2, width, height, snake1.getBody());

            if (head1 == head2)
            {
                col1 = true;
                col2 = true;
            }

            if (col1 || col2)
            {
                gameOver = true;
                if (col1 && col2)
                    loser = 3;
                else if (col1)
                    loser = 1;
                else
                    loser = 2;

                if (score1 > highScore)
                    highScore = score1;
                if (score2 > highScore)
                    highScore = score2;
                return;
            }

            bool ate1 = snake1.moveForward(head1, food, score1);
            bool ate2 = snake2.moveForward(head2, food, score2);

            if (ate1 || ate2)
            {
                food.deactivate();
                food.generate(snake1.getBody(), snake2.getBody(), width, height);
            }
            else if (!food.isActive())
            {
                food.generate(snake1.getBody(), snake2.getBody(), width, height);
            }
        }
    }

    void reset()
    {
        snake1 = Snake(height / 2 - 2, 10, RIGHT);
        snake2 = Snake(height / 2 + 2, width - 11, LEFT);
        score1 = 0;
        score2 = 0;
        gameOver = false;
        paused = false;
        loser = 0;
        food.generate(snake1.getBody(), snake2.getBody(), width, height);
    }

    void run()
    {
        clearScreen();
        while (true)
        {
            draw();
            input();
            update();
            int maxScore = max(score1, score2);
            int speed = max(40, 150 - maxScore * 5);
            Sleep(speed);
        }
    }
};

// ---------- Title Screen ----------
void showTitleScreen()
{
    clearScreen();

    cout << BOLD << NEON_GREEN << R"(
  ╔═══════════════════════════════════════════════════════════════════════════════════════╗
  ║                                                                                       ║
  ║    ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗    ██████╗  █████╗ ███╗   ███╗███████╗  ║
  ║    ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝   ██╔════╝ ██╔══██╗████╗ ████║██╔════╝  ║
  ║    ███████╗██╔██╗ ██║███████║█████╔╝ █████╗      ██║  ███╗███████║██╔████╔██║█████╗  ║
  ║    ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝      ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  ║
  ║    ███████║██║ ╚████║██║  ██║██║  ██╗███████╗   ╚██████╔╝██║  ██║██║ ╚═╝ ██║██████╗   ║
  ║    ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚═════╝   ║
  ║                                                                                       ║
  ╚═══════════════════════════════════════════════════════════════════════════════════════╝
)" << RESET
         << endl;

    cout << NEON_CYAN << BOLD << "                    MULTIPLAYER ARCADE EXPERIENCE (2 PLAYERS)\n"
         << RESET << endl;

    cout << GOLD << BOLD << "  CONTROLS:\n"
         << RESET;
    cout << BRIGHT_GREEN << "     PLAYER 1 (Green)  : ARROW KEYS (Up, Down, Left, Right)\n"
         << RESET;
    cout << PINK << "     PLAYER 2 (Purple) : W, A, S, D\n"
         << RESET;
    cout << BRIGHT_CYAN << "     P                 : Pause / Resume\n";
    cout << "     R                 : Restart Game\n";
    cout << "     Q                 : Quit Game\n"
         << RESET;

    cout << GOLD << BOLD << "\n  OBJECTIVE:\n"
         << RESET;
    cout << BRIGHT_CYAN << "     Both snakes compete for the same food (◆)!\n";
    cout << "     Whoever eats the food first grows and scores a point.\n";
    cout << "     Avoid hitting walls, yourself, or the other snake!\n"
         << RESET;

    cout << NEON_YELLOW << BOLD << "\n  Press ANY KEY to start!\n"
         << RESET;

    _getch();
    clearScreen();
}

// ---------- Main ----------
int main()
{
#ifdef _WIN32
    // Set UTF-8 encoding
    SetConsoleOutputCP(CP_UTF8);

    // Enable Virtual Terminal Processing for ANSI escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode))
    {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
        SetConsoleMode(hOut, dwMode);
    }

    // Hide console cursor
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);
#else
    // Hide cursor on Linux
    cout << "\033[?25l";
#endif

    showTitleScreen();
    Game game(50, 20);
    game.run();

    // Show cursor again on exit
    cout << "\033[?25h";

    return 0;
}

// Lab 4 tests. Build from the repo root:
//   g++ -std=c++11 --coverage -O0 -g lab4/tests.cpp -o tests
//
// main.cpp is a single translation unit with its own main(), so it is pulled in
// here with main renamed. No line of main.cpp is changed by this.
#define main snake_main
#include "../main.cpp"
#undef main

#include <iostream>
#include <string>

static int failures = 0;

#define CHECK(cond, msg)                                              \
    do                                                                \
    {                                                                 \
        if (!(cond))                                                  \
        {                                                             \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__       \
                      << "  " << msg << "\n";                         \
            failures++;                                               \
        }                                                             \
    } while (0)

static std::string str(Point p)
{
    return "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
}

// Board used by every Snake test. Snake(40, 20) starts with its head at
// (height/2, width/2) = (10, 20), facing RIGHT (main.cpp:156-159).
static const int W = 40, H = 20;

// ---------- Rule 1: head shifts one unit in its current direction ----------
static void rule1_head_moves_one_unit_right()
{
    Snake s(W, H);
    Food noFood; // inactive, so move() never scores
    int score = 0;
    Point before = s.getHead();

    bool ok = s.move(noFood, score, W, H);

    CHECK(ok, "move inside the board should succeed");
    CHECK(s.getHead() == Point(before.x, before.y + 1),
          "head " << str(s.getHead()) << " expected " << str(Point(before.x, before.y + 1)));
}

static void rule1_head_moves_one_unit_down()
{
    Snake s(W, H);
    Food noFood;
    int score = 0;
    Point before = s.getHead();

    s.changeDir(DOWN);
    s.move(noFood, score, W, H);

    CHECK(s.getHead() == Point(before.x + 1, before.y),
          "head " << str(s.getHead()) << " expected " << str(Point(before.x + 1, before.y)));
}

// ---------- Rule 2: opposite direction is ignored ----------
static void rule2_reverse_is_ignored()
{
    Snake s(W, H); // facing RIGHT
    Food noFood;
    int score = 0;
    Point before = s.getHead();

    s.changeDir(LEFT);
    bool ok = s.move(noFood, score, W, H);

    CHECK(ok, "reversal must not make the snake hit its own neck");
    CHECK(s.getHead() == Point(before.x, before.y + 1),
          "after LEFT while facing RIGHT, head " << str(s.getHead())
                                                 << " expected to keep going right to "
                                                 << str(Point(before.x, before.y + 1)));
}

// ---------- Rule 3: leaving the board makes move() return false ----------
static void rule3_move_into_right_wall_returns_false()
{
    Snake s(W, H);
    Food noFood;
    int score = 0;

    int steps = 0;
    while (s.move(noFood, score, W, H) && steps < 100)
        steps++;

    // head starts at y = 20; last playable column is W - 2 = 38
    CHECK(steps == 18, "expected 18 good moves before the wall, got " << steps);
    CHECK(!s.move(noFood, score, W, H), "move into the wall should keep failing");
}

// ---------- Rule 5: 'Q' quits (through the Console seam) ----------
// Stubbed keys in, recorded quit() calls out.
class SpyConsole : public Console
{
    std::deque<int> keys;

public:
    int quitCalls = 0;
    int quitCode = -1;

    void press(int key) { keys.push_back(key); }
    bool keyPressed() { return !keys.empty(); }
    int readKey()
    {
        int k = keys.front();
        keys.pop_front();
        return k;
    }
    void quit(int code)
    {
        quitCalls++;
        quitCode = code;
    }
};

static void rule5_q_while_playing_does_not_quit()
{
    SpyConsole console;
    Game game(W, H, console);

    console.press('Q');
    game.input();

    CHECK(console.quitCalls == 0, "Q during play called quit() " << console.quitCalls << " time(s)");
}

static void rule5_q_after_game_over_quits_with_code_0()
{
    SpyConsole console;
    Game game(W, H, console);
    for (int i = 0; i < 30; i++) // snake runs right into the wall after 19 ticks
        game.update();

    console.press('Q');
    game.input();

    CHECK(console.quitCalls == 1, "Q after game over called quit() " << console.quitCalls << " time(s)");
    CHECK(console.quitCode == 0, "quit code " << console.quitCode << " expected 0");
}

int main()
{
    rule1_head_moves_one_unit_right();
    rule1_head_moves_one_unit_down();
    rule2_reverse_is_ignored();
    rule3_move_into_right_wall_returns_false();
    rule5_q_while_playing_does_not_quit();
    rule5_q_after_game_over_quits_with_code_0();

    if (failures == 0)
        std::cout << "all tests passed\n";
    else
        std::cout << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}

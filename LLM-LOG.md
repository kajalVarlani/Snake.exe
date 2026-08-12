# LLM Log - Lab Assignment 3: Multiplayer Snake

## 1. Setup
- **Model(s)**: Gemini 3.6 Flash
- **Tool / harness**: Antigravity AI Agent
- **IDE / editor**: VS Code
- **Did you paste this assignment document into it?**: y (full assignment document provided in prompt)

---

## 2. Session Log

### Prompt 1
> "Look at the folder and see the snake game, i want you to make it perfect as of now the game is generating a new frame below the previous frame every movement, and is bliking for every movement. make it perfect"

**Code Produced (Excerpt from `main.cpp`)**:
Fixed screen scrolling by removing leading spaces from borders, enforcing `50x20` board dimensions, and using console cursor repositioning without `system("cls")`.
```cpp
// Move cursor to top-left without clearing screen
#ifdef _WIN32
COORD coord = {0, 0};
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
cout << "\033[H";
#endif
cout << buffer << flush;
```

---

### Prompt 2
> "do whatever you think is right"

**Code Produced (Excerpt from `main.cpp`)**:
Approved initial plan and executed basic setup fixes.

---

### Prompt 3
> "now make it a multiplayer game, you can find every needed information to make it multiuser from below instructions... [Lab Assignment 3 instructions pasted]"

**Code Produced (Excerpt from `main.cpp`)**:
Implemented 2-Player Snake logic in `main.cpp`:
- `snake1` controlled by Arrow Keys, `snake2` controlled by WASD.
- Both snakes share the same `food`.
- Individual scores (`score1`, `score2`) rendered in `draw()`.
- Collision handling for walls, self, and opponent snake, announcing the loser upon game over.

```cpp
// Collision checking across both snakes
Point head1 = snake1.getNextHead();
Point head2 = snake2.getNextHead();

bool col1 = snake1.willCollide(head1, width, height, snake2.getBody());
bool col2 = snake2.willCollide(head2, width, height, snake1.getBody());

if (head1 == head2) {
    col1 = true;
    col2 = true;
}

if (col1 || col2) {
    gameOver = true;
    if (col1 && col2) loser = 3;
    else if (col1) loser = 1;
    else loser = 2;
}
```

---

### Summary
- **Total Prompts to Working Code**: 3
- **Attempt that Worked**: Attempt 3 (with minor string literal syntax fix)
- **Code Provided to LLM**: Whole repository (`main.cpp`, `title.h`, `README.md`)

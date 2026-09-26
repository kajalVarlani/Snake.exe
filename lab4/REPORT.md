# Lab 4 — Group A__

| | |
|---|---|
| Repository | https://github.com/kajalVarlani/Snake.exe (your own snake game) |
| Base tag | `lab4-base` at commit `c9cd1ec` |
| Pull request | _link to this PR_ |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | When the snake moves, its head shifts by one unit in its current direction. |
| 2 | The snake ignores direction changes that are exactly opposite to its current direction. |
| 3 | If the snake moves out of the board boundaries, the move fails and returns false. |
| 4 | When a new game starts, the player's initial score is exactly zero. |
| 5 | When the user inputs 'Q' while playing, the game saves the high score and immediately exits the process. |

If you could not state one of your own game's rules without going to look, say which and
why. It costs no marks.

None. But Rule 5 turned out to be **wrong** once the source was opened. 'Q' only quits
after a game over (`main.cpp:435`, `if (gameOver)`), not while playing. The high score is
never saved: it only lives in the `highScore` member (`main.cpp:450`) and is lost on exit.
The rule stays as written, because it was written before reading. The Rule 5 tests in
Part D check what the game actually does.

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Head shifts one unit in current direction | **Yes**: `rule1_head_moves_one_unit_right`, `rule1_head_moves_one_unit_down` | None. `Snake` (`main.cpp:148`) is a plain class. `Snake::move` takes the `Food`, `score` and board size as parameters, with no I/O, `rand` or `Sleep`. |
| 2 | Opposite direction ignored | **Yes**: `rule2_reverse_is_ignored` | None. `dir` is private (`main.cpp:151`) with no getter, but the rule is observable through `move()`: facing RIGHT, `changeDir(LEFT)`, then the head still goes right. |
| 3 | Leaving the board makes `move()` return false | **Yes**: `rule3_move_into_right_wall_returns_false` | None for the right wall. The check is on `main.cpp:192`. |
| 4 | New game starts with score 0 | **No** | `main.cpp:224`: `int score` is a private member of `Game` with no accessor. The only place it leaves the object is `main.cpp:321`, `to_string(score)` inside `draw()`, concatenated with ANSI colour codes and written straight to `cout` at `main.cpp:360`. Testing it would mean asserting on rendered output, which is rendering, not behaviour. |
| 5 | 'Q' saves the high score and exits | **No** | `main.cpp:365`: `_kbhit()` and `main.cpp:367`: `_getch()` inside `Game::input()` read the real console (`<conio.h>`), so there is no way to supply the key 'Q'. `main.cpp:436`: `exit(0)` would terminate the test runner itself. Whether Q acts depends on the private `gameOver` flag (`main.cpp:225`, checked at `main.cpp:435`), with no accessor. |

> **Rules testable without modifying the source: 3 / 5**

Why three and not one or two: the snake logic already sits in a `Snake` class whose `move()`
takes everything as parameters. All the hard dependencies (`_getch`/`_kbhit`, `exit`,
`srand(time(0))` at `main.cpp:230`, `Sleep` in `run()`) are in `Game`, and only Rules 4
and 5 go through `Game`.

One thing to disclose: `main.cpp` defines its own `main()` (`main.cpp:527`), so a test file
cannot be linked next to it. The test file does `#define main snake_main` and then
`#include "../main.cpp"`. That is a preprocessing trick on the test side only. No line of
the game changed. The test runner is a small `CHECK` macro rather than gtest, because the
available toolchain is MinGW.org GCC 6.3.

---

## 3. Coverage, and what it missed — [6]

Measured on the Part B suite (Rules 1–3), against `lab4-base`.

| | |
|---|---|
| Line coverage | 14.71 % of 238 lines (`main.cpp`) |
| Branch coverage | 9.54 % taken at least once (15.90 % of 283 branches executed) |
| Command used | `g++ -std=c++11 --coverage -O0 -g lab4/tests.cpp -o tests && ./tests && gcov -b -o . lab4/tests.cpp` |

Because `tests.cpp` includes `main.cpp`, this is a single translation unit, and `gcov`
writes `main.cpp.gcov` for the game file. After Part D, with the Rule 5 tests added, the
figures are 31.98 % of lines and 18.82 % of branches taken.

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | 3: leaving the board makes the move fail |
| Line that runs | `main.cpp:192`: `if (head.x < 0 \|\| head.x >= height - 1 \|\| head.y <= 0 \|\| head.y >= width - 1)` runs 23 times. gcov shows the first three conditions evaluated every time with their true branch taken **0 %**. |
| The assertion that is missing | The top, bottom and left walls are never asserted. For example: `s.changeDir(UP);` then move until `head.x == 0`, then `CHECK(!s.move(noFood, score, W, H), ...)`. Do the same for LEFT (`head.y == 1`) and DOWN (`head.x == H - 2`). |

The suite goes green, and line 192 shows as covered, yet `head.x < 0` could be deleted or
changed to `head.x < -1`, and no test would notice. Those three walls are also the
asymmetric ones: the top row `x = 0` is playable, but the left column `y = 0` is a wall.
That is exactly the kind of thing a missing assertion hides.

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | 5: 'Q' quits the game |
| Commit 1 (seam) | `c884baf`: Add Console seam for keyboard input and quit |
| Commit 2 (test) | Add Lab 4 tests and report (hash in PR) |
| Seam kind | object |
| Enabling point | The third parameter of the `Game` constructor, `Console &c = realConsole()` (`main.cpp:254` after commit 1). `main()` still does `Game game(80, 30);` (`main.cpp:571`) and gets `RealConsole`. A test passes its own `Console`. |
| What production code gave up | See below. |

Commit 1 changes only `Game::input()`. Every `_kbhit()` becomes `console.keyPressed()`,
every `_getch()` becomes `console.readKey()`, and `exit(0)` becomes `console.quit(0)`.
`RealConsole` forwards to exactly those three calls, so a player sees no difference.

What production code gave up:

1. **The guarantee that nothing runs after 'Q'.** `exit(0)` never returns. `console.quit(0)`
   only doesn't return because `RealConsole` happens to call `exit`. The code after it in
   `input()` (currently just `break;`) now has to be safe to run, because under a test
   double it *does* run. Reading `input()` alone no longer tells you the process ends there.
2. **`Game` is no longer assignable.** It holds a `Console &` member, so the implicit
   `operator=` is deleted. Nothing assigns a `Game` today (`reset()` reassigns `snake`,
   not the game), so nothing breaks, but the option is gone.
3. **A virtual call per input check.** `keyPressed()` is called once per frame (every
   40–150 ms). That is negligible here.
4. **The seam is partial.** The title screen's `_getch()` (`main.cpp:522` in base) still
   talks to the keyboard directly. It is outside the rule, so it was left alone.

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | **spy** (`SpyConsole`, which also stubs the key queue) |
| The method under test | `Game::input()` |

`keyPressed()` and `readKey()` are questions, and a stub answering them is enough to feed
'Q' in. But the rule is about `quit()`, which is an order: the game *tells* the console to
end the process, and `quit()` returns `void`. There is no return value to check, so the
only thing left to assert is that the call happened, how many times, and with which code.
That makes the double a spy that records `quitCalls` and `quitCode`, rather than a stub.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | **Mystery guest.** `for (int i = 0; i < 30; i++) game.update();` only reaches game over because of the snake's start position and direction (`main.cpp:156-159`) and the board size. None of that is visible in the test, and the test never confirms game over happened. | `lab4/tests.cpp:138` | Compute the ticks from the board (`W - 2 - W / 2 + 1`) next to a comment naming the start cell, or make game-over arrangeable directly instead of via a magic 30. |
| 2 | **A test that nearly passes whatever the code does.** `CHECK(console.quitCalls == 0, ...)` would also pass if `input()` never read the key at all, for example if the seam were bypassed or `keyPressed()` was never called. "Nothing happened" is indistinguishable from "the key was ignored on purpose". | `lab4/tests.cpp:131` | Also assert that the key was consumed (the spy's queue is empty after `input()`), so the test proves 'Q' was read *and* ignored. |

Also noticed: `lab4/tests.cpp:94` hard-codes `steps == 18`, another number that is only
explained by reading `main.cpp`. This is a milder case of smell 1.

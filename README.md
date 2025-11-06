
# 🐍 Snake Game (C++ Cross-Platform Console Version)

A **modern terminal-based Snake Game** built entirely in **C++**, featuring **cross-platform keyboard support**, **colored visuals**, **dynamic speed increase**, and **smooth gameplay** on both **Windows** and **Linux/Ubuntu** systems.

---

## 🎮 Features

✅ **Cross-Platform Support**
Works seamlessly on both **Windows** (`cmd`, `PowerShell`) and **Linux/Ubuntu** terminals.

✅ **Colorful Interface**
Uses **ANSI escape codes** to render a bright and visually appealing snake board.

✅ **Smooth Controls**
Supports both **WASD keys** and **Arrow keys** for movement.

✅ **Dynamic Speed**
The snake’s speed **increases as your score grows** to make the game more challenging.

✅ **Game Over Screen & Restart**
Option to **restart** the game instantly (`R`) or **quit** (`Q`) after losing.

✅ **High Score Tracking (in-session)**
Displays **high score** (stored until the program exits).

---

## 🕹️ Controls

| Key        | Action                  |
| ---------- | ----------------------- |
| `W` or `↑` | Move Up                 |
| `S` or `↓` | Move Down               |
| `A` or `←` | Move Left               |
| `D` or `→` | Move Right              |
| `R`        | Restart after Game Over |
| `Q`        | Quit after Game Over    |

---

## 🎯 Objective

* Eat the **red food (◆)** to grow your snake.
* Avoid **walls** and **your own body**.
* The **game speeds up** as your score increases.
* Try to beat your **high score** in each session!

---

## ⚙️ How to Compile & Run

### 🧱 On Windows

1. Open **Command Prompt** or **PowerShell**.
2. Navigate to the folder containing your file (e.g. `main.cpp`).
3. Compile using **g++**:

   ```bash
   g++ main.cpp -o snake
   ```
4. Run the game:

   ```bash
   snake
   ```

---

### 🐧 On Linux / Ubuntu (or WSL)

1. Open the **terminal**.
2. Navigate to your project directory:

   ```bash
   cd /path/to/your/folder
   ```
3. Compile:

   ```bash
   g++ main.cpp -o snake
   ```
4. Run:

   ```bash
   ./snake
   ```

💡 *If you’re using Windows Subsystem for Linux (WSL), make sure you run it inside the Ubuntu terminal.*

---

## 🧩 Code Structure

| Class     | Description                                                                             |
| --------- | --------------------------------------------------------------------------------------- |
| **Point** | Represents coordinates (x, y) and supports collision checking via operator overloading. |
| **Food**  | Manages random food placement ensuring it doesn’t overlap with the snake.               |
| **Snake** | Handles direction, movement, growth, and self-collision detection.                      |
| **Game**  | Manages drawing, input, score, game logic, and rendering loop.                          |


---

## 🧠 Future Improvements

* Persistent **High Score** saved in a file
* Add **obstacles / levels**
* Implement **pause/resume** feature
* Save highest score

---



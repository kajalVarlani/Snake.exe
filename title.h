#ifndef TITLE_H
#define TITLE_H

#include <iostream>
#include <cstdlib>
#include <conio.h>  // for _getch()

using namespace std;

void showTitleScreen() {
    system("cls"); // clear screen

    cout << "\033[1;32m"; // Bright Green for Title
    cout << R"(

   ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗     ██████╗  █████╗ ███╗   ███╗███████╗
   ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝
   █████╗  ██╔██╗ ██║███████║█████╔╝ █████╗      ██║  ███╗███████║██╔████╔██║█████╗  
   ██╔══╝  ██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝      ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  
   ███████╗██║ ╚████║██║  ██║██║  ██╗███████╗    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗
   ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝     ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝

    )" << endl;

    cout << "\033[1;33m"; // Yellow for instructions
    cout << "\n\n";
    cout << "  Welcome to the Classic Snake Game! 🐍\n";
    cout << "  -------------------------------------\n";
    cout << "  Controls:\n";
    cout << "    W / ↑  → Move Up\n";
    cout << "    S / ↓  → Move Down\n";
    cout << "    A / ←  → Move Left\n";
    cout << "    D / →  → Move Right\n";
    cout << "\n";
    cout << "  Rules:\n";
    cout << "    - Eat '@' to grow.\n";
    cout << "    - Don't hit walls or yourself.\n";
    cout << "    - Score increases with every food.\n";
    cout << "\n\n";
    cout << "  Press any key to start the game...\n";

    cout << "\033[0m"; // Reset colors

    _getch(); // Wait for key press
    system("cls"); // Clear before starting game
}

#endif

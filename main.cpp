#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <ncurses.h>

#include "grid.hpp"

int main(int argc, char *argv[]) {
    std::stringstream
        log;  // log of all messages that would show but we have a tui
    int height, width;

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    mousemask(BUTTON1_CLICKED, nullptr);
    int prev_curs_mode = curs_set(0);

    if (argc == 1) {
        log << "note: assuming default dimensions for grid of 10x10\n";
        height = 10;
        width = 10;
    } else if (argc == 2) {
        log << "note: assuming only provided number is both the height and "
               "the width\n";
        height = std::atoi(argv[1]);
        width = height;
    } else if (argc == 3) {
        log << "note: parsing as height, width\n";
        height = std::atoi(argv[1]);
        width = std::atoi(argv[2]);
    }

    log << "note: dimensions are " << height << "x" << width << "\n";

    grid<int> world(10, 10);
    world.set_translation(
        std::map<int, char>{{0, ' '}, {1, '#'}, {2, '@'}, {3, '!'}});

    // main tui loop
    bool exit = false;
    while (!exit) {
        // computations go here
        int input = getch();
        while (input != ERR) {
            MEVENT mouse_event;
            switch (input) {
            case 'q':
                exit = true;
                break;
            case KEY_MOUSE:
                if (getmouse(&mouse_event) == OK) {
                    if (mouse_event.bstate & BUTTON1_CLICKED) {
                        log << "note: clicked on coordinates (" << mouse_event.x
                            << ", " << mouse_event.y << ")\n";
                        if (mouse_event.y < world.height() &&
                            mouse_event.x < world.width())
                            world[mouse_event.y][mouse_event.x] ^= 1;
                    }
                }
                break;
            }
            input = getch();
        }

        // let's abandon most of c++ and do a c-ish thing, this shouldn't work
        // if the previous approach didn't... right?
        erase();
        int row_idx = 0;
        for (auto row : world) {
            for (auto item : row) {
                printw("%c", world.translate(item));
            }
            row_idx++;
            move(row_idx, 0);
        }
        // ...sure
        refresh();
    }

    curs_set(prev_curs_mode);
    endwin();

    // at the very end, we output the log after clearing the screen to inform
    // the user of things that happened while the tui was up
    std::cout << log.str();
}

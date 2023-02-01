#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <ncurses.h>

#include "astar.hpp"
#include "grid.hpp"
#include "logs.hpp"
#include "render.hpp"

std::stringstream note_log;

int main(int argc, char *argv[]) {
    // TODO
    // TODO spdlog
    // TODO

    int height, width;

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, nullptr);
    int prev_curs_mode = curs_set(1);
    int curs_active = 1;

    if (argc == 1) {
        note_log << "note: assuming default dimensions for grid of 10x10\n";
        height = 10;
        width = 10;
    } else if (argc == 2) {
        note_log
            << "note: assuming only provided number is both the height and "
               "the width\n";
        height = std::atoi(argv[1]);
        width = height;
    } else if (argc == 3) {
        note_log << "note: parsing as height, width\n";
        height = std::atoi(argv[1]);
        width = std::atoi(argv[2]);
    }

    note_log << "note: dimensions are " << height << "x" << width << "\n";

    render::init(height, width, curs_active);

    // main tui loop
    while (true) {
        // computations go here
        if (render::input())
            break;

        if (astar::initialized && render::play)
            if (astar::tick())
                astar::term();

        render::draw();
        // break;
    }

    note_log << "note: returning to cursor mode: " << prev_curs_mode << "\n";
    curs_set(prev_curs_mode);
    endwin();

    // at the very end, we output the log after clearing the screen to inform
    // the user of things that happened while the tui was up
    std::cout << note_log.str();
}

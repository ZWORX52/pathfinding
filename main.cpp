#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <fmt/core.h>
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

    int height = 40, width = 80;
    double chance = 40.0;

    initscr();
    if (!has_colors()) {
        endwin();
        std::cout << "Your terminal does not support colors :(\n";
        return 1;
    }
    start_color();

    init_color(COLOR_RED, 1000, 0, 0);
    init_pair(IMPASSABLE, COLOR_RED, COLOR_BLACK);

    init_color(COLOR_MAGENTA, 1000, 0, 1000);
    init_pair(START, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(GOAL, COLOR_MAGENTA, COLOR_BLACK);

    init_color(COLOR_CYAN, 1000, 500, 0);
    init_pair(EXPLORED, COLOR_CYAN, COLOR_BLACK);

    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_pair(QUEUE, COLOR_YELLOW, COLOR_BLACK);

    init_color(COLOR_GREEN, 0, 1000, 0);
    init_pair(PATH, COLOR_GREEN, COLOR_BLACK);

    init_color(COLOR_BLUE, 0, 0, 1000);
    init_pair(EXPLORE_PATH, COLOR_BLUE, COLOR_BLACK);

    cbreak();
    noecho();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
    mouseinterval(0);

    // this has to be all the way down here, and it only needs to exist because
    // ncurses doesn't actually REPORT_MOUSE_POSITION >:(
    std::cout << "\033[?1003h\n" << std::flush;

    int prev_curs_mode = curs_set(1);
    int curs_active = 1;

    if (argc == 1) {
    } else if (argc == 2) {
        chance = std::atof(argv[1]);
    } else if (argc == 3) {
        chance = std::atof(argv[1]);
        width = std::atoi(argv[2]);
        height = width;
    } else if (argc == 4) {
        chance = std::atof(argv[1]);
        width = std::atoi(argv[2]);
        height = std::atoi(argv[3]);
    }
    note_log << fmt::format("note: {}% fill rate and {}x{} grid\n", chance,
                            width, height);

    int x, y;
    getmaxyx(stdscr, y, x);
    y -= 2;  // WARNING: update this when more status lines are added!
    if (width > x || height > y) {
        // std::cout << "\033[?1003l" << std::flush;  // restore sanity
        // endwin();
        note_log << fmt::format(
            "dimensions are too large, snapping to max (too {}, max: {})\n",
            width > x ? "wide" : "tall", width > x ? x : y);
        width = x;
        height = y;
    }

    render::init(height, width, curs_active, chance);

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
    std::cout << "\033[?1003l" << std::flush;  // restore sanity

    // at the very end, we output the log after clearing the screen to inform
    // the user of things that happened while the tui was up
    std::cout << note_log.str();
}

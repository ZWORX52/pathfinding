#include "render.hpp"
#include "astar.hpp"
#include "grid.hpp"
#include "logs.hpp"

#include <map>
#include <ncurses.h>
#include <pcg_random.hpp>
#include <random>

const int PASSABLE = 0;
const int IMPASSABLE = 1;
const int START = 2;
const int GOAL = 3;
const int EXPLORED = 4;
const int QUEUE = 5;
const int PATH = 6;
const int EXPLORE_PATH = 7;

namespace render {
const std::map<int, char> translation{
    {PASSABLE, ' '}, {IMPASSABLE, '#'}, {START, ':'}, {GOAL, '!'},
    {EXPLORED, '.'}, {QUEUE, ','},      {PATH, '*'},  {EXPLORE_PATH, '~'}};

grid<int> world(1, 1);

int goal_x, goal_y;
int start_x, start_y;
int last_mouse_x = 0, last_mouse_y = 0;
bool play = false;
int curs_active;

double chance;

void fill_random(double chance) {
    pcg_extras::seed_seq_from<std::random_device> seed_source;
    pcg64 rng(seed_source);
    std::uniform_int_distribution<int> dist(1, 10000);
    for (std::vector<int> &row : world) {
        for (int &item : row) {
            if (item == PASSABLE) {
                if (dist(rng) <= static_cast<int>(chance * 100)) {
                    // rnjesus has decided that "this square shalt have block"
                    // so it will have a block
                    item = IMPASSABLE;
                }
            }
        }
    }
}

void init(int height, int width, int _curs_active, double _chance) {
    world = grid<int>(height, width);
    world.set_translation(translation);

    goal_x = 1;
    goal_y = 1;
    world[goal_x][goal_y] = 3;

    start_x = width - 2;
    start_y = height - 2;
    world[start_y][start_x] = 2;

    curs_active = _curs_active;

    astar::init(astar::node(goal_x, goal_y), astar::node(start_x, start_y),
                world);

    chance = _chance;
    fill_random(chance);
}

void draw() {
    erase();
    int row_idx = 0;
    for (const auto &row : world) {
        for (const auto &item : row) {
            attron(COLOR_PAIR(item));
            printw("%c", world.translate(item));
            attroff(COLOR_PAIR(item));
            // note_log << world.translate(item);
            // addch(world.translate(item));
        }
        // note_log << "\n";
        row_idx++;
        move(row_idx, 0);
    }
    move(last_mouse_y, last_mouse_x);
    refresh();
}

bool input() {
    int input = getch();
    static bool dragging_impassable;
    static bool dragging_passable;

    while (input != ERR) {
        switch (input) {
        case 'q':
            return true;
        case 'f':
            // from
            world[start_y][start_x] = PASSABLE;
            start_x = last_mouse_x;
            start_y = last_mouse_y;
            world[start_y][start_x] = START;
            break;
        case 'g':
            // goal
            world[goal_y][goal_x] = PASSABLE;
            goal_x = last_mouse_x;
            goal_y = last_mouse_y;
            world[goal_y][goal_x] = GOAL;
            break;
        case 'p':
            // play/pause
            play = !play;
            break;
        case 'I':
            // initialize
            astar::init(astar::node(goal_x, goal_y),
                        astar::node(start_x, start_y), world);
            break;
        case 's':
            // step
            if (astar::tick()) {
                note_log << "note: astar: algorithm done\n";
                astar::term();
            }
            break;
        case 'T':
            // terminate
            astar::term();
            break;
        case 'c':
            // clear board
            world.clear(IMPASSABLE, PASSABLE);
            break;
        case 'B':
            // backtrack
            astar::backtrack(world);
            break;
        case 'd':
            // toggle displaying path
            astar::path_display = !astar::path_display;
            break;
        case 'r':
            // full reset: reset astar and refill grid
            world.clear(IMPASSABLE, PASSABLE);
            fill_random(chance);
            // fallthrough
        case 'R':
            // partial reset: just reset astar
            astar::reset(world);
            astar::init(astar::node(goal_x, goal_y),
                        astar::node(start_x, start_y), world);
            play = false;
            break;
        case KEY_MOUSE:
            MEVENT mouse_event;
            if (getmouse(&mouse_event) == OK) {
                    size_t mouse_event_x = mouse_event.x;
                    size_t mouse_event_y = mouse_event.y;
                if (mouse_event.bstate & BUTTON1_PRESSED) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event.y][mouse_event.x];
                        if (clicked == PASSABLE)
                            clicked = IMPASSABLE;
                        dragging_impassable = true;
                    }
                } else if (mouse_event.bstate & BUTTON1_RELEASED) {
                    dragging_impassable = false;
                } else if (mouse_event.bstate & BUTTON2_PRESSED) {
                    last_mouse_x = mouse_event_x;
                    last_mouse_y = mouse_event_y;
                } else if (mouse_event.bstate & BUTTON3_PRESSED) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event_y][mouse_event_x];
                        if (clicked == IMPASSABLE)
                            clicked = PASSABLE;
                        dragging_passable = true;
                    }
                } else if (mouse_event.bstate & BUTTON3_RELEASED) {
                    dragging_passable = false;
                } else if (mouse_event.bstate & REPORT_MOUSE_POSITION) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event_y][mouse_event_x];
                        if (dragging_impassable && clicked == PASSABLE)
                            clicked = IMPASSABLE;
                        else if (dragging_passable && clicked == IMPASSABLE)
                            clicked = PASSABLE;
                    }
                }
            }
            break;
        }
        input = getch();
    }
    return false;
}
}  // namespace render

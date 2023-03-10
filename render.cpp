#include "render.hpp"
#include "astar.hpp"
#include "grid.hpp"
#include "logs.hpp"

#include <chrono>
#include <fmt/core.h>
#include <limits>
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

const int STATUS_LINES = 3;

namespace render {
const std::map<int, char> translation{
    {PASSABLE, ' '}, {IMPASSABLE, '#'}, {START, ':'}, {GOAL, '!'},
    {EXPLORED, '.'}, {QUEUE, ','},      {PATH, '*'},  {EXPLORE_PATH, '~'}};
constexpr int STATUS_COLUMN_WIDTH = 40;

grid<int> world(1, 1);

int goal_x, goal_y;
int start_x, start_y;
int last_mouse_x = 0, last_mouse_y = 0;
bool play = false;
int curs_active;
int height, width;

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

std::vector<double> frame_times;
int frame_nos;

struct _update {
    int x, y;
    int new_val;
};
bool lazy_updates;
std::vector<_update> updates;

void update(int x, int y, int new_val) {
    if (!lazy_updates) {
        world[y][x] = new_val;
        return;
    }
    if (world[y][x] == new_val)
        return;
    updates.push_back(_update{x, y, new_val});
    world[y][x] = new_val;
}

void init(int _height, int _width, int _curs_active, double _chance) {
    world = grid<int>(_height, _width);
    world.set_translation(translation);

    goal_x = 1;
    goal_y = 1;
    world[goal_x][goal_y] = 3;

    start_x = _width - 2;
    start_y = _height - 2;
    world[start_y][start_x] = 2;

    curs_active = _curs_active;

    astar::init(astar::node(goal_x, goal_y), astar::node(start_x, start_y),
                world);

    chance = _chance;
    fill_random(chance);
    height = _height;
    width = _width;

    frame_nos = 1;
    frame_times = std::vector<double>(frame_nos);

    updates = std::vector<_update>();

    int row_idx = STATUS_LINES;
    for (const auto &row : world) {
        move(row_idx++, 0);
        for (const auto &item : row) {
            attron(COLOR_PAIR(item));
            addch(world.translate(item));
        }
    }
}

inline void status_message(const std::string &message, const int row,
                           const int column) {
    mvprintw(row, STATUS_COLUMN_WIDTH * column, "%s", message.c_str());
}

inline void erase_status() {
    for (int i = 0; i < STATUS_LINES; i++) {
        move(i, 0);
        clrtoeol();
    }
}

void draw() {
    curs_set(0);
    erase_status();
    // show stats
    status_message("astar:", 0, 0);
    astar::stats astar_stats = astar::get_stats();
    status_message(fmt::format("nodes: {}/{}", astar_stats.explored_size,
                               astar_stats.queue_size),
                   1, 0);
    if (astar::success) {
        status_message(fmt::format("path length: {}", astar_stats.path_length),
                       2, 0);
    } else if (astar::done) {
        status_message("no path found :(", 2, 0);
    } else if (astar::path_display) {
        status_message(fmt::format("exploring, explore path length: {}",
                                   astar_stats.explore_path_length),
                       2, 0);
    } else if (astar_stats.explored_size > 0) {
        status_message("exploring", 2, 0);
    }

    static auto last_render = std::chrono::high_resolution_clock::now();
    double frame_duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - last_render)
            .count();
    static auto last_stats_update = std::chrono::system_clock::now();
    frame_times.push_back(frame_duration);

    static double max_ms = 0.0, avg_ms = 0.0, min_ms = 0.0, max_fps = 0.0,
                  avg_fps = 0.0, min_fps = 0.0;

    {
        using namespace std::chrono_literals;
        if (std::chrono::system_clock::now() - last_stats_update >= 3s) {
            last_stats_update = std::chrono::system_clock::now();

            max_ms = 0.0, avg_ms = 0.0, min_ms = frame_times[0], max_fps = 0.0,
            avg_fps = 0.0, min_fps = frame_times[0];
            for (const auto &this_frame_time : frame_times) {
                double frame_time_ms = this_frame_time / 1000.0;
                if (frame_time_ms > max_ms)
                    max_ms = frame_time_ms;
                avg_ms += frame_time_ms;
                if (frame_time_ms < min_ms)
                    min_ms = frame_time_ms;

                double fps = 1.0 / (frame_time_ms / std::micro::den);
                if (fps > max_fps)
                    max_fps = fps;
                avg_fps += fps;
                if (fps < min_fps)
                    min_fps = fps;
            }
            avg_ms /= frame_times.size();
            avg_fps /= frame_times.size();
            frame_times.clear();
        }
    }
    last_render = std::chrono::high_resolution_clock::now();

    status_message("render:", 0, 1);
    // max ms is 10.3f because when using `i`, i've seen up to 100 ms :O

    status_message(fmt::format("frame took {:.3f}us ({:.3f}/{:.3f}/{:.3f})",
                               frame_duration / 1000.0, max_ms, avg_ms, min_ms),
                   1, 1);
    status_message(fmt::format("running at {:.3f} fps ({:.3f}/{:.3f}/{:.3f})",
                               1.0 / (frame_duration / std::nano::den), max_fps,
                               avg_fps, min_fps),
                   2, 1);
    if (!lazy_updates) {
        int row_idx = STATUS_LINES;
        for (const auto &row : world) {
            move(row_idx++, 0);
            for (const auto &item : row) {
                attron(COLOR_PAIR(item));
                addch(world.translate(item));
            }
        }
        lazy_updates = true;
    } else {
        for (const auto &this_update : updates) {
            attron(COLOR_PAIR(this_update.new_val));
            mvaddch(this_update.y + STATUS_LINES, this_update.x,
                    world.translate(this_update.new_val));
        }
        updates.clear();
    }
    standend();  // disable whatever color attribute was used
    move(last_mouse_y, last_mouse_x);
    curs_set(1);
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
        case 'F':
            // from
            world[start_y][start_x] = PASSABLE;
            update(start_x, start_y, PASSABLE);
            start_x = last_mouse_x;
            start_y = last_mouse_y - STATUS_LINES;
            world[start_y][start_x] = START;
            astar::change_start(astar::node(start_x, start_y));
            update(start_x, start_y, START);
            break;
        case 'T':
            // goal
            world[goal_y][goal_x] = PASSABLE;
            update(goal_x, goal_y, PASSABLE);
            goal_x = last_mouse_x;
            goal_y = last_mouse_y - STATUS_LINES;
            world[goal_y][goal_x] = GOAL;
            astar::change_goal(astar::node(goal_x, goal_y));
            update(goal_x, goal_y, GOAL);
            break;
        case 'p':
            // play/pause
            play = !play;
            break;
        case 'i':
            // instant
            if (astar::done) break;
            while (!astar::tick()) {
            }
            astar::term();
            break;
        case 's':
            // step
            if (astar::tick())
                astar::term();
            break;
        case 'S':
            // larger step
            for (int i = 0; i < 5; i++) {
                if (astar::tick())
                    astar::term();
            }
            break;
        case 'c':
            // clear board
            world.clear(IMPASSABLE, PASSABLE);
            lazy_updates = false;
            break;
        case 'd':
            // toggle displaying path
            astar::path_display = !astar::path_display;
            break;
        case 'r':
            // full reset: reset astar and refill grid
            // I can't use a fallthough to avoid code duplication here because
            // we need to reset astar before being able to clear the world
            astar::reset(world);
            astar::init(astar::node(goal_x, goal_y),
                        astar::node(start_x, start_y), world);
            world.clear(IMPASSABLE, PASSABLE);
            fill_random(chance);
            play = false;
            lazy_updates = false;
            break;
        case 'R':
            // partial reset: just reset astar
            astar::reset(world);
            astar::init(astar::node(goal_x, goal_y),
                        astar::node(start_x, start_y), world);
            play = false;
            lazy_updates = false;
            break;
        case KEY_MOUSE:
            MEVENT mouse_event;
            if (getmouse(&mouse_event) == OK) {
                size_t mouse_event_x = mouse_event.x;
                size_t mouse_event_y = mouse_event.y - STATUS_LINES;
                if (mouse_event.bstate & BUTTON1_PRESSED) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event_y][mouse_event_x];
                        if (clicked == PASSABLE)
                            update(mouse_event_x, mouse_event_y, IMPASSABLE);
                        dragging_impassable = true;
                    }
                } else if (mouse_event.bstate & BUTTON1_RELEASED) {
                    dragging_impassable = false;
                } else if (mouse_event.bstate & BUTTON2_PRESSED) {
                    last_mouse_x = mouse_event.x;
                    last_mouse_y = mouse_event.y;
                } else if (mouse_event.bstate & BUTTON3_PRESSED) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event_y][mouse_event_x];
                        if (clicked == IMPASSABLE)
                            update(mouse_event_x, mouse_event_y, PASSABLE);
                        dragging_passable = true;
                    }
                } else if (mouse_event.bstate & BUTTON3_RELEASED) {
                    dragging_passable = false;
                } else if (mouse_event.bstate & REPORT_MOUSE_POSITION) {
                    if (mouse_event_y < world.height() &&
                        mouse_event_x < world.width()) {
                        int &clicked = world[mouse_event_y][mouse_event_x];
                        if (dragging_impassable && clicked == PASSABLE)
                            update(mouse_event_x, mouse_event_y, IMPASSABLE);
                        else if (dragging_passable && clicked == IMPASSABLE)
                            update(mouse_event_x, mouse_event_y, PASSABLE);
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

#include "./astar.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <fmt/core.h>
#include <forward_list>
#include <functional>
#include <vector>

#include "./grid.hpp"
#include "./logs.hpp"

namespace astar {
grid<int> *current_grid = nullptr;

double node::compute_weight(node &goal) {
    // this is a*, so

    // f(x) = g(x) + h(x)
    // ^ ^    ^      ^
    // | |    |      |
    // | |    |      \--- heuristic -- formula is called "diagonal distance"
    // | |    |
    // | |    \--- generation
    // | |
    // | \--- current node
    // \--- this whole function
    int g = _parent->_generation + 1;
    // the actual diagonal distance formula is here, with more ascii art!
    // yay
    // distance to goal
    int dx = std::abs(_x - goal._x);
    int dy = std::abs(_y - goal._y);
    double h = 1 * (dx + dy) - 1.4142135623730950488 * std::min(dx, dy);
    //     ^   ^   ^           ^                        ^
    //     |   |   |           |                        |
    //     |   |   |           |                 /------/
    //     |   |   |           |                 |
    //     |   |   |           |                 \--- number of diagonal steps
    //     |   |   |           |
    //     |   |   |           |
    //     |   |   |           \--- net cost of 1 diagonal step (-sqrt(2))
    //     |   |   |
    //     |   |   \--- horizontal distance to goal
    //     |   |
    //     |   \--- cost of horizontal step
    //     |
    //     \--- final heuristic
    return g + h;
}

node::node(int x, int y, node &goal, node *parent) {
    _x = x;
    _y = y;
    _parent = parent;
    _generation = parent->_generation + 1;
    _weight = compute_weight(goal);
}

node::node(int x, int y, node *parent) {
    _x = x;
    _y = y;
    _parent = parent;
    if (parent != 0) {
        _generation = parent->_generation + 1;
    } else {
        _generation = 0;
    }
    _weight = 0;
}
node goal(-1, -1);

bool initialized = false;

// it's a queue. sshhhh
std::vector<node> queue;
std::forward_list<node> visited;

inline bool between(int val, int low, int high) {
    return val >= low && val < high;
}

bool tick() {
    // sanity check for whether we're initialized
    if (!initialized)
        return false;
    if (queue.empty()) {
        // algorithm is done when the queue is empty
        return true;
    }

    {
        node cur = queue.back();
        if (cur.x() == goal.x() && cur.y() == goal.y()) {
            visited.push_front(cur);
            return true;
        }
        queue.pop_back();
        visited.push_front(cur);
    }

    node *cur = &visited.front();
    if ((*current_grid)[cur->y()][cur->x()] != 2)
        (*current_grid)[cur->y()][cur->x()] = 4;

    static const std::pair<int, int> dirs[8] = {
        {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    for (std::pair<int, int> dir : dirs) {
        int newx = cur->x() + dir.first;
        int newy = cur->y() + dir.second;

        if (between(newx, 0, current_grid->width()) &&
            between(newy, 0, current_grid->height())) {
            int &cur_square = (*current_grid)[newy][newx];
            if (cur_square == 0 || cur_square == 3) {
                if (cur_square == 0)
                    (*current_grid)[newy][newx] = 5;
                queue.push_back(node(newx, newy, goal, cur));
            }
        }
    }
    std::sort(queue.begin(), queue.end(), node::rev_cmp);
    return false;
}

void backtrack(grid<int> *world) {
    // finish the algorithm by finding the final added node and going ->parent
    // repeatedly
    // assuming normal execution, the front will be the goal node. :D
    node *current = &visited.front();
    while (current != nullptr) {
        int &grid_square = (*world)[current->y()][current->x()];
        if (grid_square == 4)
            grid_square = 6;
        current = current->parent();
    }
}

void weights() {
    // prints current weights to note_log
    for (node n : queue) {
        note_log << n.weight() << " ";
    }
    note_log << "\n";
}

void init(const node &_goal, const node &start, grid<int> &world) {
    goal = _goal;
    queue.push_back(start);
    current_grid = &world;
    initialized = true;
}

void term() {
    // terminate: lets go of the pointer and maybe does other things in the
    // future (resets goal?)
    current_grid = nullptr;
    goal = node(-1, -1);
    initialized = false;
}
}  // namespace astar

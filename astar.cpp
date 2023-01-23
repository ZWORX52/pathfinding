#include "astar.hpp"

#include "grid.hpp"

#include <cmath>
#include <deque>
#include <forward_list>
#include <vector>

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
    double h = 1 * (dx + dy) + (std::sqrt(2) - 2 * 1) * std::min(dx, dy);
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
    return h;
}

node::node(int x, int y, node &goal, node *parent = nullptr) {
    _x = x;
    _y = y;
    _parent = parent;
    _weight = compute_weight(goal);
}

node::node(int x, int y, node *parent) {
    _x = x;
    _y = y;
    _parent = parent;
}

node goal(-1, -1);

// it's a queue. sshhhh
std::vector<node> queue;
std::forward_list<node> visited;

bool between(int val, int low, int high) { return val >= low && val < high; }

bool tick() {
    if (queue.empty()) {
        // algorithm is done when the queue is empty
        return true;
    }
    node cur = queue.back();
    queue.pop_back();
    visited.push_front(cur);
    const static std::pair<int, int> dirs[4] = {
        {-1, 0}, {0, -1}, {1, 0}, {0, 1}};
    for (std::pair<int, int> dir : dirs) {
        int newx = cur.x() + dir.first;
        int newy = cur.y() + dir.second;
        if (newx == goal.x() && newy == goal.y())

            if (between(newx, 0, current_grid->width()) &&
                between(newy, 0, current_grid->height()) &&
                (*current_grid)[newy][newx] == 1) {
                queue.push_back(node(newx, newy, goal, &cur));
            }
    }
    return false;
}

void init(int goal_x, int goal_y, grid<int> &world) {
    goal = node(goal_x, goal_y);
    current_grid = &world;
}
}  // namespace astar

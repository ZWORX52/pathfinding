#include "astar.hpp"

#include <cmath>
#include <deque>
#include <vector>

namespace astar {
class node {
    double weight;
    int generation;
    int x, y;
    node *parent;

    double compute_weight(node &goal) {
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
        int g = parent->generation + 1;
        // the actual diagonal distance formula is here, with more ascii art!
        // yay
        // distance to goal
        int dx = std::abs(this->x - goal.x);
        int dy = std::abs(this->y - goal.y);
        double h = 1 * (dx + dy) + (std::sqrt(2) - 2 * 1) * std::min(dx, dy);
        //     ^   ^   ^           ^                        ^
        //     |   |   |           |                        |
        //     |   |   |           |            /-----------/
        //     |   |   |           |            |
        //     |   |   |           |            \--- number of diagonal steps
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

 public:
    node(int x, int y, node *parent, node &goal) {
        this->x = x;
        this->y = y;
        this->parent = parent;
        weight = compute_weight(goal);
    }

    // rocket science!
    auto operator<=>(node &other) const {
        return this->weight <=> other.weight;
    }
};

// it's a queue. sshhhh
std::vector<node> queue;

bool tick() {
    if (queue.empty()) {
        return true;
    }

    return false;
}
}  // namespace astar

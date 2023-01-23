#pragma once
#include "grid.hpp"

#include "grid.hpp"

#include <compare>

namespace astar {
class node {
    double _weight;
    int _generation;
    int _x, _y;
    node *_parent;

    double compute_weight(node &goal);

 public:
    node(int x, int y, node &goal, node *parent);
    node(int x, int y, node *parent = nullptr);

    // rocket science!
    inline auto operator<=>(node &other) const { return _weight <=> other._weight; }

    inline int x() { return _x; }
    inline int y() { return _y; }
};

bool tick();
void init(int goal_x, int goal_y, grid<int> &world);
}  // namespace astar

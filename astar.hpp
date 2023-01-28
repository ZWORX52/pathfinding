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
    inline auto operator<=>(const node &other) const {
        return _weight <=> other._weight;
    }

    inline int x() { return _x; }
    inline int y() { return _y; }
    inline double weight() { return _weight; }
    inline node *parent() { return _parent; }

    static inline bool rev_cmp(node &lhs, node &rhs) { return lhs > rhs; }
};

extern bool initialized;

bool tick();
void init(const node &_goal, const node &start, grid<int> &world);
void backtrack(grid<int> *world);
void weights();
void term();
}  // namespace astar

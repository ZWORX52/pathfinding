#pragma once
#include "grid.hpp"

#include "grid.hpp"

#include <compare>
#include <functional>

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

struct stats {
    size_t path_length;
    size_t queue_size;
    size_t explored_size;
    size_t explore_path_length;
};

extern bool initialized;
extern bool path_display;
extern bool success;
extern bool done;

bool tick();
void init(const node &_goal, const node &start, grid<int> &world);
void backtrack(grid<int> &world);
void weights();
void term();
void reset(grid<int> &world);
stats get_stats();
void change_goal(const node &_goal);
void change_start(const node &start);
}  // namespace astar

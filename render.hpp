#pragma once

extern const int PASSABLE;
extern const int IMPASSABLE;
extern const int START;
extern const int GOAL;
extern const int EXPLORED;
extern const int QUEUE;
extern const int PATH;
extern const int EXPLORE_PATH;

extern const int STATUS_LINES;

namespace render {
// contains all the code needed for rendering the grid and managing the
// pathfinding algorithms
extern bool play;

void update(int x, int y, int new_val);
void init(int height, int width, int _curs_active, double chance);
bool input();
void draw();
}  // namespace render

# pathfinding
pathfinding algorithms confined to a TUI

this will be a collection of pathfinding algorithms that you can interact with in your terminal -- right now it's just A* however. building is simple:

- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

if you want to use ninja, it's not very different; the last two steps are changed to

- `cmake .. -G Ninja`
- `ninja`

for a fully optimized build, just add `-DCMAKE_BUILD_TYPE=Release` to the `cmake ..` command.

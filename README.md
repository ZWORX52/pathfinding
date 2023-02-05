# pathfinding
pathfinding algorithms confined to a TUI

this will be a collection of pathfinding algorithms that you can interact with in your terminal -- right now it's just A* however. building is simple:

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`

if you want to use `ninja`, it's not very different; the last two steps are changed to

3. `cmake .. -G Ninja`
4. `ninja`

for a fully optimized build, just add `-DCMAKE_BUILD_TYPE=Release` to the `cmake ..` command.

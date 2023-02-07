# pathfinding

pathfinding algorithms confined to a TUI

this will be a collection of pathfinding algorithms that you can interact with in your terminal -- right now it's just A* however. building is simple:

1. `mkdir build`
2. `cd build`
3. `cmake .. --preset default`
4. `ninja`

## development

feel free to contribute! if you have a guess as to what the next performance bottleneck is, please file an issue; if you want to fix it, go right ahead :D

please note that if you want cmake to emit a debugging-friendly build file, just specify `--preset debug` in the cmake command, and you can just `ninja` away.

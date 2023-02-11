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

## controls

| key | pneumonic | effect |
| --- | --------- | ------ |
| `q` | **q**uit | exits program |
| `F` | **f**rom | sets starting point to cursor location (see mm) |
| `T` | **t**o | sets goal to cursor location (see mm) |
| `p` | **p**lay | starts/stops animation |
| `i` | **i**nstant | skips animation; takes a moment |
| `s` | **s**tep | executes one step of the algorithm |
| `S` | big **s**tep | executes five steps of the algorithm |
| `c` | **c**lear | clears board of obstructions |
| `d` | **d**isplay | toggles displaying the explore path |
| `r` | **r**eset | resets astar and regenerates the grid |
| `R` | partial **r**eset | resets astar but keeps the grid |
| lm | left mouse | makes the square at the mouse position impassible (draggable) |
| mm | middle mouse | moves the cursor used to specially manipulate squares to the mouse position |
| rm | right mouse | makes the square at the mouse position passable (draggable) |

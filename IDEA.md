5 machines needed to recreate the supply chain of a computer in a
post-apocalyptic world so that you can finish the apocalypse.

Devices called automatons. From the old world, can work but need some tinkering
to fix.

Need 4 types to build components for a computer. And an assembler to put it
together.

For each automaton need automaton frame of that type

Need tools: wrench, screwdriver, solder, pliers, hammer

Need components: mechanical components, electrical components, junk

Start game -> Home base -> Adventure - spawn in a building -> Go up floors,
collect resources. Kill rats, zombies, depraved, robots, ????? (if there's time)
-> Go up to the last floor, then back to the base. -> Make as many trips as you
need to build 5 automata needed to build a computer. -> Build a computer ->
Press a button -> Game Over -> ????? (if there's time).

Simple stats:
Armor
Evasion
Damage
Speed

2 sensible builds: Damage + armor or speed + evasion

# Architecture

* OpenGL draws console like tiles using cp437 texture atlas.
* Simple UI using the same, but can pass start location and a string. iterate
    through chars until null from there
* Simple homebrewed ECS
* A star
* Projectiles if there's time
* No audio

# Plan

* [x] Engine cleanup
* [x] Player and movement
* [x] Map, level, rect room, basic map state, collisions with map
* [x] Entities
* [x] Enemiy random move AI
* [x] Collisions between entities
* [x] Simple follow player AI
* [x] Map procgen - Square map BSP
* [x] Map procgen - Read prefabs from file
* [x] Map procgen - Populate BSP rooms with prefabs
* [x] BFS Pathfinding
* [x] Better follow player AI
* [ ] FOV
* [ ] Combat
* [ ] Try to build on diff machines, on windows. spend a couple hrs to see if possible
* [ ] ---> Iter 1 (single room level w/ combat)
* [ ] Map procgen - Cellular automata algorithm to make some rooms destroyed
* [ ] Basic UI
* [ ] Multiple levels
* [ ] Item pickups
* [ ] Progression
* [ ] ---> Iter 2 (single building branch w/ progression and items)
* [ ] Base location; persistent
* [ ] Machine entities
* [ ] Tie up with items
* [ ] Game flow: base location<->branches
* [ ] ---> Iter 3
* [ ] More game flow
* [ ] Loose ends
* [ ] More input refactoring, key repeat, diagonal movement (maybe?)
* [ ] Game over (won or lost)
* [ ] ---> Iter 4 (shippable)
* [ ] Map procgen - More prefabs
* [ ] Mechanics Improvements
* [ ] Extension to have a more interesting plot/twist

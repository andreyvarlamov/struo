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
* [x] Clean up and refactoring
* [x] Enemey LOS
* [x] Player FOV
* [x] Opaque/transparent
* [x] Map memory
* [x] Combat
* [x] ---> Iter 1 (single room level w/ combat)
* [x] Basic UI
* [x] Log
* [x] Player stat HUD
* [x] Non-character entities
* [x] Item pickups
* [x] Item pickup UI
* [x] Equipment in combat
* [x] Equipment UI
* [x] Implement actual item game logic
* [x] Story items
* [x] Item RNG
* [x] Multiple levels
* [x] Level exits
* [x] Level UI
* [x] Progression
* [x] ---> Iter 2 (single building branch w/ progression and items)
* [x] Base location
* [x] Machine entities
* [x] Machine plans and UI
* [x] Build machines
* [x] Machine tie up with items
* [x] Built machines UI
* [x] Built machines interactions and UI
* [x] Craft computer
* [x] Game over (won)
* [x] Do something about cheats
* [x] Game start
* [x] Base location persistency & work with other levels
* [x] Generate building and scavenge
* [x] Start splash screen
* [x] Test frame items
* [x] Map procgen - lobby prefab only on floor 1
* [x] Player random location
* [x] Adjust base machine colors
* [x] ---> Iter 3 (shippable)
* [ ] Auto explore
* [ ] Test and tweak the whole game flow
* [ ] ---> Iter 4 (polished)


NO WAY:
* [ ] Map procgen - prefab translation and BSP flip
* [ ] Extension to have a more interesting plot/twist
* [ ] Map procgen - Cellular automata algorithm to make some rooms destroyed
* [ ] Timing system
* [ ] More input refactoring, key repeat, diagonal movement (maybe?)
* [ ] Better AI (follow to last known location)
* [ ] Mechanics Improvements

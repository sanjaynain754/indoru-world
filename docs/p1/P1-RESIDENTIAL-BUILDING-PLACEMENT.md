# P1 Residential Building Placement

This layer turns household simulation into physical world construction targets.

## Pipeline
Population -> households -> residential units -> physical buildings -> placement cells -> World Partition.

## Building coverage
The production world must contain physical residential buildings rather than abstract housing counters. Building types include detached houses, row houses, apartments, mixed-use residential buildings and towers.

## Important
A household occupies a residential unit. A building may contain many units, and a unit may contain a family of several people.

The C++ planner is a foundation only. UE5 production will add procedural/hand-authored meshes, interiors, furniture, utilities, doors, elevators, parking, roads, navmesh, LOD/HLOD and streaming.

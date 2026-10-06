# P1 Residential + Household + Daily Life Foundation

## Locked rule
Indoru World does not assign one house per person. Residential ownership/occupancy is household-based.

Example: a single residential unit may contain a family of 3, 4, 7, 10 or 11+ people. Multiple units can exist inside one apartment building.

## Population target
Initial production target: **1,000,000 simulated population**.
The architecture remains scale-ready for 5,000,000 and 10,000,000.

## Night behavior
The baseline simulation target is **90% of population residential at night**. The remaining population is distributed across night-shift work, emergency services, transport, security, nightlife, travel and other valid activities. This is a simulation target, not a visual requirement that exactly 90% be rendered every frame.

## Hierarchy
NPC -> Household -> Residential Unit -> Building -> Neighborhood -> City -> Country

## Implementation
The C++ foundation validates household assignment, residential capacity, night distribution, and schedules crossing midnight.

This is the simulation/data layer. Final UE5 residential meshes, interiors, furniture, animation sets and building art are separate production work.

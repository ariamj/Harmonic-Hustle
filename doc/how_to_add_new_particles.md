To add another particle...

DATA
In `shaders`:
Upload .vs and .fs glsl shader files

In `data/textures` folders
Add .png texture file

In `components.hpp`:
Increment enums `TEXTURE_ASSET_ID` and `EFFECT_ASSET_ID` and `PARTICLE_TYPE_ID`


BEHAVIOUR
Create a new `xxxxx_particle_generator.hpp` and `.cpp` file. Use trail_particle_generator files as a guide.

In the new `xxxxx_particle_generator.cpp` file:
Override `Update` and `respawnParticles` to achieve the desired behaviour.


INITIALIZING
In `render_system_init.cpp`:
`initializeParticleGenerators`: Set the particle shader program's projection matrix (the same for all particle shaders) 

In `render_system.cpp`:
`createParticleGenerator`: Add a new case for the new particle type enum.

In `world_init.cpp` (and .hpp if necessary):
If NOT attaching particles to any existing entity:
	Create a new function `createXXXXX` and declare it in the .hpp file
In `createXXXXX` for the desired entity (or dummy entity):
	Make sure the entity has both a `ParticleEffect` and a `Motion` component
	Call `renderer->createParticleGenerator()` with the desired particle type enum and associated entity type
FILES
In `shaders`:
Upload .vs and .fs glsl shader files

In `data/textures` folders
Add .png texture file

In `components.hpp`:
Increment enums `TEXTURE_ASSET_ID` and `EFFECT_ASSET_ID` and `PARTICLE_TYPE_ID`

In `src/`:
Create a new `xxxxx_particle_generator.hpp` and `.cpp` file. Use trail_particle_generator files as a guide.


INITIALIZING
In `render_system.hpp`:
Add an #include for the new `xxxxx_particle_generator.hpp` file.
Add a texture_path(.png file name) to `textures_paths` (array starts around line 40)
Add a shader_path(.vs/.fs file name, without extensions) to `effect_paths` (array starts around line 60)

In `render_system_init.cpp`:
In `initializeParticleGenerators`: Set the particle shader program's projection matrix (the same for all particle shaders) 

In `render_system.cpp`:
In `createParticleGenerator`: Add a new case for the new particle type enum.


IMPLEMENTATION
In the new `xxxxx_particle_generator.cpp` file:
Override `updateParticleBehaviours` and `respawnParticles` to achieve the desired behaviour.


ATTACHING TO ENTITIES
In `world_init.cpp` (and .hpp if necessary):
If NOT attaching particles to any existing entity:
	Create a new function `createXXXXX` and declare it in the .hpp file
In `createXXXXX` for the desired entity (or dummy entity):
	Make sure the entity has both a `ParticleEffect` and a `Motion` component
	Call `renderer->createParticleGenerator(...)` with the desired particle type enum and associated entity type

To add particles to more entity types, simply call `renderer->createParticleGenerator(...)` as above when the entities are created.
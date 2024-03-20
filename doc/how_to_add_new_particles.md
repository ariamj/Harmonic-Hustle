To add another particle...

In `shaders`:
Upload .vs and .fs glsl shader files

In `data/textures` folders
Add .png texture file

In `components.hpp`:
Increment enums `TEXTURE_ASSET_ID` and `EFFECT_ASSET_ID`
Increment enum `PARTICLE_TYPE_ID`

In `render_system_init.cpp`:
Inside `initializeParticleGenerators`, set the particle shader program's projection matrix (the same for all particle shaders) 

In `world_init.cpp`:
If NOT attaching particles to any existing entity:
	Create a new function `createEntityName` that creates a new dummy entity `EntityName` with a `ParticleEffect` and `Motion` component (useful for position)
In `createEntityName` for the desired entity (or dummy entity):
	Add a `ParticleEffect` component to the entity type
	Call `renderer->createParticleGenerator()` with the desired particle type enum and associated entity type

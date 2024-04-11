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
Add a texture_path(.png file name) to `textures_paths`
Add a shader_path(.vs/.fs file name, without extensions) to `effect_paths`
  
In `render_system_init.cpp`:  
In `initializeParticleGenerators`: Set the particle shader program's projection matrix (the same for all particle shaders)   
  
In `render_system.cpp`:  
In `createParticleGenerator`: Add a new case for the new particle type enum.  
  
In relevant scenes (currently only `battle_screen.cpp`):  
Call `renderer->createParticleGenerator(...)` with the desired particle type enum  
Call `renderer->particle_generators.clear()` before exiting the scene to free memory.  
  
  
IMPLEMENTATION
In the new `xxxxx_particle_generator.cpp` file:  
Override `updateParticleBehaviours` and `respawnParticles` to achieve the desired behaviour.  
Override `getType` to return the correct `PARTICLE_TYPE_ID` enum.  

  
ATTACHING TO ENTITIES  
In `world_init.cpp`:  
If NOT attaching particles to any existing entity:  
	Create a new function `createXXXXX` and declare it in the `world_init.hpp` file  
In `createXXXXX` for the desired entity (or dummy entity):  
	Make sure the entity has both a `ParticleEffect` and a `Motion` component  
	Make sure to set the `ParticleEffect.type` to the correct enum. 
	  
	  
NOT WORKING?  
Make sure to remove entities from the particles registry at some point.  
	Examples: On collision, on leaving screen, on a timer (ParticleTimer component)  
	Otherwise, only MAX_PARTICLE_ENTITIES will have particles, and not any entities afterwards.  
If particles are not appearing, make sure that scale is not zero, and color.a is not zero on respawn  

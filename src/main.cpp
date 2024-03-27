#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "world_system.hpp"
#include "overworld_screen.hpp"
#include "battle_screen.hpp"
#include "settings_screen.hpp"
#include <audio_system.hpp>
#include "physics_system.hpp"
#include "ai_system.hpp"
#include "iostream"

using Clock = std::chrono::high_resolution_clock;

int main() {
    
    // Global systems
    WorldSystem world;
    RenderSystem renderSystem;
    PhysicsSystem physics;
    AISystem ai_system;

    // Initialize window
    GLFWwindow* window = world.create_window();
    if (!window) {
        // Error message
        printf("Press any key to exit");
        getchar();
        return EXIT_FAILURE;
    }

    // Initialize main systems
    
    renderSystem.init(window);
    world.init(&renderSystem);

    // Variables for counting frames

    int frames = 0;
    float ms_count = 0.f;
    float fps_update_delay = 200.f;
    float fps_ratio = 1000.f / fps_update_delay;

    // variable timestep loop
    auto t = Clock::now();
    while (!world.is_over()) {
        // Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

        // Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
        
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;


        // Real version
        world.step(elapsed_ms);
        ai_system.step(elapsed_ms);
        physics.step(elapsed_ms, &renderSystem);
        world.handle_collisions();
        renderSystem.draw();

        // DEBUG RUNTIME version (duplicated steps, make sure one version is commented out )
        //auto pre_world = Clock::now();
        //world.step(elapsed_ms);
        //auto post_world = Clock::now();
        //float world_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(post_world - pre_world)).count() / 1000;

        //auto pre_ai = Clock::now();
        //ai_system.step(elapsed_ms);
        //auto post_ai = Clock::now();
        //float ai_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(post_ai - pre_ai)).count() / 1000;

        //auto pre_physics = Clock::now();
        //physics.step(elapsed_ms, &renderSystem);
        //auto post_physics = Clock::now();
        //float physics_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(post_physics - pre_physics)).count() / 1000;

        //auto pre_collisions = Clock::now();
        //world.handle_collisions();
        //auto post_collisions = Clock::now();
        //float collisions_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(post_collisions - pre_collisions)).count() / 1000;

        //auto pre_render = Clock::now();
        //renderSystem.draw();
        //auto post_render = Clock::now();
        //float render_ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(post_render - pre_render)).count() / 1000;

        //std::cout << "World: " << world_ms << ", Physics:" << physics_ms << ", Render:" << render_ms << "\n";

        frames += 1;
        ms_count += elapsed_ms;

        if (ms_count > fps_update_delay) {
            ms_count = 0.f;
            FPS = frames * fps_ratio; // update global variable
            frames = 0;
        }

    }

    return EXIT_SUCCESS;
}
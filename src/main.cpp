#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "world_system.hpp"
#include <audio_system.hpp>

using Clock = std::chrono::high_resolution_clock;

int main() {
    
    // Global systems
    WorldSystem world;
    RenderSystem renderSystem;
    AudioSystem audioSystem;

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
    audioSystem.init();

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
    }

    return EXIT_SUCCESS;
}
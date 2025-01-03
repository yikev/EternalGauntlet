
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>
#include <thread>

// internal
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

using Clock = std::chrono::high_resolution_clock;

// Entry point
	int main()
	{
		// Global systems
		WorldSystem world;
		RenderSystem renderer;
		PhysicsSystem physics;

		// Initializing window
		GLFWwindow* window = world.create_window();
		if (!window) {
			// Time to read the error message
			printf("Press any key to exit");
			getchar();
			return EXIT_FAILURE;
		}
		// initialize the main systems
		renderer.init(window);
		world.init(&renderer);

		// font init
		std::string font_filename = PROJECT_SOURCE_DIR + std::string("data/fonts/Kenney_Pixel_Square.ttf");
		unsigned int font_default_size = 48;
		renderer.fontInit(font_filename, font_default_size);


		// Print the map
		world.printMap();

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

			world.step(elapsed_ms);
			physics.step(elapsed_ms);
			world.handle_collisions();

			renderer.draw();

			// 45 fps per second
			const float target_frame_time_ms = 1000.0f / 60.0f;


			// Limit fps
			auto frame_end = Clock::now();
			float frame_duration_ms = std::chrono::duration<float, std::milli>(frame_end - t).count();

			if (world.getCurrentGameState() == GameState::PLAYING && frame_duration_ms < target_frame_time_ms) {
				std::this_thread::sleep_for(std::chrono::milliseconds((long)(target_frame_time_ms*2 - frame_duration_ms)));
			}
			else {
				if (frame_duration_ms < target_frame_time_ms) {
					std::this_thread::sleep_for(std::chrono::milliseconds((long)(target_frame_time_ms - frame_duration_ms)));
				}
			}
		}

		return EXIT_SUCCESS;
	}

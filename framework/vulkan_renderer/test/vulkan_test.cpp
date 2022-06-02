#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>
#include <mm/services/vulkan_renderer.hpp>

#include <mm/logger.hpp>

TEST(sdl_service, window_vulkan) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	// create window
	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
	ASSERT_TRUE(sdl_ss_ptr->createWindow("test vulkan window", 800, 600, SDL_WINDOW_VULKAN));
	ASSERT_NE(sdl_ss_ptr->win, nullptr);

	engine.addService<MM::Services::VulkanRenderer>();
	ASSERT_TRUE(engine.enableService<MM::Services::VulkanRenderer>());

	auto& vk_rend = engine.getService<MM::Services::VulkanRenderer>();
	ASSERT_TRUE(vk_rend.createDevice(engine));
	ASSERT_TRUE(vk_rend.createSwapchain(engine));

	engine.run();

	engine.disableService<MM::Services::VulkanRenderer>();
	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}


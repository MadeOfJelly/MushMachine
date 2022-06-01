#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>

#include <SDL_vulkan.h>

#include <volk.h>

TEST(sdl_service, window_volk) {
	MM::Engine engine;

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	// init volk
	auto init_res = volkInitialize();
	ASSERT_EQ(init_res, VK_SUCCESS);


	// create window
	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
	ASSERT_TRUE(sdl_ss_ptr->createWindow("test vulkan window", 800, 600, SDL_WINDOW_VULKAN));
	ASSERT_NE(sdl_ss_ptr->win, nullptr);

	// create vulkan instance

	// Get the required extension count
	unsigned int count;
	ASSERT_TRUE(
		SDL_Vulkan_GetInstanceExtensions(
			sdl_ss_ptr->win,
			&count,
			nullptr
		)
	);

	std::vector<const char*> extensions {
		//VK_EXT_DEBUG_REPORT_EXTENSION_NAME // Sample additional extension
	};
	size_t additional_extension_count = extensions.size();
	extensions.resize(additional_extension_count + count);

	ASSERT_TRUE(
		SDL_Vulkan_GetInstanceExtensions(
			sdl_ss_ptr->win,
			&count,
			extensions.data() + additional_extension_count
		)
	);

	// Now we can make the Vulkan instance
	VkInstanceCreateInfo create_info {};
	//create_info.pApplicationInfo;
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	VkInstance instance;
	VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
	ASSERT_EQ(result, VK_SUCCESS);

	// finish setting up volk ?
	volkLoadInstance(instance);

	// the surface for the window
	VkSurfaceKHR surface;
	ASSERT_TRUE(
		SDL_Vulkan_CreateSurface(
			sdl_ss_ptr->win,
			instance,
			&surface
		)
	);

	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}


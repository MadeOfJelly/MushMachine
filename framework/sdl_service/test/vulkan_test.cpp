#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>
#include <vector>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_structs.hpp> // mf
#include <vulkan/vulkan_enums.hpp>

#include <SDL_vulkan.h>

#include <mm/logger.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void setup_dispacher(void) {
	// TODO: investigate why
	// TODO: use SDL?
	vk::DynamicLoader dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

void setup_instance_dispatcher(const vk::Instance& instance) {
	// initialize function pointers for instance
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
}

bool can_use_layer(std::string_view layer_want) {
	for (const auto& layer : vk::enumerateInstanceLayerProperties()) {
		if (static_cast<std::string_view>(layer.layerName) == layer_want) {
			return true;
		}
	}

	return false;
}

bool can_use_validation(void) {
	return can_use_layer("VK_LAYER_KHRONOS_validation");
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/
) {
	spdlog::level::level_enum level{};
	switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			level = spdlog::level::level_enum::debug;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			level = spdlog::level::level_enum::info;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			level = spdlog::level::level_enum::warn;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			level = spdlog::level::level_enum::err;
			break;
		default:
			level = spdlog::level::level_enum::critical; // what ever
	}

	switch (messageType) {
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			spdlog::get("VulkanGeneral")->log(level, "{}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			spdlog::get("VulkanValidation")->log(level, "{}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			spdlog::get("VulkanPerformance")->log(level, "{}", pCallbackData->pMessage);
			break;
	}

	return VK_FALSE;
}

static const vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info{
	{},
	vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
	| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
	| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
	| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
	vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
	| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
	| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
	debug_callback
};

vk::Instance create_instance(
	const vk::ApplicationInfo& app_info,
	std::vector<const char*> extensions = {},
	std::vector<const char*> layers = {}
) {
	// for debugging
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	// Get the required extension count
	unsigned int count;
	if (SDL_Vulkan_GetInstanceExtensions(nullptr, &count, nullptr) != SDL_TRUE) {
		return nullptr;
	}

	size_t additional_extension_count = extensions.size();
	extensions.resize(additional_extension_count + count);

	// fill sdl extensions
	if (SDL_Vulkan_GetInstanceExtensions(nullptr, &count, extensions.data() + additional_extension_count) != SDL_TRUE) {
		return nullptr;
	}

	// Now we can make the Vulkan instance
	vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> c{
		vk::InstanceCreateInfo{
			{},
			&app_info,
			static_cast<uint32_t>(layers.size()),
			layers.data(),
			static_cast<uint32_t>(extensions.size()),
			extensions.data()
		},
		debug_utils_messenger_create_info
	};

	vk::Instance instance = vk::createInstance(c.get(), nullptr);

	setup_instance_dispatcher(instance);

	return instance;
}

void setup_device_dispatcher(const vk::Device& device) {
	// function pointer specialization for device
	VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
}


TEST(sdl_service, window_vulkan) {
	MM::Engine engine;
	{ // setup vulkan loggers
#if 0
		MM::Logger::initSectionLogger("VulkanGeneral");
		// way too noisy otherwise
		spdlog::get("VulkanGeneral")->set_level(spdlog::level::level_enum::warn);
#else
		// or just dont log to stdio?
		MM::Logger::initSectionLogger("VulkanGeneral", false);
#endif
		MM::Logger::initSectionLogger("VulkanValidation");
		MM::Logger::initSectionLogger("VulkanPerformance");
	}

	engine.addService<MM::Services::SDLService>();
	ASSERT_TRUE(engine.enableService<MM::Services::SDLService>());

	auto* sdl_ss_ptr = engine.tryService<MM::Services::SDLService>();
	ASSERT_NE(sdl_ss_ptr, nullptr);

	setup_dispacher();

	// create window
	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
	ASSERT_TRUE(sdl_ss_ptr->createWindow("test vulkan window", 800, 600, SDL_WINDOW_VULKAN));
	ASSERT_NE(sdl_ss_ptr->win, nullptr);

	// create vulkan instance

	const vk::ApplicationInfo app_info {
		"app_name",
		VK_MAKE_VERSION(1, 0, 0), // app version
		"MushMachine",
		// TODO: engine version macro or something
		VK_MAKE_VERSION(0, 8, 0), // engine version
		VK_API_VERSION_1_1
	};

	// TODO: make validation layer conditional
	std::vector<const char*> layers{};
	if (can_use_validation()) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
		SPDLOG_INFO("ENABLED validation layer");
	} else {
		SPDLOG_INFO("validation layer NOT AVAILABLE!");
	}

	vk::Instance instance = create_instance(
		app_info,
		{},
		layers
	);
	ASSERT_NE(static_cast<VkInstance>(instance), nullptr);

	auto debug_messenger = instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);

	// the surface for the window (no device dependent?)
	VkSurfaceKHR surface;
	ASSERT_TRUE(
		SDL_Vulkan_CreateSurface(
			sdl_ss_ptr->win,
			instance,
			&surface
		)
	);

	// create a dispatcher, based on additional vkDevice/vkGetDeviceProcAddr
	auto physicalDevices = instance.enumeratePhysicalDevices();
	ASSERT_TRUE(!physicalDevices.empty()); // make test fail on unsupported machines

	// list devices
	for (const auto& ph_device : physicalDevices) {
		auto props = ph_device.getProperties();
		SPDLOG_INFO(
			"found device: [{}] ({}) '{}'",
			props.deviceID,
			(props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? "discrete" : "other"),
			props.deviceName
		);
	}

	auto& selected_phys_dev = physicalDevices.front();
	for (const auto& fam_props : selected_phys_dev.getQueueFamilyProperties()) {
		auto test_bit = [](const auto& flags, const auto& bit) -> bool {
			return (flags & bit) == bit;
		};
		SPDLOG_INFO(
			"QueueFamily: queueCount:{} graphics:{} compute:{} transfer:{}",
			fam_props.queueCount,
			test_bit(fam_props.queueFlags, vk::QueueFlagBits::eGraphics) ? "true" : "false",
			test_bit(fam_props.queueFlags, vk::QueueFlagBits::eCompute) ? "true" : "false",
			test_bit(fam_props.queueFlags, vk::QueueFlagBits::eTransfer) ? "true" : "false"
		);
	}

	const float queue_prio = 1.f; // hmmmm
	vk::DeviceQueueCreateInfo graphics_queue_create_info {
		{},
		0, // just pick the first one for now
		1, // count
		&queue_prio
	};

	vk::PhysicalDeviceFeatures device_features {
	};

	vk::DeviceCreateInfo device_create_info {
		{},
		1,
		&graphics_queue_create_info,
		// layers
		0,
		nullptr,
		// extensions
		0,
		nullptr,
		&device_features
	};
	vk::Device device = selected_phys_dev.createDevice(device_create_info, nullptr);
	ASSERT_NE(static_cast<VkDevice>(device), nullptr);

	setup_device_dispatcher(device);

	vk::Queue graphics_queue = device.getQueue(0, 0); 

	// cleanup
	device.destroy();

	instance.destroy(surface);
	instance.destroy(debug_messenger);
	instance.destroy();

	engine.disableService<MM::Services::SDLService>();

	ASSERT_EQ(sdl_ss_ptr->win, nullptr);
}


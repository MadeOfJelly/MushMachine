#include <gtest/gtest.h>

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_structs.hpp> // mf
#include <vulkan/vulkan_enums.hpp>
#include "spdlog/spdlog.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

#include <SDL_vulkan.h>

#include <mm/logger.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// create a dispatcher, based on additional vkDevice/vkGetDeviceProcAddr
void setup_dispacher(void) {
#if 1
	// investigate why this stopped working
	static vk::DynamicLoader dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
#else
	auto load_res = SDL_Vulkan_LoadLibrary(nullptr);
	assert(load_res == 0);
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
#endif

	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
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

	// initialize function pointers for instance
	VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

	return instance;
}

namespace MM::Services {

class VulkanRenderer : public Service {
	private:
		VkInstance _instance{};
		VkDebugUtilsMessengerEXT _debug_messenger{};

		VkSurfaceKHR _surface{};

		VkPhysicalDevice _physical_device{};
		VkDevice _device{};

		VkQueue _graphics_queue{};
		//VkQueue _present_queue{};
		VkSwapchainKHR _swapchain{};

	public:
		VulkanRenderer(void) {
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

			SPDLOG_INFO("constructed VulkanRenderer");
		}
		~VulkanRenderer(void) {};

		bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override {
			assert(!VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceLayerProperties);
			setup_dispacher();
			assert(VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceLayerProperties);

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
			_instance = instance;

			_debug_messenger = instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);

			return true;
		}

		void disable(Engine&) override {
			// cleanup
			if (_device) {
				vk::Device device{_device};
				device.destroy(_swapchain);
				device.destroy();
			}

			vk::Instance instance{_instance};
			instance.destroy(_surface);
			instance.destroy(_debug_messenger);
			instance.destroy();
		}

		const char* name(void) override { return "VulkanRenderer"; }

	public:
		bool createDevice(Engine& engine) {
			// the surface for the window (not device dependent)
			if (SDL_Vulkan_CreateSurface(engine.getService<SDLService>().win, _instance, &_surface) != SDL_TRUE) {
				SPDLOG_ERROR("creating vulkan surface from window. (is the SDL_WINDOW_VULKAN flag set?)");
				return false;
			}

			// convenience hpp wrapper
			assert(_surface);
			vk::SurfaceKHR surface{_surface};
			assert(_instance);
			vk::Instance instance{_instance};

			auto physical_devices = instance.enumeratePhysicalDevices();
			if (physical_devices.empty()) {
				SPDLOG_ERROR("no physical vulkan devices found");
				return false;
			}

			// list devices
			for (const auto& ph_device : physical_devices) {
				auto props = ph_device.getProperties();
				SPDLOG_INFO(
					"found device: [{}] ({}) '{}'",
					props.deviceID,
					(props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? "discrete" : "other"),
					props.deviceName
				);
			}

			auto& selected_phys_dev = physical_devices.front();
			_physical_device = selected_phys_dev;

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

			uint32_t queue_fam_index = 0;

			// test for support for swapchain
			if (selected_phys_dev.getSurfaceSupportKHR(queue_fam_index, surface) != VK_TRUE) {
				SPDLOG_ERROR("selected device does not support the surface");
				return false;
			}

			const float queue_prio = 1.f; // hmmmm
			vk::DeviceQueueCreateInfo graphics_queue_create_info {
				{},
				queue_fam_index, // just pick the first one for now
				1, // count
				&queue_prio
			};

			vk::PhysicalDeviceFeatures device_features {
			};

			// do i need this?
			std::vector<const char*> device_extentions{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			vk::DeviceCreateInfo device_create_info {
				{},
				1,
				&graphics_queue_create_info,
				// layers
				0,
				nullptr,
				// extensions
				static_cast<uint32_t>(device_extentions.size()),
				device_extentions.data(),
				&device_features
			};
			vk::Device device = selected_phys_dev.createDevice(device_create_info, nullptr);
			_device = device;

			// function pointer specialization for device
			VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

			//_present_queue = device.getQueue(0, 0);
			// we assume it also does present
			_graphics_queue = device.getQueue(0, 0);

			return true;
		}

		bool createSwapchain(Engine& engine) {
			assert(_physical_device);
			vk::PhysicalDevice physical_device {_physical_device};
			assert(_device);
			vk::Device device {_device};

			vk::SurfaceFormatKHR swap_surf_format {
				vk::Format::eB8G8R8A8Srgb,
				vk::ColorSpaceKHR::eSrgbNonlinear,
			};
			{ // select format
				//for (const auto& format : selected_phys_dev.getSurfaceFormatsKHR(surface)) {
					//if (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
						//if (format.format == vk::Format::eB8G8R8A8Srgb) {

						//}
					//}
				//}
			}

			vk::Extent2D surface_extent {};
			{
				int w, h;
				SDL_Vulkan_GetDrawableSize(engine.getService<SDLService>().win, &w, &h);
				surface_extent.width = w;
				surface_extent.height = h;
			}

			auto phys_surf_caps = physical_device.getSurfaceCapabilitiesKHR(_surface);

			// flush all loggers
			spdlog::apply_all([](const auto& logger) { logger->flush(); });

			assert(VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateSwapchainKHR);

			// create the swapchain
			_swapchain = device.createSwapchainKHR({
				{},
				_surface,
				phys_surf_caps.minImageCount,
				swap_surf_format.format,
				swap_surf_format.colorSpace,
				surface_extent,
				1, // imageArrayLayers
				vk::ImageUsageFlagBits::eColorAttachment,
				vk::SharingMode::eExclusive,
				// TODO: fill in rest
			});

			auto images = device.getSwapchainImagesKHR(_swapchain);
			SPDLOG_INFO("have {} swapchain images", images.size());

			return true;
		}
};

} // MM::Services

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


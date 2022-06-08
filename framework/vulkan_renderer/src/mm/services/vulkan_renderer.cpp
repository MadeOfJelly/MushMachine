#include "./vulkan_renderer.hpp"

#include <mm/engine.hpp>
#include <mm/services/sdl_service.hpp>

#include <vulkan/vulkan.hpp>

// mf ycm, FIXME: remove before merge
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>

#include <SDL_vulkan.h>

// HACK
namespace { // dont leak linkage
#include <mm/vulkan/res/tut1.vert.spv.h>
#include <mm/vulkan/res/tut1.frag.spv.h>

// meh, we dont have the type, only the value
//static_assert(alignof(tut1_vert_spv) == alignof(uint32_t));
//static_assert(alignof(tut1_frag_spv) == alignof(uint32_t));
}

#include <mm/logger.hpp>

// https://youtu.be/eaKeeRngZBo

// this needs to be defined only once
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// create a dispatcher, based on additional vkDevice/vkGetDeviceProcAddr
static void setup_dispacher(void) {
	// both should work
	// but vk::DynamicLoader reloads the dll, so it will be open more then once
	// and also might be a different one from sdl
#if 0
	static vk::DynamicLoader dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
#else
	auto load_res = SDL_Vulkan_LoadLibrary(nullptr);
	assert(load_res == 0);
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
#endif

	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

static bool can_use_layer(std::string_view layer_want) {
	for (const auto& layer : vk::enumerateInstanceLayerProperties()) {
		if (static_cast<std::string_view>(layer.layerName) == layer_want) {
			return true;
		}
	}

	return false;
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

static vk::Instance create_instance(
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

VulkanRenderer::VulkanRenderer(void) {
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

VulkanRenderer::~VulkanRenderer(void) {
}

bool VulkanRenderer::enable(Engine&, std::vector<UpdateStrategies::TaskInfo>& task_array) {
	assert(!VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceLayerProperties);
	setup_dispacher();
	assert(VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceLayerProperties);

	// TODO: user configurable
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

	if (can_use_layer("VK_LAYER_KHRONOS_validation")) {
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

	// TODO: dont require this?
	_debug_messenger = instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);

	_swapchain_curr_idx = 0; // important

	{ // add task
		task_array.push_back(
			UpdateStrategies::TaskInfo{"VulkanRenderer::render"}
			.phase(UpdateStrategies::update_phase_t::POST) // *smirk*
			.fn([this](Engine& e){ this->render(e); })
		);
	}

	return true;
}

void VulkanRenderer::disable(Engine&) {
	// cleanup
	if (_device) {
		vk::Device device{_device};

		auto device_destroy_each = [&device](auto& container) {
			for (const auto& it : container) {
				device.destroy(it);
			}
		};

		auto device_destroy_each_kv = [&device](auto& map) {
			for (const auto& it : map) {
				device.destroy(it.second);
			}
		};

		device.waitIdle();

		// resouce cache
		device_destroy_each_kv(_r_shader_module);

		device_destroy_each(_swapchain_framebuffers);
		device_destroy_each(_swapchain_image_views);
		device.destroy(_swapchain);
		device_destroy_each(_swapchain_sem_image_available);
		device_destroy_each(_swapchain_sem_render_finished);
		device_destroy_each(_swapchain_fence_in_flight);
		device.destroy();
	}

	vk::Instance instance{_instance};
	instance.destroy(_surface);
	instance.destroy(_debug_messenger);
	instance.destroy();
}

void VulkanRenderer::render(Engine&) {
	vk::Device device{_device};
	vk::SwapchainKHR swapchain{_swapchain};

	// wait for next fb/img/img_view to be free again
	// in most cases there are 2 but might be 1 or more
	vk::Fence in_flight{_swapchain_fence_in_flight.at(_swapchain_curr_idx)};
	auto wait_in_flight_res = device.waitForFences(in_flight, true, UINT64_MAX);
	device.resetFences(in_flight);

	uint32_t next_img_index = device.acquireNextImageKHR(
		_swapchain,
		UINT64_MAX,
		_swapchain_sem_image_available.at(_swapchain_curr_idx)
	).value;

	{
		auto g_queue = vk::Queue{_graphics_queue};
		// do the commands n stuff

		// queue submit
		vk::Semaphore tmp_sem_wait{_swapchain_sem_image_available.at(_swapchain_curr_idx)};
		vk::PipelineStageFlags tmp_sem_wait_stages{vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::Semaphore tmp_sem_sig{_swapchain_sem_render_finished.at(_swapchain_curr_idx)};
		g_queue.submit({vk::SubmitInfo{
			tmp_sem_wait,
			tmp_sem_wait_stages,
			{},
			tmp_sem_sig
		}}, _swapchain_fence_in_flight.at(_swapchain_curr_idx));
	}

	{ // queue present
		auto p_queue = vk::Queue{_graphics_queue}; // TODO: present queue

		vk::Semaphore tmp_sem_wait{_swapchain_sem_render_finished.at(_swapchain_curr_idx)};
		auto present_res = p_queue.presentKHR(vk::PresentInfoKHR{
			tmp_sem_wait,
			swapchain,
			// _swapchain_curr_idx
			next_img_index
		});

	}
	// next image (everything)
	_swapchain_curr_idx = (_swapchain_curr_idx + 1) % _swapchain_images.size();
}

bool VulkanRenderer::createDevice(Engine& engine) {
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

bool VulkanRenderer::createSwapchain(Engine& engine) {
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

	{
		_swapchain_images.clear();
		auto images = device.getSwapchainImagesKHR(_swapchain);
		for (const auto& img : images) {
			_swapchain_images.push_back(img);
		}
	}
	SPDLOG_INFO("have {} swapchain images", _swapchain_images.size());

	_swapchain_image_views.clear();
	for (const auto& img : _swapchain_images) {
		_swapchain_image_views.push_back(device.createImageView({
			{},
			img,
			vk::ImageViewType::e2D,
			swap_surf_format.format,
			{}, // comp mapping
			{ // subres
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1,
			},
		}));
	}


	// TODO: move

	// TODO: refactor, provide "primitive" wrapper like opengl
	auto create_shader_module = [&](const uint8_t* data, const size_t size) -> vk::ShaderModule {
		return device.createShaderModule(vk::ShaderModuleCreateInfo{
			{},
			size,
			reinterpret_cast<const uint32_t*>(data)
		});
	};

	using namespace entt::literals;

	auto vert_module = create_shader_module(tut1_vert_spv, tut1_vert_spv_len);
	_r_shader_module["tut1_vert"_hs] = vert_module;
	auto frag_module = create_shader_module(tut1_frag_spv, tut1_frag_spv_len);
	_r_shader_module["tut1_frag"_hs] = frag_module;

	std::vector<vk::PipelineShaderStageCreateInfo> pl_shader_ci {
		{
			{},
			vk::ShaderStageFlagBits::eVertex,
			vert_module,
			"main",
		},
		{
			{},
			vk::ShaderStageFlagBits::eFragment,
			frag_module,
			"main",
		}
	};

	std::vector<vk::PipelineVertexInputStateCreateInfo> pl_vertex_input_ci {
		// hardcoded in shader, so no actual data here
		{},
		{}, // do i need two?
	};

	vk::PipelineInputAssemblyStateCreateInfo pl_input_asm_ci {
		{},
		vk::PrimitiveTopology::eTriangleList,
		VK_FALSE
	};

	device.createGraphicsPipeline({}, {
		{},
		static_cast<uint32_t>(pl_shader_ci.size()),
		pl_shader_ci.data(),
		pl_vertex_input_ci.data(),
		&pl_input_asm_ci,
	});

	_swapchain_framebuffers.clear();
	for (const auto& img_view : _swapchain_image_views) {
		vk::ImageView tmp_img_view = img_view;
		_swapchain_framebuffers.push_back(device.createFramebuffer({
			{},
			{}, // rend
			1,
			&tmp_img_view,
			surface_extent.width,
			surface_extent.height,
			1
		}));
	}

	// TODO: max simultanious frames
	_swapchain_sem_image_available.clear();
	_swapchain_sem_render_finished.clear();
	_swapchain_fence_in_flight.clear();
	for (size_t i = 0; i < _swapchain_images.size(); i++) {
		_swapchain_sem_image_available.push_back(device.createSemaphore(vk::SemaphoreCreateInfo{}));
		_swapchain_sem_render_finished.push_back(device.createSemaphore(vk::SemaphoreCreateInfo{}));
		_swapchain_fence_in_flight.push_back(device.createFence({vk::FenceCreateFlagBits::eSignaled}));
	}

	return true;
}

} // MM::Services


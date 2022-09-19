#pragma once

#include "entt/core/hashed_string.hpp"
#include <mm/engine_fwd.hpp>
#include <mm/services/service.hpp>
#include <unordered_map>

// fwd vk stuff

#define MM_VK_DEFINE_HANDLE(object) typedef struct object##_T* object;
// TODO: determain what we use
//#ifndef VK_DEFINE_NON_DISPATCHABLE_HANDLE
	//#if (VK_USE_64_BIT_PTR_DEFINES==1)
		#define MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
	//#else
		//#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
	//#endif
//#endif

MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkBuffer)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImage)
MM_VK_DEFINE_HANDLE(VkInstance)
MM_VK_DEFINE_HANDLE(VkPhysicalDevice)
MM_VK_DEFINE_HANDLE(VkDevice)
MM_VK_DEFINE_HANDLE(VkQueue)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSemaphore)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkFence)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImageView)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkFramebuffer)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkShaderModule)

// extensions
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkDebugUtilsMessengerEXT)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSurfaceKHR)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkSwapchainKHR)

namespace MM::Services {

class VulkanRenderer : public Service {
	private:
		// lets use the c-api types
		VkInstance _instance{};
		VkDebugUtilsMessengerEXT _debug_messenger{};

		VkSurfaceKHR _surface{};

		VkPhysicalDevice _physical_device{};
		VkDevice _device{};

		VkQueue _graphics_queue{};
		//VkQueue _present_queue{};

		VkSwapchainKHR _swapchain{};
		uint32_t _swapchain_curr_idx{};
		std::vector<VkImage> _swapchain_images{};
		std::vector<VkImageView> _swapchain_image_views{};
		std::vector<VkFramebuffer> _swapchain_framebuffers{};

		std::vector<VkSemaphore> _swapchain_sem_image_available{};
		std::vector<VkSemaphore> _swapchain_sem_render_finished{};
		std::vector<VkFence> _swapchain_fence_in_flight{};

		// resource cache
		std::unordered_map<entt::hashed_string::hash_type, VkShaderModule> _r_shader_module{};

	public:
		VulkanRenderer(void);
		~VulkanRenderer(void);

	private: // Service interface
		bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
		void disable(Engine& engine) override;

		const char* name(void) override { return "VulkanRenderer"; }

		void render(Engine& engine);

	public:
		bool createDevice(Engine& engine);

		bool createSwapchain(Engine& engine);

		//VkShaderModule getR_shader_module(const entt::hashed_string::value_type key) {
			//return _r_shader_module.at(key);
		//}

		//VkShaderModule getR_shader_module(const char* key) {
			//return getR_shader_module(entt::hashed_string::value(key));
		//}
};

} // MM::Services


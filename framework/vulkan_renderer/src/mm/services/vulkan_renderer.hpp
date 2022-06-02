#pragma once

#include <mm/engine_fwd.hpp>
#include <mm/services/service.hpp>

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
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkImageView)
MM_VK_DEFINE_NON_DISPATCHABLE_HANDLE(VkFramebuffer)

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
		std::vector<VkImage> _swapchain_images{};
		std::vector<VkImageView> _swapchain_image_views{};
		std::vector<VkFramebuffer> _swapchain_framebuffers{};

	public:
		VulkanRenderer(void);
		~VulkanRenderer(void);

		bool enable(Engine& engine, std::vector<UpdateStrategies::TaskInfo>& task_array) override;
		void disable(Engine&) override;

		const char* name(void) override { return "VulkanRenderer"; }

	public:
		bool createDevice(Engine& engine);

		bool createSwapchain(Engine& engine);
};

} // MM::Services


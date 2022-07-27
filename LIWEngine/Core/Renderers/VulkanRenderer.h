#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <vulkan/vulkan.h>
#include "LIWglfw.h"
//#include <mutex>

#include "LIWRenderer.h"
#include "Maths/LIWMaths.h"
#include "Application/Window.h"
#include "LIWExceptions.h"

#include "Utility/LIWFile.h"

namespace LIW {
	class VulkanRenderer:
		public LIWRenderer
	{
		//
		// Configurations
		//
	private:
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true; 
#endif
		const std::vector<const char*> validationLayers = {
			//"VK_LAYER_LUNARG_standard_validation"
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		static const int DEBUG_VERBOSE = 0;
		
		const int MAX_FRAMES_IN_FLIGHT = 2;

		//
		// Internal Structs
		//
	private:
		struct QueueFamilyIndices {
			int GraphicsFamilyIndex = -1;
			int PresentFamilyIndex = -1;
			int TransferFamilyIndex = -1;
		};

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities{};
			std::vector<VkSurfaceFormatKHR> formats{};
			std::vector<VkPresentModeKHR> presentModes{};
		};
		
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;

			// 2 structs needed for vertex data format 
			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}
			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

				// Pos
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				// Color
				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				// Texcoord
				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

				return attributeDescriptions;
			}
		};
		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

			{ {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};
		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		struct UniformBufferObject {
			glm::vec2 foo;
			alignas(16) glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

	public:
		friend class App::Window;
		VulkanRenderer(App::Window& window);
		virtual ~VulkanRenderer();

		void WaitUntilIdle();

	protected:
		//virtual void RenderScene() = 0;
		//virtual void Resize(int x, int y) = 0;
		inline virtual void RenderScene() { drawFrame(); } //TODO: Tmp
		virtual void Resize(int x, int y);

	private:
		// Create vulkan instance
		void createInstance(const char* pApplicationName, const char* pEngineName);
		
		// Vulkan surface
		void createSurface();

		// Physical Device (Graphics Card)
		void pickPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void printPhysicalDeviceQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		void printPhysicalDeviceExtensionSupport(VkPhysicalDevice device);
		void printPhysicalDeviceProperties(VkPhysicalDevice device);

		// Logical Device
		void createLogicalDevice();

		// Swapchain
		void createSwapChain();
		void recreateSwapChain();
		void cleanupSwapChain();
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createSwapChainImageViews();
		// Swapchain Frame Buffer
		void createFramebuffers();

		// Depth buffer
		void createDepthResources();
		VkFormat findDepthFormat();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		bool hasStencilComponent(VkFormat format);

		// Command Buffer
		void createCommandPool();
		void createCommandBuffers();
		// Single time command buffer
		VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandPool);
		void endSingleTimeCommands(VkCommandBuffer& commandBuffer, VkCommandPool& commandPool, VkQueue& submitQueue);

		// Descriptor
		// Descriptor Pool
		void createDescriptorPool();
		// Allocate Descriptors from pool
		void createDescriptorSets();

		// Sync
		void createSyncObjects();

		////
		// 
		// Customizable (Should be done in derived classes)
		//
		////
		// Simple render pass (attachments, subpasses...)
		void createRenderPass();
		// Create Resource Descriptor
		void createDescriptorSetLayout();
		// Simple pipeline
		void createGraphicsPipeline();

		// Add command to CommandBuffer
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		// Change uniform buffer
		void updateUniformBuffer(uint32_t currentImage);
		// Draw!!
		void drawFrame();

		// Cleanup
		void cleanup();

	public:
		////
		//
		// Resource Ops
		//
		////
		// 
		// Texture 
		//
		// Image
		void createTextureImage();
		void createImage2DExclusive(uint32_t width, uint32_t height, 
			VkFormat format, uint32_t mipLevels, VkImageUsageFlags usage, 
			VkImage& image);
		void createImage2DShared(uint32_t width, uint32_t height,
			VkFormat format, uint32_t mipLevels, VkImageUsageFlags usage, 
			const uint32_t* queueFamilies, uint32_t queueFamilyCount,
			VkImage& image);
		void createMemoryForTexture(VkImage& image, VkMemoryPropertyFlags properties, VkDeviceMemory& imageMemory);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);		
		// Image View
		void createTextureImageView();
		void create2DImageView(VkImage image, VkFormat format, const VkImageSubresourceRange& range, 
							VkImageView& imageView);
		void create2DImageView_Texture2D(VkImage image, VkFormat format, 
							VkImageView& imageView);
		void create2DImageView_RenderTarget(VkImage image, VkFormat format, 
							VkImageView& imageView);
		void create2DImageView_DepthBuffer(VkImage image, VkFormat format,
			VkImageView& imageView);
		// Sampler
		void createTextureSampler();
		//
		// Shader
		//
		VkShaderModule createShaderModule(const std::vector<char>& code);
		//
		// Geometry
		//
		// buffer
		void createBufferExclusive(VkDeviceSize size,
			VkBufferUsageFlags usage, 
			VkBuffer& buffer);
		void createBufferShared(VkDeviceSize size,
			VkBufferUsageFlags usage, 
			const uint32_t* queueFamilies, uint32_t queueFamilyCount,
			VkBuffer& buffer);
		void createMemoryForBuffer(VkBuffer& buffer, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		// vertex buffer
		void createVertexBuffer();
		// index buffer
		void createIndexBuffer();
		// uniform buffer
		void createUniformBuffers();

	private:
		bool checkValidationLayerSupport();
		void setupDebugCallback();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		std::vector<const char*> getRequiredExtensions();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	protected:
		// Instance
		VkInstance m_Instance;
		// Device (Physical and Logical)
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		// Surface
		VkSurfaceKHR m_Surface;

		// Swapchain
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		// Swapchain Framebuffer
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		// Resize
		bool m_FramebufferResized = false;

		// Queues
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_TransferQueue;
		
		// Command pools
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		VkCommandPool m_CommandPoolTransfer;

		// Descriptor
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// Sync 
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		uint32_t m_CurrentFrame = 0;


		// Debug
		VkDebugUtilsMessengerEXT debugMessenger;


		////
		// 
		// Customizable (Should be given derived classes)
		//
		////
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkRenderPass m_RenderPass;
		VkPipeline m_GraphicsPipeline;

		//Depth buffer
		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;

		VkImage m_TextureImage;
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
		VkDeviceMemory m_TextureImageMemory;
	};
}

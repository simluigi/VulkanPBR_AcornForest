/*======================================================================
Vulkan Presentation : VulkanFramework.h
Author:			Sim Luigi
Last Modified:	2020.12.13
=======================================================================*/
#pragma once

#define GLFW_INCLUDE_VULKAN    // VulkanSDK��GLFW�ƈꏏ�ɃC���N���[�h���܂��B
#include <GLFW/glfw3.h>        // replaces #include <vulkan/vulkan.h> and automatically bundles it with glfw include

#include <glm/glm.hpp>                      // glm�C���N���[�h
#include <glm/gtc/matrix_transform.hpp>     // ���f���g�����X�t�H�[��
#include <glm/gtx/hash.hpp>

#include <array>
#include <optional>
#include <iostream>  // std::cerr, try to migrate out of debug callback

struct Vertex
{
	// glm���C�u�����[���V�F�[�_�[�R�[�h�ɍ����Ă�C++�f�[�^�^��p�ӂ��Ă���܂��B
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	// �V�F�[�_�[���GPU�ɓǂݍ��܂ꂽ��A���_�V�F�[�_�[�ɓn���֐�2��
	// Two functions to tell Vulkan how to pass the shader data once it's been uploaded to the GPU

	// �@ ���_�o�C���f�B���O�F���̓ǂݍ��ޗ��@�istride, ���_���ƁE�C���X�^���X���Ɓj
	// Vertex Binding: Which rate to load data from memory throughout the vertices (stride, by vertex or by instance)
	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;                                // �z��1�����i�C���f�b�N�X 0�j
		bindingDescription.stride = sizeof(Vertex);                    // stride: ���̗v�f�܂ł̃o�C�g��  number of bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // RATE_INSTANCE�i�C���X�^���X�����_�����O�j

		return bindingDescription;
	}

	// �A�A�g���r���[�g�f�X�N���v�^�[�F���_�o�C���f�B���O����ǂݍ��񂾒��_�f�[�^�̈���
	// Attribute Descriptor: how to handle vertex input
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		// attributeDescriptions[0]: �ʒu���@Position
		// bindingDescription�Ɠ����l: ���_�V�F�[�_�[ (location = 0) in	
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;    // 3D�W�I���g���[	
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// attributeDescriptions[1]: �J���[���@Color�@�i��L�Ƃقړ����j
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	// �I�y���[�^�I�[�o�[���C�h: == �A���_��r�p�i�d���j
	// Operator Override for vertex comparison: equals == 
	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

// ���_�d�������n�b�V���֐��i��ł����ƕ׋����邱�Ɓj
// Hash function for filtering duplicate vertices (study this later!)
namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator() (Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos)
				^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1)
				^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

// UBO (UniformBufferObject): �}�g���N�X�ϊ����EMVP Transform
struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


// Vulkan��̂����鏈���̓L���[�ŏ�������Ă��܂��B�����ɂ���ăL���[�̎�ނ��قȂ�܂��B
struct QueueFamilyIndices
{
	// optional: ��񂪑��݂��Ă��邩���Ă��Ȃ����̃R���e�i�i�ǂ���ł��\�j
	// optional contained value; value MAY or MAY NOT be present. (C++17)

	std::optional<uint32_t> graphicsFamily;   // �O���t�B�b�N�n�L���[
	std::optional<uint32_t> presentFamily;    // �v���[���g�i�`��j�L���[

	// �L���[�̊e�l�������Ƒ��݂��Ă��邩�@check if value exists for all values
	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

// SwapChain�ڍ�
// Swap Chain Details
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


// Main class
// ���C���N���X
class CVulkanFramework
{

public:

	// 000 �f�o�b�O�R�[���o�b�N
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,     // �G���[�̏d�v���G��r�I�y���[�^�Ŕ�ׂ��܂��B
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void populateDebugMessengerCreateInfo
	    (VkDebugUtilsMessengerCreateInfoEXT& createInfo);    // 001 �f�o�b�O���b�Z���W���[�p���b�Z�[�W����
	void setupDebugMessenger();                              // 002 �f�o�b�O���b�Z���W���[�ݒ�
	bool checkValidationLayerSupport();                      // 003 �o���f�[�V�������C���[�Ή��m�F
	std::vector<const char*> getRequiredExtensions();    	 // 004 �o���f�[�V�������C���[�G�N�X�e���V�������l��


	

	void run();         
	void mainLoop();    

	void initWindow();                   // 101 �E�C���h�E������
	void initVulkan();                   // 102 Vulkan������
	void createInstance();               // 103 Vulkan�C���X�^���X������
	void createSurface();                // 104 GLFW�T�[�t�F�X����
	void pickPhysicalDevice();           // 105 Vulkan�Ή�GPU��I��
	void createLogicalDevice();          // 106 ���W�J���f�o�C�X�����i�f�o�C�X���o����j
	void createSwapChain();              // 107 �X���b�v�`�F�C������
	void createImageViews();             // 108 �C���[�W�r���[����
	void createRenderPass();             // �����_�[�p�X
	void createDescriptorSetLayout();    // ���\�[�X�ŃX�N���v�^�[���C�A�E�g 
	void createGraphicsPipeline();       // �O���t�B�b�N�X�p�C�v���C������
	void createColorResources();         // �J���[���\�[�X�����iMSAA)
	void createDepthResources();         // �f�v�X���\�[�X����
	void createFramebuffers();           // �t���[���o�b�t�@�����i�f�v�X���\�[�X�̌�j
	void createCommandPool();            // �R�}���h�o�b�t�@�[���i�[����v�[���𐶐�
	void createTextureImage();           // �e�N�X�`���[�}�b�s���O�p�摜����
	void createTextureImageView();       // �e�N�X�`���[���A�N�Z�X���邽�߂̃C���[�W�r���[����
	void createTextureSampler();         // �e�N�X�`���[�T���v���[����
	void loadModel();                    // ���f���f�[�^��ǂݍ���
	void createVertexBuffer();           // ���_�o�b�t�@�[����
	void createIndexBuffer();		     // �C���f�b�N�X�o�b�t�@�[����
	void createUniformBuffers();         // ���j�t�H�[���o�b�t�@�[����
	void createDescriptorPool();         // �f�X�N���v�^�[�Z�b�g���i�[����v�[���𐶐�
	void createDescriptorSets();         // �f�X�N���v�^�[�Z�b�g�𐶐�
	void createCommandBuffers();         // �R�}���h�o�b�t�@�[����
	void createSyncObjects();            // ���������I�u�W�F�N�g����
	

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	static std::vector<char> readFile(const std::string& fileName);
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	//----------------

	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUseableSampleCount();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);
	VkFormat findDepthFormat();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	bool hasStencilComponent(VkFormat format);



	static void framebufferResizeCallback
	    (GLFWwindow* window, int width, int height);
	void recreateSwapChain();
	void updateUniformBuffer(uint32_t currentImage);
	void drawFrame();
	
	void cleanup();
	void cleanupSwapChain();





private:

	GLFWwindow*                     m_Window;                // WINDOWS�ł͂Ȃ�GLFW;�@�N���X�v���b�g�t�H�[���Ή�
	VkInstance                      m_Instance;              // �C���X�^���X�F�A�v���P�[�V������SDK�̂Ȃ���

	VkDebugUtilsMessengerEXT        m_DebugMessenger;        // �f�o�b�O�R�[���o�b�N

	VkSurfaceKHR                    m_Surface;               // GLFW -> WSI (Windows System Integration) -> �E�B���h�E����

	VkPhysicalDevice    m_PhysicalDevice = VK_NULL_HANDLE;   // �����f�o�C�X�iGPU�E�O���t�B�b�N�X�J�[�h�j
	VkDevice            m_LogicalDevice;                     // �����f�o�C�X�Ƃ̃V�X�e���C���^�[�t�F�[�X
	VkQueue                         m_GraphicsQueue;         // �O���t�B�b�N�X��p�L���[
	VkQueue                         m_PresentQueue;          // �v���[���g�i�`��j��p�L���[

	VkSwapchainKHR                  m_SwapChain;             // �\������\��̉摜�̃L���[
	std::vector<VkImage>            m_SwapChainImages;	     // �L���[�摜
	VkFormat                        m_SwapChainImageFormat;  // �摜�t�H�[�}�b�g
	VkExtent2D                      m_SwapChainExtent;       // extent : �摜���]���[�V�����i�ʏ�A�E�B���h�E�Ɠ����j

	std::vector<VkImageView>   m_SwapChainImageViews;        // VkImage�̃n���h���G�摜���g�p����ۂɃA�N�Z�X����i�r���[���̂��́j
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;      // SwapChain�̃t���[���o�b�t�@

	VkRenderPass                    m_RenderPass;            // �����_�[�p�X
	VkDescriptorSetLayout           m_DescriptorSetLayout;   // �ŃX�N���v�^�[�Z�b�g���C�A�E�g
	VkPipelineLayout                m_PipelineLayout;        // �O���t�B�b�N�X�p�C�v���C�����C�A�E�g
	VkPipeline                      m_GraphicsPipeline;      // �O���t�B�b�N�X�p�C�v���C������

	VkCommandPool                   m_CommandPool;           // CommandPool : �R�}���h�o�b�t�@�[�A�����Ă��̊��蓖�Ă��������Ǘ��A
	std::vector<VkCommandBuffer>    m_CommandBuffers;

	VkDescriptorPool                m_DescriptorPool;        // DescriptorPool : �f�X�N���v�^�[�Z�b�g�A�����Ă��̊��蓖�Ă��������Ǘ�
	std::vector<VkDescriptorSet>    m_DescriptorSets;

	std::vector<Vertex>             m_Vertices;              // ���_�f�[�^�i���f���p�j
	std::vector<uint32_t>           m_Indices;               // �C���f�b�N�X�f�[�^�i���f���p�j

	VkBuffer                        m_VertexBuffer;          // ���_�o�b�t�@�[
	VkDeviceMemory                  m_VertexBufferMemory;    // ���_�o�b�t�@�[�������[���蓖��
	VkBuffer                        m_IndexBuffer;           // �C���f�b�N�X�o�b�t�@�[
	VkDeviceMemory                  m_IndexBufferMemory;     // �C���f�b�N�X�o�b�t�@�[�������[���蓖��

	std::vector<VkBuffer>           m_UniformBuffers;
	std::vector<VkDeviceMemory>     m_UniformBuffersMemory;

	VkImage                         m_DepthImage;            // Z�\�[�g�Ȃǂ̃f�v�X�o�b�t�@�����O�p�@Depth Buffering
	VkDeviceMemory                  m_DepthImageMemory;
	VkImageView                     m_DepthImageView;

	uint32_t                        m_MipLevels;             // �~�b�v�}�b�v�p
	VkImage                         m_TextureImage;          // �e�N�X�`���[�}�b�s���O�p�iTexel���A�Ȃǁj
	VkDeviceMemory                  m_TextureImageMemory;
	VkImageView                     m_TextureImageView;
	VkSampler                       m_TextureSampler;

	VkSampleCountFlagBits           m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;    // �}���`�T���v�����O�r�b�g��  Multisampling bit count 
	VkImage                         m_ColorImage;                             // �}���`�T���v�����O�o�b�t�@�[�p
	VkDeviceMemory                  m_ColorImageMemory;                       // �}���`�T���v�����O�o�b�t�@�[�p
	VkImageView                     m_ColorImageView;                         // �}���`�T���v�����O�o�b�t�@�[�p

	// Semaphore�F�ȒP�Ɂu�V�O�i���v�B�����𓯊����邽�߂ɗ��p���܂��B
	// Fence: GPU-CPU�̊Ԃ̓����@�\�G�Q�[�g�������ȃX�g�b�p�[�ł���B
	std::vector<VkSemaphore>        m_ImageAvailableSemaphores;    // �C���[�W�`�揀�������Z�}�t�H
	std::vector<VkSemaphore>        m_RenderFinishedSemaphores;    // �����_�����O�����Z�}�t�H
	std::vector<VkFence>            m_InFlightFences;              // �N�����̃t�F���X
	std::vector<VkFence>            m_ImagesInFlight;              // �������̉摜
	size_t                          m_CurrentFrame = 0;            // ���݂��t���[���J�E���^�[

	bool m_FramebufferResized = false;    // �E�E�B���h�E�T�C�Y���ύX������

};
// Begin: Win32 handrolled types
extern "C" {
	#if defined(_WIN64)
	typedef __int64 LONG_PTR; 
	#else
	typedef long LONG_PTR;
	#endif
	typedef LONG_PTR LRESULT;
	#define CALLBACK __stdcall
	typedef void *PVOID;
	typedef PVOID HANDLE;
	typedef HANDLE HWND;
	typedef unsigned int UINT;
	#if defined(_WIN64)
	typedef unsigned __int64 UINT_PTR;
	#else
	typedef unsigned int UINT_PTR;
	#endif
	typedef UINT_PTR WPARAM;
	#if defined(_WIN64)
	typedef __int64 LONG_PTR; 
	#else
	typedef long LONG_PTR;
	#endif
	typedef LONG_PTR LPARAM;
	#define WM_DESTROY 0x0002

	// Calling convention for WinMain
	#define WINAPI __stdcall

	// Additional handle types
	typedef HANDLE HINSTANCE;
	typedef HANDLE HICON;
	typedef HANDLE HCURSOR;
	typedef HANDLE HBRUSH;
	typedef HANDLE HMENU;
	typedef HANDLE HMODULE;

	// Pointer types
	typedef char* LPSTR;
	typedef const char* LPCSTR;
	typedef const wchar_t* LPCWSTR;
	typedef void* LPVOID;
	typedef void* HMONITOR;

	// Integer types
	typedef long LONG;
	typedef unsigned long DWORD;
	typedef int BOOL;
	typedef unsigned short ATOM;

	// Function pointer type for window procedure
	typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

	// Structure for POINT (used in MSG)
	typedef struct {
		LONG x;
		LONG y;
	} POINT;

	// Structure for MSG
	typedef struct {
		HWND hwnd;
		UINT message;
		WPARAM wParam;
		LPARAM lParam;
		DWORD time;
		POINT pt;
	} MSG;
	typedef const MSG* LPMSG;

	// Structure for WNDCLASSEX
	typedef struct {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HINSTANCE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCSTR lpszMenuName;
		LPCSTR lpszClassName;
		HICON hIconSm;
	} WNDCLASSEX;

	// Structure for SECURITY_ATTRIBUTES
	typedef struct {
		DWORD  nLength;
		LPVOID lpSecurityDescriptor;
		BOOL   bInheritHandle;
	} SECURITY_ATTRIBUTES;

	// Constants used in the code
	#define MB_OK           0x00000000L
	#define IDI_APPLICATION ((LPCSTR)32512)
	#define IDC_ARROW       ((LPCSTR)32512)
	#define COLOR_WINDOW    5
	#define WS_OVERLAPPEDWINDOW 0x00CF0000L
	#define SW_SHOW			5
	#define WM_QUIT         0x0012
	#define PM_REMOVE		0x0001
	#define WM_KEYDOWN      0x0100
	#define VK_ESCAPE		0x1B

	// Function prototypes
	LRESULT DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	void PostQuitMessage(int nExitCode);
	HICON LoadIconA(HINSTANCE hInstance, LPCSTR lpIconName);
	HCURSOR LoadCursorA(HINSTANCE hInstance, LPCSTR lpCursorName);
	ATOM RegisterClassExA(const WNDCLASSEX* lpwcx);
	HWND CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	DWORD GetLastError(void);
	BOOL ShowWindow(HWND hWnd, int nCmdShow);
	BOOL UpdateWindow(HWND hWnd);
	BOOL GetMessageA(MSG* lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	BOOL TranslateMessage(const MSG* lpMsg);
	LRESULT DispatchMessageA(const MSG* lpMsg);
	int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
	HMODULE GetModuleHandleA(LPCSTR lpModuleName);
	BOOL PeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
	void OutputDebugStringA(LPCSTR str);
}
// End: Win32 handrolled types

#include "stdio.h"
#include "assert.h"
#include "stdint.h"

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int32_t     b32;
typedef char16_t 	c16;
typedef wchar_t     w16;

typedef float       f32;
typedef double      f64;

typedef uintptr_t   uptr;
typedef ptrdiff_t   size;
typedef size_t      usize;

typedef char byte;

#define sizeof(x)	(u32)sizeof(x)
#define countof(x) (sizeof((x)) / sizeof(*(x)))


#define Stmnt(S) do{ S }while(0)

#define Min(a,b) (((a)<(b))?(a):(b))
#define Max(a,b) (((a)>(b))?(a):(b))
#define Clamp(a,x,b) (((x)<(a))?(a):((b)<(x))?(b):(x))
#define ClampTop(a,b) Min(a,b)
#define ClampBottom(a,b) Max(a,b)

// TODO: handroll this as well
// TODO: check out zeux/volk?
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define VK_CHECK(call) \
	do { \
		VkResult result_ = call; \
		assert(result_ == VK_SUCCESS); \
	} while(0)


static b32 running = 1;

static VkInstance instance;
static VkPhysicalDevice physicalDevice;
static VkDevice device;
static u32 queueFamilyIndex = 0; // SHORTCUT: this needs to be computed from queue properties


/**
 * SHADER BINARY
 */
// TODO: Do I wanna play with in which section I put it in?
asm(".align 64;"
    ".global vkSpvVert;"
    "vkSpvVert:;"
    ".incbin \"triangle.vert.spv\";"
    ".global vkSpvVertEnd;"
    "vkSpvVertEnd:;"
);

asm(".align 64;"
    ".global vkSpvFrag;"
    "vkSpvFrag:;"
    ".incbin \"triangle.frag.spv\";"
    ".global vkSpvFragEnd;"
    "vkSpvFragEnd:;"
);

extern "C" {
    extern u8 vkSpvVert[];
    extern u8 vkSpvVertEnd[];
    extern u8 vkSpvFrag[];
    extern u8 vkSpvFragEnd[];
}

VkSemaphore createSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkSemaphore semaphore = 0;
	VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, 0, &semaphore));
	
	return semaphore;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
				running = 0;
				return 0;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			running = 0;
			return 0;
	}
	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

VkBool32 debugReportCallback(
	VkDebugReportFlagsEXT flags, 
	VkDebugReportObjectTypeEXT objectType, 
	u64 object, usize location, 
	i32 messageCode, 
	const char* pLayerPrefix, 
	const char* pMessage, 
	void* pUserData)
{
	(void)objectType;
	(void)object;
	(void)location;
	(void)messageCode;
	(void)pLayerPrefix;
	(void)pUserData;

	const char* type = 
		(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) ? "ERROR" :
		(flags & (VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)) ? "WARNING" :
		"INFO";

	char message[4096];
	snprintf(message, countof(message), "%s: %s\n", type, pMessage);

	printf("%s", message);

	OutputDebugStringA(message);

	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		assert(!"Validation error encountered!");
	}
	return VK_FALSE;
}



// NOTE: Not a great idea, but we have 1 image for foreseeable(!) future so...
VkImageMemoryBarrier imageBarrier(
	VkImage image, 
	VkAccessFlags srcAccessMask, 
	VkAccessFlags dstAccessMask,
	VkImageLayout oldLayout,
	VkImageLayout newLayout)
{
	VkImageMemoryBarrier result = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	result.srcAccessMask = srcAccessMask;
	result.dstAccessMask = dstAccessMask;
	result.oldLayout = oldLayout;
	result.newLayout = newLayout;
	result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.image = image;
	result.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	result.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	result.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	return result;
}

// Entry point
// Window application
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main() {
	HINSTANCE hInstance = GetModuleHandleA(0);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(0, IDI_APPLICATION);
    wc.hCursor = LoadCursorA(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "MyClass";

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(0, "RegisterClassEx failed", "Error", MB_OK);
        return 1;
    }

	/**
	* INSTANCE CREATION (+INSTANCE EXTENSIONS)
	*/
	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	{
		VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
		// SHORTCUT: In prod, you should probably check if 1.1 is available
		appInfo.apiVersion = VK_API_VERSION_1_1; // ? cool to use with 1.4? 

		instanceInfo.pApplicationInfo = &appInfo;

		#ifdef _DEBUG
		const char* debugLayers[] = {
			"VK_LAYER_KHRONOS_validation"
		};

		instanceInfo.ppEnabledLayerNames = debugLayers;
		instanceInfo.enabledLayerCount = countof(debugLayers);
		#endif

		const char* instanceExtensions[] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		};

		instanceInfo.ppEnabledExtensionNames = instanceExtensions;
		instanceInfo.enabledExtensionCount = countof(instanceExtensions);
	}
	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &instance));

	/**
	 * REGISTER DEBUG CALLBACK
	 */
	VkDebugReportCallbackCreateInfoEXT callbackInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
	callbackInfo.flags = 
		VK_DEBUG_REPORT_WARNING_BIT_EXT | 
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | 
		VK_DEBUG_REPORT_ERROR_BIT_EXT;
	callbackInfo.pfnCallback = debugReportCallback;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = 
		(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	VkDebugReportCallbackEXT callback = 0;
	VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &callbackInfo, 0, &callback));

	/**
	* PICKING PHYSICAL DEVICE
	*/
	{
		VkPhysicalDevice physicalDevices[16];
		u32 physicalDeviceCount = countof(physicalDevices);
		VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices));

		for (u32 i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

			if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				printf("Picking discrete GPU %s\n", props.deviceName);
				physicalDevice = physicalDevices[i];
				goto physicalDeviceChosen;
			}
		}

		if (physicalDeviceCount > 0)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevices[0], &props);

			printf("Picking fallback GPU %s\n", props.deviceName);
			physicalDevice = physicalDevices[0];
			goto physicalDeviceChosen;
		}

		printf("No physical devices available!\n");
		return 0;

physicalDeviceChosen:
		assert(physicalDevice);
	}

	/**
	 * PICK QUEUE FAMILY (INDEX)
	 */
	{
		// NOTE: Typical thing in Vulkan where you call a function first without saving the result to just get the count
		u32 queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, 0);
		
		VkQueueFamilyProperties queueFamilyProperties[64];
		assert(queueFamilyPropertyCount < countof(queueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties);

		for (u32 i = 0; i < queueFamilyPropertyCount; i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueFamilyIndex = i;
				break;
			}
		}
		// TODO: Handle no graphics queue family!
	}

	/**
	* LOGICAL DEVICE CREATION
	*/
	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	{
		VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		// TODO: Validation error
		queueInfo.queueFamilyIndex = queueFamilyIndex; // SHORTCUT: this needs to be computed from queue properties
		queueInfo.queueCount = 1;
		f32 queuePriorities[] = { 1.0f };
		queueInfo.pQueuePriorities = queuePriorities;

		const char* deviceExtensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.pQueueCreateInfos = &queueInfo;
		deviceInfo.ppEnabledExtensionNames = deviceExtensions;
		deviceInfo.enabledExtensionCount = countof(deviceExtensions);

		//VkDevice device = 0;
	}
	VK_CHECK(vkCreateDevice(physicalDevice, &deviceInfo, 0, &device));

	/**
	* WIN32 WINDOW CREATION
	*/
	u32 windowWidth = 800; // TODO: Hardcoded for now, any win32 way of just getting?
	u32 windowHeight = 600;

    HWND hwnd = CreateWindowExA(
        0, "MyClass", "Ida",
        WS_OVERLAPPEDWINDOW,
        100, 100, windowWidth, windowHeight,
        0, 0, hInstance, 0
    );
    assert(hwnd);

	/**
	* SURFACE CREATION
	*/
	VkSurfaceKHR surface = 0;
	VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	{
		surfaceInfo.hinstance = hInstance;
		surfaceInfo.hwnd = hwnd;
	}
	VK_CHECK(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, 0, &surface));
	assert(surface);

	/**
	 * GETTING THE FORMAT
	 */
	u32 formatCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, 0));

	VkSurfaceFormatKHR formats[64] = {};
	assert(formatCount < countof(formats));
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats));

	assert(formatCount > 0); // TODO: This might need to handle either formatCount being 0 or first element reporting VK_FORMAT_UNDEFINED

	VkFormat swapchainFormat = (VkFormat)0;

	if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		swapchainFormat = VK_FORMAT_R8G8B8A8_UNORM;
		goto swapchainFormatPicked;
	}

	for (u32 i = 0; i < formatCount; i++)
	{
		if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM || formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
		{
			swapchainFormat = formats[i].format;
			goto swapchainFormatPicked;
		}
	}

	swapchainFormat = formats[0].format;
swapchainFormatPicked:

	VkSurfaceCapabilitiesKHR surfaceCaps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps));

	VkCompositeAlphaFlagBitsKHR surfaceComposite = 
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR :
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR :
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR :
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

	/**
	* SWAPCHAIN CREATION
	*/
	VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	{
		swapchainInfo.surface = surface;
		swapchainInfo.minImageCount = Max(2u, surfaceCaps.minImageCount);
		swapchainInfo.imageFormat = swapchainFormat;
		swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		// TODO: This fixed the validation error, but do check the spec
		swapchainInfo.imageExtent.width = surfaceCaps.currentExtent.width; // windowWidth;
		swapchainInfo.imageExtent.height = surfaceCaps.currentExtent.height; //windowHeight;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.queueFamilyIndexCount = 1;
		swapchainInfo.pQueueFamilyIndices = &queueFamilyIndex;
		swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainInfo.compositeAlpha = surfaceComposite;
		swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}
	VkSwapchainKHR swapchain = 0;
	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, 0, &swapchain));


	/**
	* ACQUIRE AND RELEASE SEMAPHORE CREATION
	*/
	VkSemaphore acquireSemaphore = 0;
	VkSemaphoreCreateInfo acquireSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VK_CHECK(vkCreateSemaphore(device, &acquireSemaphoreInfo, 0, &acquireSemaphore));
	assert(acquireSemaphore);

	VkSemaphore releaseSemaphore = 0;
	VkSemaphoreCreateInfo releaseSemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VK_CHECK(vkCreateSemaphore(device, &releaseSemaphoreInfo, 0, &releaseSemaphore));
	assert(releaseSemaphore);

	VkQueue queue = 0;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	/**
	 * VERTEX SHADER MODULE CREATION
	 */
	VkShaderModule triangleVS = 0;
	VkShaderModuleCreateInfo vertShaderInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	{
		usize vertShaderLength = vkSpvVertEnd - vkSpvVert;
		assert(vertShaderLength % 4 == 0);
		vertShaderInfo.codeSize = vertShaderLength;
		vertShaderInfo.pCode = (u32 *)vkSpvVert;
	}
	VK_CHECK(vkCreateShaderModule(device, &vertShaderInfo, 0, &triangleVS));
	assert(triangleVS);

	/**
	 * FRAGMENT SHADER MODULE CREATION
	 */
	VkShaderModule triangleFS = 0;

	VkShaderModuleCreateInfo fragShaderInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	{
		usize fragShaderLength = vkSpvFragEnd - vkSpvFrag;
		assert(fragShaderLength % 4 == 0);
		fragShaderInfo.codeSize = fragShaderLength;
		fragShaderInfo.pCode = (u32 *)vkSpvFrag;
	}
	VK_CHECK(vkCreateShaderModule(device, &fragShaderInfo, 0, &triangleFS));
	assert(triangleFS);

	/**
	* RENDER PASS
	*/
	VkRenderPass renderPass = 0;
	VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	{
		VkAttachmentReference colorAttachments = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachments;

		VkAttachmentDescription attachments[1] = {};
		attachments[0].format = swapchainFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassInfo.attachmentCount = countof(attachments);
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
	}
	VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, 0, &renderPass));

	/**
	 * PIPELINE CREATION
	 */
	// NOTE: A pipeline HAS TO contain all the stuff that you might ever need/use
	VkPipeline trianglePipeline = 0;

	VkPipelineCache pipelineCache = 0;

	VkPipelineLayout layout = 0;
	VkPipelineLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, 0, &layout));

	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	{
		VkPipelineShaderStageCreateInfo stages[2] = {};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = triangleVS;
		stages[0].pName = "main";
		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = triangleFS;
		stages[1].pName = "main";

		pipelineInfo.stageCount = countof(stages);
		pipelineInfo.pStages = stages;

		VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		// Zero for now, coz we have hardcoded data in shader
		pipelineInfo.pVertexInputState = &vertexInput;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInfo.pInputAssemblyState = &inputAssembly;

		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		pipelineInfo.pViewportState = &viewportState;
	
		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.lineWidth = 1.0f;
		pipelineInfo.pRasterizationState = &rasterizationState;

		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineInfo.pMultisampleState = &multisampleState;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		pipelineInfo.pDepthStencilState = &depthStencilState;

		VkPipelineColorBlendAttachmentState colorAttachmentState = {};
		colorAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorAttachmentState;
		pipelineInfo.pColorBlendState = &colorBlendState;

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = countof(dynamicStates);
		dynamicState.pDynamicStates = dynamicStates;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = renderPass;
	}
	VK_CHECK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, 0, &trianglePipeline));
	assert(trianglePipeline);

	/**
	 * FRAMEBUFFERS AND IMAGE VIEWS CREATION
	 */
	u32 swapchainImageCount = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, 0));

	VkImage swapchainImages[16]; // SHORTCUT: !!!
	assert(swapchainImageCount < countof(swapchainImages));
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages));

	VkImageView swapchainImageViews[16] = {};
	{
		for (u32 i = 0; i < swapchainImageCount; ++i)
		{
			/**
		 * IMAGE VIEW
		 */
			VkImageViewCreateInfo imageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageViewInfo.image = swapchainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = swapchainFormat;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(device, &imageViewInfo, 0, &swapchainImageViews[i]));
			assert(swapchainImageViews[i]);
		}
	}

	VkFramebuffer swapchainImageFramebuffers[16] = {};
	{
		for (u32 i = 0; i < swapchainImageCount; ++i)
		{
		/**
		* FRAMEBUFFER
		*/
			VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = &swapchainImageViews[i];
			framebufferInfo.width = surfaceCaps.currentExtent.width; // windowWidth
			framebufferInfo.height = surfaceCaps.currentExtent.height; // windowHeight
			framebufferInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, 0, &swapchainImageFramebuffers[i]));
			assert(swapchainImageFramebuffers[i]);
		}
	}


	VkCommandPool commandPool = 0;
	VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	{
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	}
	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, 0, &commandPool));

	VkCommandBuffer commandBuffer = 0;
	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	{
		allocateInfo.commandPool = commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
	}
	VK_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer));

	ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

	MSG msg = {};
    while (running) {
		if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		u32 imageIndex = 0;
		VK_CHECK(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &imageIndex));

		VK_CHECK(vkResetCommandPool(device, commandPool, 0));

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		// NOTE: Before we start a pass we are rendering to an image and ???
		VkImageMemoryBarrier renderBeginBarrier = imageBarrier(swapchainImages[imageIndex], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
							 VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderBeginBarrier);

		VkClearColorValue color = {{ 48.0f / 255.0f, 10.0f / 255.0f, 36.0f / 255.0f, 1.0f }};
		VkClearValue clearColor = { color };

		VkRenderPassBeginInfo passBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		passBeginInfo.renderPass = renderPass; // abstract configuration thingy
		passBeginInfo.framebuffer = swapchainImageFramebuffers[imageIndex]; // actual images
		passBeginInfo.renderArea.extent.width = surfaceCaps.currentExtent.width; // windowWidth;
		passBeginInfo.renderArea.extent.height = surfaceCaps.currentExtent.height; // windowHeight; // what are tilers?
		passBeginInfo.clearValueCount = 1;
		passBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = { 0, f32(windowHeight), f32(windowWidth), -f32(windowHeight), 0, 1 };
		VkRect2D scissor = { { 0, 0 }, { windowWidth, windowHeight } };

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

// NOTE: spirv-dis triangle.vert.spv

		/**
		 * NOTE: DRAW CALLS GO HERE
		 */
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);


		vkCmdEndRenderPass(commandBuffer); // NOTE: Can't have more than one renderpass in flight

		VkImageMemoryBarrier renderEndBarrier = imageBarrier(swapchainImages[imageIndex], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
							 VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderEndBarrier);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		VkPipelineStageFlags submitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &acquireSemaphore;
		submitInfo.pWaitDstStageMask = &submitStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &releaseSemaphore;
		
		VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &releaseSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		VK_CHECK(vkQueuePresentKHR(queue, &presentInfo));

		VK_CHECK(vkDeviceWaitIdle(device));
	}

	// TODO: destroy?

	vkDestroyInstance(instance, 0);

    return 0;
}
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
	typedef unsigned __int64 ULONGLONG;

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
	#define WM_SIZE			0x0005
	#define WM_QUIT         0x0012
	#define PM_REMOVE		0x0001
	#define WM_KEYDOWN      0x0100
	#define VK_ESCAPE		0x1B

	#define LOWORD(l) ((unsigned short)((l) & 0xFFFF))
	#define HIWORD(l) ((unsigned short)(((l) >> 16) & 0xFFFF))

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
	ULONGLONG GetTickCount64();
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

static u32 windowWidth;
static u32 windowHeight;
static b32 windowResized;

static VkInstance instance;
static VkPhysicalDevice physicalDevice;
static VkDevice device;
static u32 queueFamilyIndex = 0; // SHORTCUT: this needs to be computed from queue properties
static VkFormat swapchainFormat;


/**
 * SHADER BINARY
 */
// TODO: Do I wanna play with in which section I put it in?
asm(".align 64;"
    ".global vkSpvComp;"
    "vkSpvComp:;"
    ".incbin \"../build/triangle.comp.spv\";"
    ".global vkSpvCompEnd;"
    "vkSpvCompEnd:;"
);

extern "C" {
	extern u8 vkSpvComp[];
    extern u8 vkSpvCompEnd[];
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
		case WM_SIZE: {
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			windowWidth = width;
			windowHeight = height;
			windowResized = 1;
		} break;
		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
				running = 0;
				return 0;
			}
		} break;
		case WM_DESTROY: {
			PostQuitMessage(0);
			running = 0;
			return 0;
		} break;
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

// NOTE: win32 exclusive!
b32 supportsPresentation(VkPhysicalDevice physDevice, u32 familyIndex)
{
	b32 result = vkGetPhysicalDeviceWin32PresentationSupportKHR(physDevice, familyIndex);
	printf("PRESENTATION SUPPORTED: %s\n", result ? "YES" : "NO");

	return result;
}

u32 getQueueFamily(VkPhysicalDevice physDevice)
{
	u32 queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueCount, 0);
		
	VkQueueFamilyProperties queues[64];
	assert(queueCount < countof(queues));
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueCount, queues);

	for (u32 i = 0; i < queueCount; i++)
	{
		if (queues[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
		{
			return i;
		}
	}

	return VK_QUEUE_FAMILY_IGNORED;
}

#define SWAPCHAIN_IMAGES 16
typedef struct Swapchain
{
	VkSwapchainKHR swapchain;

	VkImage images[SWAPCHAIN_IMAGES];
	VkImageView imageViews[SWAPCHAIN_IMAGES];

	u8 imageCount;

	u16 width;
	u16 height;
} Swapchain;

void createSwapchain(
	Swapchain *result,  
	VkSurfaceKHR surface,
	u16 width, 
	u16 height,
	VkSwapchainKHR oldSwapchain)
{
	VkSurfaceCapabilitiesKHR surfaceCaps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps));

	VkCompositeAlphaFlagBitsKHR surfaceComposite = 
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) ? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR :
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR :
		(surfaceCaps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) ? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR :
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

	VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	{
		swapchainInfo.surface = surface;
		swapchainInfo.minImageCount = Max(2u, surfaceCaps.minImageCount);
		swapchainInfo.imageFormat = swapchainFormat;
		swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		swapchainInfo.imageExtent.width = width;
		swapchainInfo.imageExtent.height = height;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		swapchainInfo.queueFamilyIndexCount = 1;
		swapchainInfo.pQueueFamilyIndices = &queueFamilyIndex;
		swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainInfo.compositeAlpha = surfaceComposite;
		swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchainInfo.oldSwapchain = oldSwapchain;
	}
	VkSwapchainKHR swapchain = 0;
	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, 0, &swapchain));
	assert(swapchain);

	u32 imageCount = 0;
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, 0));

	VkImage images[16]; // SHORTCUT: !!!
	assert(imageCount < countof(images));
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images));

	VkImageView imageViews[16] = {};
	{
		for (u32 i = 0; i < imageCount; ++i)
		{
			/**
			* IMAGE VIEW
			*/
			VkImageViewCreateInfo imageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageViewInfo.image = images[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = swapchainFormat;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(device, &imageViewInfo, 0, &imageViews[i]));
			assert(imageViews[i]);
		}
	}

	result->swapchain = swapchain;

	result->imageCount = imageCount;
	result->width = width;
	result->height = height;
	for (u32 i = 0; i < imageCount; i++)
	{
		result->images[i] = images[i];
		result->imageViews[i] = imageViews[i];
	}
}

void destroySwapchain(Swapchain *swapchain)
{
	for (u32 i = 0; i < swapchain->imageCount; i++)
	{
		vkDestroyImageView(device, swapchain->imageViews[i], 0);
	}

	vkDestroySwapchainKHR(device, swapchain->swapchain, 0);
}

typedef struct ComputeResources {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
} ComputeResources;

void createComputeImage(ComputeResources *result, u32 width, u32 height) {
	VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = swapchainFormat;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(device, &imageInfo, 0, &result->image));
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, result->image, &memRequirements);
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	u32 memoryTypeIndex = 0;
	for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
			memoryTypeIndex = i;
			break;
		}
	}
	VkMemoryAllocateInfo memAllocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	memAllocInfo.allocationSize = memRequirements.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	VK_CHECK(vkAllocateMemory(device, &memAllocInfo, 0, &result->memory));
	VK_CHECK(vkBindImageMemory(device, result->image, result->memory, 0));
	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.image = result->image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = swapchainFormat;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.layerCount = 1;
	VK_CHECK(vkCreateImageView(device, &viewInfo, 0, &result->view));
}

void destroyComputeImage(ComputeResources *resources) {
	vkDestroyImageView(device, resources->view, 0);
	vkDestroyImage(device, resources->image, 0);
	vkFreeMemory(device, resources->memory, 0);
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

		// Thingy-majig to get sync validation
		VkValidationFeaturesEXT validationFeatures = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
		VkValidationFeatureEnableEXT enabledFeatures[] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
		validationFeatures.enabledValidationFeatureCount = 1;
		validationFeatures.pEnabledValidationFeatures = enabledFeatures;

		instanceInfo.pNext = &validationFeatures;
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

	VkDebugReportCallbackEXT debugCallback = 0;
	VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &callbackInfo, 0, &debugCallback));

	/**
	* PICKING PHYSICAL DEVICE
	*/
	{
		VkPhysicalDevice discrete = 0;
		VkPhysicalDevice fallback = 0;

		VkPhysicalDevice physicalDevices[16];
		u32 physicalDeviceCount = countof(physicalDevices);
		VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices));

		for (u32 i = 0; i < physicalDeviceCount; i++)
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

			printf("GPU %s\n", props.deviceName);

			u32 familyIndex = getQueueFamily(physicalDevices[i]);
			if (familyIndex == VK_QUEUE_FAMILY_IGNORED) continue;
			if (!supportsPresentation(physicalDevices[i], familyIndex)) continue;
			if (!discrete && props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				discrete = physicalDevices[i];
				queueFamilyIndex = familyIndex;
			}

			if (!fallback)
			{
				fallback = physicalDevices[i];
				if (!discrete) {
					queueFamilyIndex = familyIndex;
				}
			}
		}

		if (!fallback)
		{
			printf("ERROR: No GPUs found!\n");
		}

		VkPhysicalDeviceProperties pickedProps;

		if (discrete)
		{
			vkGetPhysicalDeviceProperties(discrete, &pickedProps);
			printf("Picking discrete GPU %s\n", pickedProps.deviceName);
			physicalDevice = discrete;
		} else {
			vkGetPhysicalDeviceProperties(fallback, &pickedProps);
			printf("Picking fallback GPU %s\n", pickedProps.deviceName);
			physicalDevice = fallback;
		}
	}
	assert(physicalDevice);

	/**
	* LOGICAL DEVICE CREATION
	*/
	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	{
		VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueInfo.queueFamilyIndex = queueFamilyIndex;
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
	}
	VK_CHECK(vkCreateDevice(physicalDevice, &deviceInfo, 0, &device));

	/**
	* WIN32 WINDOW CREATION
	*/
	windowWidth = 800;
	windowHeight = 600;

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

	swapchainFormat = (VkFormat)0;

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

	/**
	* SWAPCHAIN CREATION
	*/
	VkSurfaceCapabilitiesKHR surfaceCaps;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps));

	Swapchain swapchain;
	createSwapchain(&swapchain, surface, surfaceCaps.currentExtent.width, surfaceCaps.currentExtent.height, 0);

	/**
	 * COMPUTE RESOURCES CREATION
	 */
	ComputeResources computeResources = {};
	createComputeImage(&computeResources, windowWidth, windowHeight);

	/**
	* DESCRIPTOR SET FOR STORAGE IMAGE
	*/
	VkDescriptorSetLayout descriptorSetLayout = 0;
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		VkDescriptorSetLayoutCreateInfo storageLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		storageLayoutInfo.bindingCount = 1;
		storageLayoutInfo.pBindings = &layoutBinding;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &storageLayoutInfo, 0, &descriptorSetLayout));
	}

	VkDescriptorPool descriptorPool = 0;
	{
		VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 };
		VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, 0, &descriptorPool));
	}

	VkDescriptorSet descriptorSet = 0;
	{
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));
	}

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView = computeResources.view;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorWrite.pImageInfo = &imageInfo;
	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, 0);


	/**
	* SEMAPHORE CREATION
	*/
	VkSemaphore imageAvailableSemaphore = createSemaphore(device);
	VkSemaphore computeFinishedSemaphore = createSemaphore(device);
	VkSemaphore copyFinishedSemaphore = createSemaphore(device);

	VkQueue queue = 0;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	/**
	* COMPUTE SHADER MODULE CREATION
	*/
	VkShaderModule computeShader = 0;
	VkShaderModuleCreateInfo compShaderInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	{
		usize compShaderLength = vkSpvCompEnd - vkSpvComp;
		assert(compShaderLength % 4 == 0);
		compShaderInfo.codeSize = compShaderLength;
		compShaderInfo.pCode = (u32 *)vkSpvComp;
	}
	VK_CHECK(vkCreateShaderModule(device, &compShaderInfo, 0, &computeShader));
	assert(computeShader);

	/**
	* COMPUTE PIPELINE CREATION
	*/
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(float) + 2 * sizeof(uint32_t); // time (float), width (uint), height (uint)

	VkPipelineLayout computePipelineLayout = 0;
	VkPipelineLayoutCreateInfo computeLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	computeLayoutInfo.setLayoutCount = 1;
	computeLayoutInfo.pSetLayouts = &descriptorSetLayout;
	computeLayoutInfo.pushConstantRangeCount = 1;
	computeLayoutInfo.pPushConstantRanges = &pushConstantRange;
	VK_CHECK(vkCreatePipelineLayout(device, &computeLayoutInfo, 0, &computePipelineLayout));

	VkComputePipelineCreateInfo computePipelineInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	computePipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computePipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computePipelineInfo.stage.module = computeShader;
	computePipelineInfo.stage.pName = "main";
	computePipelineInfo.layout = computePipelineLayout;

	VkPipeline computePipeline = 0;
	VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineInfo, 0, &computePipeline));
	assert(computePipeline);

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

	f64 startTime = (f64)GetTickCount64() / 1000.0f;

	MSG msg = {};
    while (running) {
		f64 currentTime = (f64)GetTickCount64() / 1000.0f - startTime;

		if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);

			if (!running) break;
		}

		if (windowResized && windowWidth > 0 && windowHeight > 0)
		{
			VK_CHECK(vkDeviceWaitIdle(device));
			Swapchain oldSwapchain = swapchain;
			ComputeResources oldComputeResources = computeResources;
			createSwapchain(&swapchain, surface, windowWidth, windowHeight, oldSwapchain.swapchain);
			createComputeImage(&computeResources, windowWidth, windowHeight);
			imageInfo.imageView = computeResources.view;
			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, 0);
			destroySwapchain(&oldSwapchain);
			destroyComputeImage(&oldComputeResources);
			windowResized = 0;
		}

		u32 imageIndex = 0;
		VkResult result = vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			windowResized = 1;
			continue;
		}

		VK_CHECK(vkResetCommandPool(device, commandPool, 0));

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		// Transition compute image to VK_IMAGE_LAYOUT_GENERAL for compute shader writes
		VkImageMemoryBarrier computeImageBarrier = imageBarrier(computeResources.image, 0, VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, 0, 0, 0, 1, &computeImageBarrier);

		struct PushConstants {
			float time;
			uint32_t width;
			uint32_t height;
		} pushConstants;
		pushConstants.time = (f32)currentTime; // You’ll need to track time (e.g., seconds since start)
		pushConstants.width = windowWidth;
		pushConstants.height = windowHeight;
		vkCmdPushConstants(commandBuffer, computePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstants), &pushConstants);

		// Dispatch compute shader
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &descriptorSet, 0, 0);
		vkCmdDispatch(commandBuffer, (windowWidth + 15) / 16, (windowHeight + 15) / 16, 1);

		// Transition compute image to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL for copy
		computeImageBarrier = imageBarrier(computeResources.image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &computeImageBarrier);

		// Transition swapchain image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		VkImageMemoryBarrier swapchainImageBarrier = imageBarrier(swapchain.images[imageIndex], 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &swapchainImageBarrier);

		// Copy compute image to swapchain image
		VkImageCopy copyRegion = {};
		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.extent.width = windowWidth;
		copyRegion.extent.height = windowHeight;
		copyRegion.extent.depth = 1;
		vkCmdCopyImage(commandBuffer, computeResources.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchain.images[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);


		// Transition swapchain image to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		swapchainImageBarrier = imageBarrier(swapchain.images[imageIndex], VK_ACCESS_TRANSFER_WRITE_BIT, 0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &swapchainImageBarrier);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
		submitInfo.pWaitDstStageMask = &waitStage;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &copyFinishedSemaphore;
		VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &copyFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &(swapchain.swapchain);
		presentInfo.pImageIndices = &imageIndex;
		result = vkQueuePresentKHR(queue, &presentInfo);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			windowResized = 1;
		}

		VK_CHECK(vkDeviceWaitIdle(device));
	}

	// NOTE: Wait for everything pending on device to complete
	VK_CHECK(vkDeviceWaitIdle(device));

	vkDestroyCommandPool(device, commandPool, 0);

	destroySwapchain(&swapchain);

	vkDestroyPipeline(device, computePipeline, 0);
	vkDestroyPipelineLayout(device, computePipelineLayout, 0);
	vkDestroyShaderModule(device, computeShader, 0);

	vkDestroySemaphore(device, imageAvailableSemaphore, 0);
	vkDestroySemaphore(device, computeFinishedSemaphore, 0);
	vkDestroySemaphore(device, copyFinishedSemaphore, 0);

	vkDestroyDescriptorPool(device, descriptorPool, 0);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, 0);
	destroyComputeImage(&computeResources);
	
	vkDestroySurfaceKHR(instance, surface, 0);

	vkDestroyDevice(device, 0);

	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(instance, debugCallback, 0);

	vkDestroyInstance(instance, 0);

    return 0;
}
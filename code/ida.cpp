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



VkPhysicalDevice pickPhysicalDevice(VkPhysicalDevice *physicalDevices, u32 physicalDeviceCount)
{
	for (u32 i = 0; i < physicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

		if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			printf("Picking discrete GPU %s\n", props.deviceName);
			return physicalDevices[i];
		}
	}

	if (physicalDeviceCount > 0)
	{
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(physicalDevices[0], &props);

		printf("Picking fallback GPU %s\n", props.deviceName);
		return physicalDevices[0];
	}

	printf("No physical devices available!\n");
	return 0;
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
	* INSTANCE CREATION
	*/
	// SHORTCUT: In prod, you should probably check if 1.1 is available
	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.apiVersion = VK_API_VERSION_1_1; // ? cool to use with 1.4? 

	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
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
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	instanceInfo.ppEnabledExtensionNames = instanceExtensions;
	instanceInfo.enabledExtensionCount = countof(instanceExtensions);

	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &instance));


	/**
	* PICKING PHYSICAL DEVICE
	*/
	VkPhysicalDevice physicalDevices[16];
	u32 physicalDeviceCount = countof(physicalDevices);
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices));

	VkPhysicalDevice physicalDevice = pickPhysicalDevice(physicalDevices, physicalDeviceCount);
	assert(physicalDevice);

	/**
	* LOGICAL DEVICE CREATION
	*/
	u32 queueFamilyIndex = 0;
	VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	// TODO: Validation error
	queueInfo.queueFamilyIndex = queueFamilyIndex; // SHORTCUT: this needs to be computed from queue properties
	queueInfo.queueCount = 1;
	f32 queuePriorities[] = { 1.0f };
	queueInfo.pQueuePriorities = queuePriorities;

	const char* deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;
	deviceInfo.enabledExtensionCount = countof(deviceExtensions);

	VkDevice device = 0;
	VK_CHECK(vkCreateDevice(physicalDevice, &deviceInfo, 0, &device));


	/**
	* WIN32 WINDOW CREATION
	*/
	i16 windowWidth = 800; // TODO: Hardcoded for now, any win32 way of just getting?
	i16 windowHeight = 600;

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
	VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
	surfaceInfo.hinstance = hInstance;
	surfaceInfo.hwnd = hwnd;
	VkSurfaceKHR surface = 0;
	VK_CHECK(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, 0, &surface));
	assert(surface);

	/**
	* SWAPCHAIN CREATION
	*/
	VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	swapchainInfo.surface = surface;
	swapchainInfo.minImageCount = 2;
	swapchainInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM; // SHORTCUT: Some devices only support BGRA
	swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainInfo.imageExtent.width = windowWidth;
	swapchainInfo.imageExtent.height = windowHeight;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainInfo.queueFamilyIndexCount = 1;
	swapchainInfo.pQueueFamilyIndices = &queueFamilyIndex;
	swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

	VkSwapchainKHR swapchain = 0;
	VK_CHECK(vkCreateSwapchainKHR(device, &swapchainInfo, 0, &swapchain));


	/**
	* ACQUIRE AND RELEASE SEMAPHORE CREATION
	*/
	VkSemaphore acquireSemaphore = createSemaphore(device);
	assert(acquireSemaphore);

	VkSemaphore releaseSemaphore = createSemaphore(device);
	assert(releaseSemaphore);

	VkQueue queue = 0;
	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

	VkImage swapchainImages[16]; // SHORTCUT: !!!
	u32 swapchainImageCount = countof(swapchainImages);
	VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages));

	VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

	VkCommandPool commandPool = 0;
	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, 0, &commandPool));

	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer = 0;
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

		VkClearColorValue color = {{ 1, 0, 1, 1 }};
		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.levelCount = 1;
		range.layerCount = 1;

		vkCmdClearColorImage(commandBuffer, swapchainImages[imageIndex], VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);

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
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

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
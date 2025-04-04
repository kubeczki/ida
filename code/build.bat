@echo off

set VULKAN_SDK=C:\VulkanSDK\1.4.309.0
set INCLUDE=%VULKAN_SDK%\Include;%INCLUDE%
set LIB=%VULKAN_SDK%\Lib;%LIB%
set PATH=%VULKAN_SDK%\Bin;%PATH%

echo VULKAN_SDK=%VULKAN_SDK%
REM echo INCLUDE=%INCLUDE%
REM echo LIB=%LIB%
REM echo PATH=%PATH%


REM -Werror - treat all warnings as errors
REM -fsanitize=address
REM -D_DEBUG - is this Vulkan specific?
set CXXFLAGS=-g -std=c++17 -Wall -Wextra -pedantic -fno-exceptions -fno-rtti -D_DEBUG
REM -Wno-keyword-macro - keyword hidden by macro definition
REM -Wno-language-extension-token - used __int64
REM -Wmissing-field-initializers - struct intialization
set IGNOREDWARNINGS=-Wno-keyword-macro -Wno-language-extension-token -Wno-missing-field-initializers
REM -fuse-ld=lld - use clangs linker
set LINKERFLAGS=-luser32 -lgdi32 -lvulkan-1 -fuse-ld=lld

REM -Wl,/SUBSYSTEM:CONSOLE
clang++ %CXXFLAGS% %IGNOREDWARNINGS% -o ../build/ida.exe ida.cpp %LINKERFLAGS% -Wl,/SUBSYSTEM:CONSOLE
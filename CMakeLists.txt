# CMakeList.txt: JayRender 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 获取OS信息
if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


set(CMAKE_CXX_STANDARD 17)

project ("JayRender")

# 将源代码添加到此项目的可执行文件。

set(VK_SDK_PATH $ENV{VK_SDK_PATH})
if(NOT DEFINED ENV{VK_SDK_PATH})
    message(FATAL_ERROR "VK_SDK_PATH ENV not defined!")
endif()
if (NOT VK_SDK_PATH)
    message(FATAL_ERROR "VK_SDK_PATH environment variable is not set.")
endif()

# 预编译着色器

find_program(GLSLC_PROGRAM glslc REQUIRED)

execute_process(COMMAND ${GLSLC_PROGRAM} ${PROJECT_SOURCE_DIR}/Shaders/Base/vertex.vert -o ${PROJECT_SOURCE_DIR}/bin/asset/vertex.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${PROJECT_SOURCE_DIR}/Shaders/Base/frag.frag -o ${PROJECT_SOURCE_DIR}/bin/asset/frag.spv)


# ------------
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(VK_SDK_INCLUDE $ENV{VK_SDK_PATH}/Include)
set(VK_SDK_LIB $ENV{VK_SDK_PATH}/Lib/vulkan-1.lib)
set(GLFW_LIB $ENV{VK_SDK_PATH}/Lib/glfw3.lib)

file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR} "include/*.h")
file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR} "include/*.cpp")


file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR} "Source/*.cpp")
file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR} "Source/*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})




add_executable (JayRender 
"JayRender.h" "JayRender.cpp"  


"include/imgui-docking/imgui.cpp"
"include/imgui-docking/imgui_demo.cpp"
"include/imgui-docking/imgui_draw.cpp"
"include/imgui-docking/imgui_tables.cpp"
"include/imgui-docking/imgui_widgets.cpp"
"include/imgui-docking/backends/imgui_impl_glfw.cpp"
"include/imgui-docking/backends/imgui_impl_vulkan.cpp"

"Source/Tools/ImageLoader.hpp" "Source/Tools/ImageLoader.cpp"
"Source/RHI/VulkanHelper.hpp" "Source/RHI/VulkanHelper.cpp"
"Source/RHI/RHIDynamic.hpp" "Source/RHI/RHIDynamic.cpp"
"Source/RHI/RHIInterface.hpp" "Source/RHI/RHIInterface.cpp"



"Source/Tools/ModelLoader.hpp" "Source/Tools/ModelLoader.cpp" 
"Source/CommandManager.hpp" "Source/CommandManager.cpp"
"Source/DescriptorManager.hpp" "Source/DescriptorManager.cpp"
"Source/SwapChain.h" "Source/SwapChain.cpp"  
"Source/Tools.h" "Source/Tools.cpp"  
"Source/Shader.h" "Source/Shader.cpp" 
"Source/RenderProcess.h" "Source/RenderProcess.cpp" 
"Source/Renderer.h" "Source/Renderer.cpp" 
"Source/Buffer.h" "Source/Buffer.cpp" 
"Source/Uniform.h" "Source/Uniform.cpp" 
"Source/Texture.hpp" "Source/Texture.cpp"
"Source/UploadBuffer.hpp" "Source/UploadBuffer.cpp"
"Source/RHI/Framebuffers.hpp" "Source/RHI/Framebuffers.cpp"

"Source/3D/GameObject/GameObject.hpp" "Source/3D/GameObject/GameObject.cpp"
"Source/3D/RawMesh.hpp" "Source/3D/RawMesh.cpp" 
"Source/3D/Camera.hpp" "Source/3D/Camera.cpp"
"Source/3D/Scene.hpp" "Source/3D/Scene.cpp" 


"Source/Context.h" "Source/Context.cpp"

"Source/UiContext.hpp" "Source/UiContext.cpp"


"Source/AABB_Tree/AABB_Tree.cpp" "Source/AABB_Tree/AABB_Tree.h" "Source/AABB_Tree/TinyVector.h"
"Source/Math/Algorithms.h" "Source/Math/Massage.h" "Source/Math/MPoint3.h" "Source/Math/Mrgb.h" "Source/Math/MVector3.h"
"Source/PolyMesh/IOManager.h" "Source/PolyMesh/IOManger.cpp" "Source/PolyMesh/MemoryPool.h" "Source/PolyMesh/PolyMesh.cpp" "Source/PolyMesh/PolyMesh.h" 
"Source/PolyMesh/PolyMeshIterators.cpp" "Source/PolyMesh/PolyMeshIterators.h" "Source/PolyMesh/PolyMesh_Base.cpp" "Source/PolyMesh/PolyMesh_Base.h"
"Source/QEM.h" "Source/QEM.cpp"

"Source/Engine.hpp" "Source/Engine.cpp"

)




target_link_libraries(JayRender PUBLIC $ENV{VK_SDK_PATH}/Lib/glfw3.lib)
target_link_libraries(JayRender PUBLIC ${VK_SDK_LIB})



target_include_directories(JayRender PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(JayRender PUBLIC ${VK_SDK_INCLUDE})

include_directories(${VK_SDK_INCLUDE})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET JayRender PROPERTY CXX_STANDARD 17)
  message("????")
endif()

# TODO: 如有需要，请添加测试并安装目标。

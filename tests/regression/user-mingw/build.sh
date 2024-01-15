VK_SDK="include/VulkanSDK"
export VULKAN_SDK=$VK_SDK/Windows
export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
export VULKAN_INCLUDE_DIR="$VK_SDK/Windows/Include" 

if [[ -d build ]];
then 
    rm -rf build
fi

mkdir build && cd build
cmake -D CMAKE_TOOLCHAIN_FILE=../windows.cmake .. && make

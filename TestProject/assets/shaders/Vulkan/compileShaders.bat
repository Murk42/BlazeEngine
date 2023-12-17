:: %1 - projectDirectory

echo Compiling Shaders:
call "D:\VulkanSDK\1.3.261.1\Bin\glslc.exe" %1assets\shaders\Vulkan\line2d.vert -o %1assets\shaders\Vulkan\line2d.vert.spv
call "D:\VulkanSDK\1.3.261.1\Bin\glslc.exe" %1assets\shaders\Vulkan\line2d.frag -o %1assets\shaders\Vulkan\line2d.frag.spv
pause
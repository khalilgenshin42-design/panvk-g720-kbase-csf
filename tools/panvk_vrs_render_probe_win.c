/*
 * PanVK pipeline VRS GPU execution probe for Winlator.
 * Renders a full-screen coordinate pattern twice, at 1x1 and 2x2,
 * waits for GPU completion, reads the image back, and compares spatial
 * repetition. The 2x2 path should produce repeated 2x2 color blocks.
 *
 * SPDX-License-Identifier: MIT
 */
#define WIN32_LEAN_AND_MEAN
#define VK_NO_PROTOTYPES
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include "vrs_probe_spv.h"

#define W 64u
#define H 64u
#define IMAGE_BYTES (W * H * 4u)

#define LOAD_I(name) ((PFN_vk##name)gipa(instance, "vk" #name))
#define LOAD_D(name) ((PFN_vk##name)get_device_proc_addr(device, "vk" #name))

static FILE *logf;
static PFN_vkGetInstanceProcAddr gipa;

#define LOG(...) do { if (logf) { fprintf(logf, __VA_ARGS__); fflush(logf); } } while (0)

static uint32_t
memory_type(const VkPhysicalDeviceMemoryProperties *props, uint32_t bits,
            VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred)
{
   for (uint32_t i = 0; i < props->memoryTypeCount; i++) {
      VkMemoryPropertyFlags f = props->memoryTypes[i].propertyFlags;
      if ((bits & (1u << i)) && (f & required) == required &&
          (f & preferred) == preferred)
         return i;
   }
   for (uint32_t i = 0; i < props->memoryTypeCount; i++) {
      VkMemoryPropertyFlags f = props->memoryTypes[i].propertyFlags;
      if ((bits & (1u << i)) && (f & required) == required)
         return i;
   }
   return UINT32_MAX;
}

#define DECLARE_DEVICE_FUNCS \
   PFN_vkDestroyDevice destroy_device = LOAD_D(DestroyDevice); \
   PFN_vkGetDeviceQueue get_device_queue = LOAD_D(GetDeviceQueue); \
   PFN_vkCreateImage create_image = LOAD_D(CreateImage); \
   PFN_vkDestroyImage destroy_image = LOAD_D(DestroyImage); \
   PFN_vkGetImageMemoryRequirements get_image_memory_requirements = LOAD_D(GetImageMemoryRequirements); \
   PFN_vkAllocateMemory allocate_memory = LOAD_D(AllocateMemory); \
   PFN_vkFreeMemory free_memory = LOAD_D(FreeMemory); \
   PFN_vkBindImageMemory bind_image_memory = LOAD_D(BindImageMemory); \
   PFN_vkCreateImageView create_image_view = LOAD_D(CreateImageView); \
   PFN_vkDestroyImageView destroy_image_view = LOAD_D(DestroyImageView); \
   PFN_vkCreateBuffer create_buffer = LOAD_D(CreateBuffer); \
   PFN_vkDestroyBuffer destroy_buffer = LOAD_D(DestroyBuffer); \
   PFN_vkGetBufferMemoryRequirements get_buffer_memory_requirements = LOAD_D(GetBufferMemoryRequirements); \
   PFN_vkBindBufferMemory bind_buffer_memory = LOAD_D(BindBufferMemory); \
   PFN_vkMapMemory map_memory = LOAD_D(MapMemory); \
   PFN_vkUnmapMemory unmap_memory = LOAD_D(UnmapMemory); \
   PFN_vkCreateRenderPass create_render_pass = LOAD_D(CreateRenderPass); \
   PFN_vkDestroyRenderPass destroy_render_pass = LOAD_D(DestroyRenderPass); \
   PFN_vkCreateFramebuffer create_framebuffer = LOAD_D(CreateFramebuffer); \
   PFN_vkDestroyFramebuffer destroy_framebuffer = LOAD_D(DestroyFramebuffer); \
   PFN_vkCreateShaderModule create_shader_module = LOAD_D(CreateShaderModule); \
   PFN_vkDestroyShaderModule destroy_shader_module = LOAD_D(DestroyShaderModule); \
   PFN_vkCreatePipelineLayout create_pipeline_layout = LOAD_D(CreatePipelineLayout); \
   PFN_vkDestroyPipelineLayout destroy_pipeline_layout = LOAD_D(DestroyPipelineLayout); \
   PFN_vkCreateGraphicsPipelines create_graphics_pipelines = LOAD_D(CreateGraphicsPipelines); \
   PFN_vkDestroyPipeline destroy_pipeline = LOAD_D(DestroyPipeline); \
   PFN_vkCreateCommandPool create_command_pool = LOAD_D(CreateCommandPool); \
   PFN_vkDestroyCommandPool destroy_command_pool = LOAD_D(DestroyCommandPool); \
   PFN_vkAllocateCommandBuffers allocate_command_buffers = LOAD_D(AllocateCommandBuffers); \
   PFN_vkResetCommandBuffer reset_command_buffer = LOAD_D(ResetCommandBuffer); \
   PFN_vkBeginCommandBuffer begin_command_buffer = LOAD_D(BeginCommandBuffer); \
   PFN_vkEndCommandBuffer end_command_buffer = LOAD_D(EndCommandBuffer); \
   PFN_vkCmdBeginRenderPass cmd_begin_render_pass = LOAD_D(CmdBeginRenderPass); \
   PFN_vkCmdEndRenderPass cmd_end_render_pass = LOAD_D(CmdEndRenderPass); \
   PFN_vkCmdBindPipeline cmd_bind_pipeline = LOAD_D(CmdBindPipeline); \
   PFN_vkCmdDraw cmd_draw = LOAD_D(CmdDraw); \
   PFN_vkCmdCopyImageToBuffer cmd_copy_image_to_buffer = LOAD_D(CmdCopyImageToBuffer); \
   PFN_vkCmdPipelineBarrier cmd_pipeline_barrier = LOAD_D(CmdPipelineBarrier); \
   PFN_vkQueueSubmit queue_submit = LOAD_D(QueueSubmit); \
   PFN_vkCreateFence create_fence = LOAD_D(CreateFence); \
   PFN_vkDestroyFence destroy_fence = LOAD_D(DestroyFence); \
   PFN_vkWaitForFences wait_for_fences = LOAD_D(WaitForFences)

static int
render_rate(VkDevice device, VkPhysicalDeviceMemoryProperties *mem_props,
            PFN_vkGetDeviceProcAddr get_device_proc_addr,
            VkQueue queue, uint32_t queue_family, VkFormat format,
            VkRenderPass render_pass, VkPipelineLayout layout, VkShaderModule vs, VkShaderModule fs,
            VkExtent2D rate, uint8_t *pixels)
{
   DECLARE_DEVICE_FUNCS;
   if (!destroy_device || !get_device_queue || !create_image ||
       !get_image_memory_requirements || !allocate_memory || !bind_image_memory ||
       !create_image_view || !create_buffer || !get_buffer_memory_requirements ||
       !bind_buffer_memory || !map_memory || !unmap_memory ||
       !create_pipeline_layout || !create_graphics_pipelines ||
       !create_command_pool || !allocate_command_buffers ||
       !begin_command_buffer || !end_command_buffer || !cmd_begin_render_pass ||
       !cmd_end_render_pass || !cmd_bind_pipeline || !cmd_draw ||
       !cmd_copy_image_to_buffer || !cmd_pipeline_barrier || !queue_submit ||
       !create_fence || !wait_for_fences)
      return 0;

   VkImage image = VK_NULL_HANDLE;
   VkDeviceMemory image_mem = VK_NULL_HANDLE, buffer_mem = VK_NULL_HANDLE;
   VkImageView image_view = VK_NULL_HANDLE;
   VkFramebuffer framebuffer = VK_NULL_HANDLE;
   VkBuffer staging = VK_NULL_HANDLE;
   VkPipeline pipeline = VK_NULL_HANDLE;
   VkCommandPool pool = VK_NULL_HANDLE;
   VkFence fence = VK_NULL_HANDLE;
   int ok = 0;

   VkImageCreateInfo image_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = format,
      .extent = {W, H, 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
   };
   if (create_image(device, &image_info, NULL, &image) != VK_SUCCESS)
      goto done;
   VkMemoryRequirements image_req;
   get_image_memory_requirements(device, image, &image_req);
   uint32_t image_type = memory_type(mem_props, image_req.memoryTypeBits,
                                     0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
   if (image_type == UINT32_MAX)
      goto done;
   VkMemoryAllocateInfo image_alloc = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = image_req.size,
      .memoryTypeIndex = image_type,
   };
   if (allocate_memory(device, &image_alloc, NULL, &image_mem) != VK_SUCCESS ||
       bind_image_memory(device, image, image_mem, 0) != VK_SUCCESS)
      goto done;

   VkImageViewCreateInfo view_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                     VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
   };
   if (create_image_view(device, &view_info, NULL, &image_view) != VK_SUCCESS)
      goto done;

   VkFramebufferCreateInfo framebuffer_info = {
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = render_pass,
      .attachmentCount = 1,
      .pAttachments = &image_view,
      .width = W,
      .height = H,
      .layers = 1,
   };
   PFN_vkCreateFramebuffer create_framebuffer = LOAD_D(CreateFramebuffer);
   PFN_vkDestroyFramebuffer destroy_framebuffer = LOAD_D(DestroyFramebuffer);
   if (!create_framebuffer || !destroy_framebuffer ||
       create_framebuffer(device, &framebuffer_info, NULL, &framebuffer) != VK_SUCCESS)
      goto done;

   VkBufferCreateInfo buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = IMAGE_BYTES,
      .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
   };
   if (create_buffer(device, &buffer_info, NULL, &staging) != VK_SUCCESS)
      goto done;
   VkMemoryRequirements buffer_req;
   get_buffer_memory_requirements(device, staging, &buffer_req);
   uint32_t buffer_type = memory_type(mem_props, buffer_req.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
   if (buffer_type == UINT32_MAX)
      goto done;
   VkMemoryAllocateInfo buffer_alloc = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = buffer_req.size,
      .memoryTypeIndex = buffer_type,
   };
   if (allocate_memory(device, &buffer_alloc, NULL, &buffer_mem) != VK_SUCCESS ||
       bind_buffer_memory(device, staging, buffer_mem, 0) != VK_SUCCESS)
      goto done;

   VkPipelineShaderStageCreateInfo stages[2] = {
      {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
       .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = vs, .pName = "main"},
      {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
       .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = fs, .pName = "main"},
   };
   VkPipelineVertexInputStateCreateInfo vertex_input = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
   };
   VkPipelineInputAssemblyStateCreateInfo assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
   };
   VkViewport viewport = {0, 0, (float)W, (float)H, 0, 1};
   VkRect2D scissor = {{0, 0}, {W, H}};
   VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1, .pViewports = &viewport,
      .scissorCount = 1, .pScissors = &scissor,
   };
   VkPipelineRasterizationStateCreateInfo raster = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_NONE,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
      .lineWidth = 1.0f,
   };
   VkPipelineMultisampleStateCreateInfo ms = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
   };
   VkPipelineColorBlendAttachmentState blend_attachment = {
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
   };
   VkPipelineColorBlendStateCreateInfo blend = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1, .pAttachments = &blend_attachment,
   };
   VkPipelineFragmentShadingRateStateCreateInfoKHR fsr = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR,
      .fragmentSize = rate,
      .combinerOps = {VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR,
                      VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR},
   };
   VkGraphicsPipelineCreateInfo pipeline_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &fsr,
      .stageCount = 2, .pStages = stages,
      .pVertexInputState = &vertex_input,
      .pInputAssemblyState = &assembly,
      .pViewportState = &viewport_state,
      .pRasterizationState = &raster,
      .pMultisampleState = &ms,
      .pColorBlendState = &blend,
      .layout = layout,
      .renderPass = render_pass,
      .subpass = 0,
   };
   if (create_graphics_pipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                 NULL, &pipeline) != VK_SUCCESS)
      goto done;

   VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family,
   };
   if (create_command_pool(device, &pool_info, NULL, &pool) != VK_SUCCESS)
      goto done;
   VkCommandBufferAllocateInfo cb_alloc = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
   };
   PFN_vkAllocateCommandBuffers alloc_cb = LOAD_D(AllocateCommandBuffers);
   if (!alloc_cb)
      goto done;
   VkCommandBuffer cb;
   if (alloc_cb(device, &cb_alloc, &cb) != VK_SUCCESS)
      goto done;

   VkCommandBufferBeginInfo begin = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
   };
   if (begin_command_buffer(cb, &begin) != VK_SUCCESS)
      goto done;
   VkClearValue clear = {.color = {{0, 0, 0, 1}}};
   VkRenderPassBeginInfo rp_begin = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = render_pass,
      .framebuffer = framebuffer,
      .renderArea = {{0, 0}, {W, H}},
      .clearValueCount = 1,
      .pClearValues = &clear,
   };
   cmd_begin_render_pass(cb, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
   cmd_bind_pipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
   cmd_draw(cb, 3, 1, 0, 0);
   cmd_end_render_pass(cb);

   VkBufferImageCopy copy = {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
      .imageOffset = {0, 0, 0},
      .imageExtent = {W, H, 1},
   };
   cmd_copy_image_to_buffer(cb, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                            staging, 1, &copy);
   VkBufferMemoryBarrier host_barrier = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = staging,
      .offset = 0,
      .size = VK_WHOLE_SIZE,
   };
   cmd_pipeline_barrier(cb, VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_HOST_BIT, 0, 0, NULL, 1,
                        &host_barrier, 0, NULL);
   if (end_command_buffer(cb) != VK_SUCCESS)
      goto done;

   VkFenceCreateInfo fence_info = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
   if (create_fence(device, &fence_info, NULL, &fence) != VK_SUCCESS)
      goto done;
   VkSubmitInfo submit = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &cb,
   };
   if (queue_submit(queue, 1, &submit, fence) != VK_SUCCESS)
      goto done;
   PFN_vkWaitForFences wait_fences = LOAD_D(WaitForFences);
   if (!wait_fences || wait_fences(device, 1, &fence, VK_TRUE, 30ull * 1000ull * 1000ull * 1000ull) != VK_SUCCESS)
      goto done;

   void *mapped = NULL;
   if (map_memory(device, buffer_mem, 0, IMAGE_BYTES, 0, &mapped) != VK_SUCCESS)
      goto done;
   memcpy(pixels, mapped, IMAGE_BYTES);
   unmap_memory(device, buffer_mem);
   ok = 1;

done:
   if (fence && destroy_fence) destroy_fence(device, fence, NULL);
   if (framebuffer && destroy_framebuffer) destroy_framebuffer(device, framebuffer, NULL);
   if (pool && destroy_command_pool) destroy_command_pool(device, pool, NULL);
   if (pipeline && destroy_pipeline) destroy_pipeline(device, pipeline, NULL);
   if (staging && destroy_buffer) destroy_buffer(device, staging, NULL);
   if (buffer_mem && free_memory) free_memory(device, buffer_mem, NULL);
   if (image_view && destroy_image_view) destroy_image_view(device, image_view, NULL);
   if (image && destroy_image) destroy_image(device, image, NULL);
   if (image_mem && free_memory) free_memory(device, image_mem, NULL);
   return ok;
}

static uint64_t
checksum(const uint8_t *p)
{
   uint64_t h = 1469598103934665603ull;
   for (uint32_t i = 0; i < IMAGE_BYTES; i++) {
      h ^= p[i];
      h *= 1099511628211ull;
   }
   return h;
}

static uint32_t
equal_neighbors(const uint8_t *p)
{
   uint32_t equal = 0;
   for (uint32_t y = 0; y < H; y++) {
      for (uint32_t x = 0; x < W; x++) {
         const uint8_t *a = &p[(y * W + x) * 4];
         if (x + 1 < W && memcmp(a, a + 4, 4) == 0) equal++;
         if (y + 1 < H && memcmp(a, a + W * 4, 4) == 0) equal++;
      }
   }
   return equal;
}

int
main(void)
{
   logf = fopen("panvk_vrs_render_probe.log", "w");
   if (!logf) return 2;
   LOG("PanVK VRS render probe: 1x1 control vs 2x2 pipeline rate\n");

   HMODULE loader = LoadLibraryA("vulkan-1.dll");
   if (!loader) {
      LOG("FAIL: LoadLibrary(vulkan-1.dll), error=%lu\n", (unsigned long)GetLastError());
      fclose(logf); return 1;
   }
   gipa = (PFN_vkGetInstanceProcAddr)GetProcAddress(loader, "vkGetInstanceProcAddr");
   if (!gipa) { LOG("FAIL: vkGetInstanceProcAddr missing\n"); fclose(logf); return 1; }

   PFN_vkCreateInstance create_instance = (PFN_vkCreateInstance)gipa(VK_NULL_HANDLE, "vkCreateInstance");
   if (!create_instance) { LOG("FAIL: vkCreateInstance missing\n"); fclose(logf); return 1; }
   VkApplicationInfo app = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "PanVK VRS render probe",
      .applicationVersion = 1,
      .pEngineName = "PanVK VRS render probe",
      .engineVersion = 1,
      .apiVersion = VK_API_VERSION_1_3,
   };
   VkInstanceCreateInfo ici = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app,
   };
   VkInstance instance = VK_NULL_HANDLE;
   VkResult result = create_instance(&ici, NULL, &instance);
   if (result != VK_SUCCESS) { LOG("FAIL: vkCreateInstance=%d\n", result); fclose(logf); return 1; }

   PFN_vkEnumeratePhysicalDevices enumerate = LOAD_I(EnumeratePhysicalDevices);
   PFN_vkGetPhysicalDeviceProperties2 get_props2 = LOAD_I(GetPhysicalDeviceProperties2);
   PFN_vkGetPhysicalDeviceFeatures2 get_features2 = LOAD_I(GetPhysicalDeviceFeatures2);
   PFN_vkEnumerateDeviceExtensionProperties enum_ext = LOAD_I(EnumerateDeviceExtensionProperties);
   PFN_vkGetPhysicalDeviceQueueFamilyProperties get_queues = LOAD_I(GetPhysicalDeviceQueueFamilyProperties);
   PFN_vkGetPhysicalDeviceMemoryProperties get_mem_props = LOAD_I(GetPhysicalDeviceMemoryProperties);
   PFN_vkGetPhysicalDeviceFormatProperties get_format_props = LOAD_I(GetPhysicalDeviceFormatProperties);
   PFN_vkCreateDevice create_device = LOAD_I(CreateDevice);
   PFN_vkGetDeviceProcAddr get_device_proc_addr = LOAD_I(GetDeviceProcAddr);
   PFN_vkDestroyInstance destroy_instance = LOAD_I(DestroyInstance);
   if (!enumerate || !get_props2 || !get_features2 || !enum_ext || !get_queues ||
       !get_mem_props || !get_format_props || !create_device ||
       !get_device_proc_addr || !destroy_instance) {
      LOG("FAIL: required instance entrypoint missing\n");
      fclose(logf); return 1;
   }

   uint32_t pd_count = 0;
   result = enumerate(instance, &pd_count, NULL);
   if (result != VK_SUCCESS || !pd_count) {
      LOG("FAIL: no physical device, result=%d count=%u\n", result, pd_count);
      destroy_instance(instance, NULL); fclose(logf); return 1;
   }
   VkPhysicalDevice *pds = calloc(pd_count, sizeof(*pds));
   if (!pds) { destroy_instance(instance, NULL); fclose(logf); return 1; }
   enumerate(instance, &pd_count, pds);

   int passed = 0;
   for (uint32_t d = 0; d < pd_count && !passed; d++) {
      VkPhysicalDeviceDriverProperties driver = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
      };
      VkPhysicalDeviceProperties2 props = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
         .pNext = &driver,
      };
      get_props2(pds[d], &props);
      LOG("\nDevice %u: %s, driver=%s (%s), API=%u.%u.%u\n", d,
          props.properties.deviceName, driver.driverName, driver.driverInfo,
          VK_VERSION_MAJOR(props.properties.apiVersion),
          VK_VERSION_MINOR(props.properties.apiVersion),
          VK_VERSION_PATCH(props.properties.apiVersion));

      uint32_t ext_count = 0;
      enum_ext(pds[d], NULL, &ext_count, NULL);
      VkExtensionProperties *exts = calloc(ext_count ? ext_count : 1, sizeof(*exts));
      int fsr_ext = 0;
      if (exts && enum_ext(pds[d], NULL, &ext_count, exts) == VK_SUCCESS) {
         for (uint32_t i = 0; i < ext_count; i++)
            if (!strcmp(exts[i].extensionName, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
               fsr_ext = 1;
      }
      free(exts);
      if (!fsr_ext) { LOG("SKIP: FSR extension absent\n"); continue; }

      VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr_features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
      };
      VkPhysicalDeviceFeatures2 features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &fsr_features,
      };
      get_features2(pds[d], &features);
      LOG("pipelineFragmentShadingRate=%u\n", fsr_features.pipelineFragmentShadingRate);
      if (!fsr_features.pipelineFragmentShadingRate) continue;

      VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
      VkFormatProperties fp;
      get_format_props(pds[d], format, &fp);
      VkFormatFeatureFlags need = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                  VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
      if ((fp.optimalTilingFeatures & need) != need) {
         LOG("SKIP: RGBA8 optimal format lacks color-attachment/transfer-src support (0x%x)\n",
             fp.optimalTilingFeatures);
         continue;
      }

      uint32_t qcount = 0;
      get_queues(pds[d], &qcount, NULL);
      VkQueueFamilyProperties *qprops = calloc(qcount ? qcount : 1, sizeof(*qprops));
      if (!qprops) continue;
      get_queues(pds[d], &qcount, qprops);
      uint32_t qf = UINT32_MAX;
      for (uint32_t q = 0; q < qcount; q++)
         if (qprops[q].queueCount && (qprops[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)) { qf = q; break; }
      free(qprops);
      if (qf == UINT32_MAX) { LOG("FAIL: no graphics queue\n"); continue; }

      float priority = 1.0f;
      VkDeviceQueueCreateInfo qci = {
         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = qf, .queueCount = 1, .pQueuePriorities = &priority,
      };
      VkPhysicalDeviceFragmentShadingRateFeaturesKHR enable_fsr = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
         .pipelineFragmentShadingRate = VK_TRUE,
      };
      VkPhysicalDeviceFeatures2 enable_features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &enable_fsr,
      };
      const char *extensions[] = {VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME};
      VkDeviceCreateInfo dci = {
         .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
         .pNext = &enable_features,
         .queueCreateInfoCount = 1, .pQueueCreateInfos = &qci,
         .enabledExtensionCount = 1, .ppEnabledExtensionNames = extensions,
      };
      VkDevice device = VK_NULL_HANDLE;
      result = create_device(pds[d], &dci, NULL, &device);
      LOG("vkCreateDevice=%d\n", result);
      if (result != VK_SUCCESS) continue;
      PFN_vkGetDeviceProcAddr gdpa = get_device_proc_addr;
      PFN_vkGetPhysicalDeviceMemoryProperties get_memory = get_mem_props;
      VkPhysicalDeviceMemoryProperties mem_props;
      get_memory(pds[d], &mem_props);
      PFN_vkGetDeviceQueue get_queue = (PFN_vkGetDeviceQueue)gdpa(device, "vkGetDeviceQueue");
      PFN_vkCreateRenderPass create_rp = (PFN_vkCreateRenderPass)gdpa(device, "vkCreateRenderPass");
      PFN_vkCreateFramebuffer create_fb = (PFN_vkCreateFramebuffer)gdpa(device, "vkCreateFramebuffer");
      PFN_vkCreateShaderModule create_sm = (PFN_vkCreateShaderModule)gdpa(device, "vkCreateShaderModule");
      PFN_vkCreatePipelineLayout create_pl = (PFN_vkCreatePipelineLayout)gdpa(device, "vkCreatePipelineLayout");
      PFN_vkDestroyShaderModule destroy_sm = (PFN_vkDestroyShaderModule)gdpa(device, "vkDestroyShaderModule");
      PFN_vkDestroyPipelineLayout destroy_pl = (PFN_vkDestroyPipelineLayout)gdpa(device, "vkDestroyPipelineLayout");
      PFN_vkDestroyFramebuffer destroy_fb = (PFN_vkDestroyFramebuffer)gdpa(device, "vkDestroyFramebuffer");
      PFN_vkDestroyRenderPass destroy_rp = (PFN_vkDestroyRenderPass)gdpa(device, "vkDestroyRenderPass");
      PFN_vkDestroyDevice destroy_dev = (PFN_vkDestroyDevice)gdpa(device, "vkDestroyDevice");
      if (!get_queue || !create_rp || !create_fb || !create_sm || !create_pl ||
          !destroy_sm || !destroy_pl || !destroy_fb || !destroy_rp || !destroy_dev) {
         LOG("FAIL: render setup entrypoint missing\n");
         destroy_dev(device, NULL); continue;
      }
      VkQueue queue = VK_NULL_HANDLE;
      get_queue(device, qf, 0, &queue);

      VkAttachmentDescription attachment = {
         .format = format, .samples = VK_SAMPLE_COUNT_1_BIT,
         .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
         .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
         .finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      };
      VkAttachmentReference color_ref = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
      VkSubpassDescription subpass = {
         .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
         .colorAttachmentCount = 1, .pColorAttachments = &color_ref,
      };
      VkSubpassDependency dependency = {
         .srcSubpass = 0, .dstSubpass = VK_SUBPASS_EXTERNAL,
         .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
         .dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
         .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
         .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
      };
      VkRenderPassCreateInfo rpci = {
         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
         .attachmentCount = 1, .pAttachments = &attachment,
         .subpassCount = 1, .pSubpasses = &subpass,
         .dependencyCount = 1, .pDependencies = &dependency,
      };
      VkRenderPass rp = VK_NULL_HANDLE;
      result = create_rp(device, &rpci, NULL, &rp);
      LOG("vkCreateRenderPass=%d\n", result);
      if (result != VK_SUCCESS) { destroy_dev(device, NULL); continue; }

      VkShaderModuleCreateInfo smv={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_vert_spv),.pCode=vrs_vert_spv};
      VkShaderModuleCreateInfo smf={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_frag_spv),.pCode=vrs_frag_spv};
      VkShaderModule vs=VK_NULL_HANDLE,fs=VK_NULL_HANDLE;
      result=create_sm(device,&smv,NULL,&vs);
      if(result==VK_SUCCESS)result=create_sm(device,&smf,NULL,&fs);
      VkPipelineLayout pl=VK_NULL_HANDLE;
      VkPipelineLayoutCreateInfo plci={.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
      if(result==VK_SUCCESS)result=create_pl(device,&plci,NULL,&pl);
      LOG("shader modules/pipeline layout=%d\n",result);
      if(result!=VK_SUCCESS){destroy_dev(device,NULL);continue;}

      uint8_t *image_1x1=malloc(IMAGE_BYTES),*image_2x2=malloc(IMAGE_BYTES);
      if(!image_1x1||!image_2x2){LOG("FAIL: host allocation\n");destroy_dev(device,NULL);continue;}
      int ok1=render_rate(device,&mem_props,gdpa,queue,qf,format,rp,pl,vs,fs,(VkExtent2D){1,1},image_1x1);
      LOG("1x1 draw submit/fence/readback=%s\n",ok1?"PASS":"FAIL");
      int ok2=render_rate(device,&mem_props,gdpa,queue,qf,format,rp,pl,vs,fs,(VkExtent2D){2,2},image_2x2);
      LOG("2x2 draw submit/fence/readback=%s\n",ok2?"PASS":"FAIL");
      if(ok1&&ok2){
         uint32_t e1=equal_neighbors(image_1x1),e2=equal_neighbors(image_2x2);
         uint64_t h1=checksum(image_1x1),h2=checksum(image_2x2);
         LOG("1x1 checksum=0x%016llx equal-neighbor-count=%u\n",(unsigned long long)h1,e1);
         LOG("2x2 checksum=0x%016llx equal-neighbor-count=%u\n",(unsigned long long)h2,e2);
         LOG("image comparison: %s\n",h1!=h2?"different":"identical");
         if(h1!=h2 && e2>e1+W*H/2){
            LOG("PASS: GPU executed both rates and 2x2 produced coarse repeated output\n");
            passed=1;
         } else {
            LOG("FAIL: readback did not show the expected 2x2 coarse-rate signature\n");
         }
      }
      free(image_1x1);free(image_2x2);
      destroy_sm(device,vs,NULL);destroy_sm(device,fs,NULL);
      destroy_pl(device,pl,NULL);destroy_fb(device,fb,NULL);
      destroy_rp(device,rp,NULL);
      destroy_dev(device,NULL);
   }
   free(pds);
   destroy_instance(instance,NULL);
   LOG("\n%s\n",passed?"VRS GPU RENDER PASS":"VRS GPU RENDER FAIL");
   fclose(logf);
   return passed?0:1;
}

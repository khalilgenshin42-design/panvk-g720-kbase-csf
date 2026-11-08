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
#include <stdbool.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include "vrs_probe_spv.h"

/* Fail the probe build if its Vulkan-Headers enum ABI is not the spec ABI. */
_Static_assert(VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR == 0, "Vulkan FSR KEEP enum mismatch");
_Static_assert(VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR == 1, "Vulkan FSR REPLACE enum mismatch");
_Static_assert(VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR == 2, "Vulkan FSR MIN enum mismatch");
_Static_assert(VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR == 3, "Vulkan FSR MAX enum mismatch");
_Static_assert(VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR == 4, "Vulkan FSR MUL enum mismatch");

#define W 64u
#define H 64u
#define IMAGE_BYTES (W * H * 4u)

#define LOAD_I(name) ((PFN_vk##name)gipa(instance, "vk" #name))
#define LOAD_D(name) ((PFN_vk##name)get_device_proc_addr(device, "vk" #name))

static FILE *log_file;
static PFN_vkGetInstanceProcAddr gipa;

#define LOG(...) do { if (log_file) { fprintf(log_file, __VA_ARGS__); fflush(log_file); } } while (0)

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
   PFN_vkCreateSampler create_sampler = LOAD_D(CreateSampler); \
   PFN_vkDestroySampler destroy_sampler = LOAD_D(DestroySampler); \
   PFN_vkCmdCopyBufferToImage cmd_copy_buffer_to_image = LOAD_D(CmdCopyBufferToImage); \
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
   PFN_vkCmdBindDescriptorSets cmd_bind_descriptor_sets = LOAD_D(CmdBindDescriptorSets); \
   PFN_vkCmdDraw cmd_draw = LOAD_D(CmdDraw); \
   PFN_vkCmdCopyImageToBuffer cmd_copy_image_to_buffer = LOAD_D(CmdCopyImageToBuffer); \
   PFN_vkCmdPipelineBarrier cmd_pipeline_barrier = LOAD_D(CmdPipelineBarrier); \
   PFN_vkQueueSubmit queue_submit = LOAD_D(QueueSubmit); \
   PFN_vkCreateFence create_fence = LOAD_D(CreateFence); \
   PFN_vkDestroyFence destroy_fence = LOAD_D(DestroyFence); \
   PFN_vkWaitForFences wait_for_fences = LOAD_D(WaitForFences); \
   PFN_vkDeviceWaitIdle device_wait_idle = LOAD_D(DeviceWaitIdle); \
   PFN_vkCmdSetFragmentShadingRateKHR cmd_set_fragment_shading_rate = LOAD_D(CmdSetFragmentShadingRateKHR)


/* Five constant-colour mip levels expose implicit-LOD selection in readback. */
static int
create_mip_texture(VkDevice device,
                   const VkPhysicalDeviceMemoryProperties *mem_props,
                   PFN_vkGetDeviceProcAddr get_device_proc_addr,
                   VkQueue queue, uint32_t queue_family,
                   VkImage *out_image, VkDeviceMemory *out_image_mem,
                   VkImageView *out_view, VkSampler *out_sampler)
{
   DECLARE_DEVICE_FUNCS;
   if (!create_image || !destroy_image || !get_image_memory_requirements ||
       !allocate_memory || !free_memory || !bind_image_memory ||
       !create_buffer || !destroy_buffer || !get_buffer_memory_requirements ||
       !bind_buffer_memory || !map_memory || !unmap_memory ||
       !create_image_view || !destroy_image_view || !create_sampler ||
       !destroy_sampler || !cmd_pipeline_barrier || !cmd_copy_buffer_to_image ||
       !create_command_pool || !destroy_command_pool || !allocate_command_buffers ||
       !begin_command_buffer || !end_command_buffer || !queue_submit ||
       !create_fence || !destroy_fence || !wait_for_fences)
      return 0;

   const uint32_t levels = 5;
   const VkDeviceSize bytes = (256u*256u + 128u*128u + 64u*64u +
                               32u*32u + 16u*16u) * 4u;
   VkImage image=VK_NULL_HANDLE;
   VkDeviceMemory image_mem=VK_NULL_HANDLE, upload_mem=VK_NULL_HANDLE;
   VkBuffer upload=VK_NULL_HANDLE;
   VkImageView view=VK_NULL_HANDLE;
   VkSampler sampler=VK_NULL_HANDLE;
   VkCommandPool pool=VK_NULL_HANDLE;
   VkFence fence=VK_NULL_HANDLE;
   int submitted=0, ok=0;
   VkResult vr;
   VkImageCreateInfo ii={.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType=VK_IMAGE_TYPE_2D,.format=VK_FORMAT_R8G8B8A8_UNORM,
      .extent={256,256,1},.mipLevels=levels,.arrayLayers=1,
      .samples=VK_SAMPLE_COUNT_1_BIT,.tiling=VK_IMAGE_TILING_OPTIMAL,
      .usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
      .sharingMode=VK_SHARING_MODE_EXCLUSIVE,.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED};
   vr=create_image(device,&ii,NULL,&image);
   LOG("mip vkCreateImage=%d\\n",vr);
   if(vr!=VK_SUCCESS)goto done;
   VkMemoryRequirements ir; get_image_memory_requirements(device,image,&ir);
   uint32_t it=memory_type(mem_props,ir.memoryTypeBits,0,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
   if(it==UINT32_MAX)goto done;
   VkMemoryAllocateInfo ia={.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize=ir.size,.memoryTypeIndex=it};
   vr=allocate_memory(device,&ia,NULL,&image_mem); if(vr!=VK_SUCCESS)goto done;
   vr=bind_image_memory(device,image,image_mem,0); if(vr!=VK_SUCCESS)goto done;

   VkBufferCreateInfo bi={.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,.size=bytes,
      .usage=VK_BUFFER_USAGE_TRANSFER_SRC_BIT,.sharingMode=VK_SHARING_MODE_EXCLUSIVE};
   vr=create_buffer(device,&bi,NULL,&upload); if(vr!=VK_SUCCESS)goto done;
   VkMemoryRequirements br; get_buffer_memory_requirements(device,upload,&br);
   uint32_t bt=memory_type(mem_props,br.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
   if(bt==UINT32_MAX)goto done;
   VkMemoryAllocateInfo ba={.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize=br.size,.memoryTypeIndex=bt};
   vr=allocate_memory(device,&ba,NULL,&upload_mem); if(vr!=VK_SUCCESS)goto done;
   vr=bind_buffer_memory(device,upload,upload_mem,0); if(vr!=VK_SUCCESS)goto done;
   void *mapped=NULL; vr=map_memory(device,upload_mem,0,bytes,0,&mapped);
   if(vr!=VK_SUCCESS)goto done;
   static const uint8_t rgba[5][4]={{255,0,0,255},{0,255,0,255},
      {0,0,255,255},{255,255,0,255},{255,0,255,255}};
   VkBufferImageCopy regions[5]; VkDeviceSize offset=0;
   for(uint32_t l=0;l<levels;l++){
      uint32_t dim=256u>>l; uint8_t *dst=(uint8_t*)mapped+offset;
      for(uint32_t p=0;p<dim*dim;p++)memcpy(dst+p*4u,rgba[l],4);
      regions[l]=(VkBufferImageCopy){.bufferOffset=offset,
         .imageSubresource={VK_IMAGE_ASPECT_COLOR_BIT,l,0,1},
         .imageExtent={dim,dim,1}};
      offset+=(VkDeviceSize)dim*dim*4u;
   }
   unmap_memory(device,upload_mem);

   VkCommandPoolCreateInfo pci={.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags=VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,.queueFamilyIndex=queue_family};
   vr=create_command_pool(device,&pci,NULL,&pool); if(vr!=VK_SUCCESS)goto done;
   VkCommandBufferAllocateInfo cai={.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool=pool,.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY,.commandBufferCount=1};
   VkCommandBuffer cb=VK_NULL_HANDLE; vr=allocate_command_buffers(device,&cai,&cb);
   if(vr!=VK_SUCCESS)goto done;
   VkCommandBufferBeginInfo cbi={.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags=VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
   vr=begin_command_buffer(cb,&cbi); if(vr!=VK_SUCCESS)goto done;
   VkImageMemoryBarrier b0={.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .dstAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT,
      .oldLayout=VK_IMAGE_LAYOUT_UNDEFINED,.newLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
      .image=image,.subresourceRange={VK_IMAGE_ASPECT_COLOR_BIT,0,levels,0,1}};
   cmd_pipeline_barrier(cb,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,0,NULL,0,NULL,1,&b0);
   cmd_copy_buffer_to_image(cb,upload,image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,levels,regions);
   VkImageMemoryBarrier b1={.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT,.dstAccessMask=VK_ACCESS_SHADER_READ_BIT,
      .oldLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
      .image=image,.subresourceRange={VK_IMAGE_ASPECT_COLOR_BIT,0,levels,0,1}};
   cmd_pipeline_barrier(cb,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,0,NULL,0,NULL,1,&b1);
   vr=end_command_buffer(cb); if(vr!=VK_SUCCESS)goto done;
   VkFenceCreateInfo fi={.sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
   vr=create_fence(device,&fi,NULL,&fence); if(vr!=VK_SUCCESS)goto done;
   VkSubmitInfo si={.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount=1,.pCommandBuffers=&cb};
   vr=queue_submit(queue,1,&si,fence); if(vr!=VK_SUCCESS)goto done;
   submitted=1; vr=wait_for_fences(device,1,&fence,VK_TRUE,30000000000ull);
   LOG("mip upload fence=%d\\n",vr); if(vr!=VK_SUCCESS)goto done;

   VkImageViewCreateInfo vi={.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image=image,.viewType=VK_IMAGE_VIEW_TYPE_2D,.format=VK_FORMAT_R8G8B8A8_UNORM,
      .components={VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY,
         VK_COMPONENT_SWIZZLE_IDENTITY,VK_COMPONENT_SWIZZLE_IDENTITY},
      .subresourceRange={VK_IMAGE_ASPECT_COLOR_BIT,0,levels,0,1}};
   vr=create_image_view(device,&vi,NULL,&view); if(vr!=VK_SUCCESS)goto done;
   VkSamplerCreateInfo sci={.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter=VK_FILTER_NEAREST,.minFilter=VK_FILTER_NEAREST,
      .mipmapMode=VK_SAMPLER_MIPMAP_MODE_NEAREST,
      .addressModeU=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW=VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .minLod=0.0f,.maxLod=4.0f,.borderColor=VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK};
   vr=create_sampler(device,&sci,NULL,&sampler); if(vr!=VK_SUCCESS)goto done;
   *out_image=image;*out_image_mem=image_mem;*out_view=view;*out_sampler=sampler;
   image=VK_NULL_HANDLE;image_mem=VK_NULL_HANDLE;view=VK_NULL_HANDLE;
   sampler=VK_NULL_HANDLE;ok=1;
done:
   if(submitted&&!ok&&device_wait_idle)device_wait_idle(device);
   if(fence)destroy_fence(device,fence,NULL);
   if(pool)destroy_command_pool(device,pool,NULL);
   if(upload)destroy_buffer(device,upload,NULL);
   if(upload_mem)free_memory(device,upload_mem,NULL);
   if(sampler)destroy_sampler(device,sampler,NULL);
   if(view)destroy_image_view(device,view,NULL);
   if(image)destroy_image(device,image,NULL);
   if(image_mem)free_memory(device,image_mem,NULL);
   return ok;
}

static int
render_rate(VkDevice device, VkPhysicalDeviceMemoryProperties *mem_props,
            PFN_vkGetDeviceProcAddr get_device_proc_addr,
            VkQueue queue, uint32_t queue_family, VkFormat format,
            VkRenderPass render_pass, VkPipelineLayout layout, VkShaderModule vs, VkShaderModule fs,
            VkFragmentShadingRateCombinerOpKHR op0,
            VkFragmentShadingRateCombinerOpKHR op1,
            VkExtent2D rate, bool dynamic_rate,
            VkDescriptorSet invocation_set,
            VkBuffer invocation_buffer, VkDeviceMemory invocation_mem,
            uint32_t *invocations, uint8_t *pixels)
{
   DECLARE_DEVICE_FUNCS;
   if (!destroy_device || !get_device_queue || !create_image ||
       !get_image_memory_requirements || !allocate_memory || !bind_image_memory ||
       !create_image_view || !create_buffer || !get_buffer_memory_requirements ||
       !bind_buffer_memory || !map_memory || !unmap_memory ||
       !create_pipeline_layout || !create_graphics_pipelines ||
       !create_command_pool || !allocate_command_buffers ||
       !begin_command_buffer || !end_command_buffer || !cmd_begin_render_pass ||
       !cmd_end_render_pass || !cmd_bind_pipeline || !cmd_bind_descriptor_sets || !cmd_draw ||
       !cmd_copy_image_to_buffer || !cmd_pipeline_barrier || !queue_submit ||
       !create_fence || !wait_for_fences ||
       (dynamic_rate && !cmd_set_fragment_shading_rate))
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
   int submitted = 0;

   LOG("  creating %ux%u pipeline and offscreen target\n", rate.width, rate.height);
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
   LOG("    stage: vkCreateImage begin\n");
   VkResult step_result = create_image(device, &image_info, NULL, &image);
   LOG("    stage: vkCreateImage result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   VkMemoryRequirements image_req;
   LOG("    stage: vkGetImageMemoryRequirements begin\n");
   get_image_memory_requirements(device, image, &image_req);
   LOG("    stage: image memory size=%llu typeBits=0x%x\n",
       (unsigned long long)image_req.size, image_req.memoryTypeBits);
   uint32_t image_type = memory_type(mem_props, image_req.memoryTypeBits,
                                     0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
   if (image_type == UINT32_MAX)
      goto done;
   VkMemoryAllocateInfo image_alloc = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = image_req.size,
      .memoryTypeIndex = image_type,
   };
   LOG("    stage: vkAllocateMemory(image) begin\n");
   step_result = allocate_memory(device, &image_alloc, NULL, &image_mem);
   LOG("    stage: vkAllocateMemory(image) result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   LOG("    stage: vkBindImageMemory begin\n");
   step_result = bind_image_memory(device, image, image_mem, 0);
   LOG("    stage: vkBindImageMemory result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
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
   LOG("    stage: vkCreateImageView begin\n");
   step_result = create_image_view(device, &view_info, NULL, &image_view);
   LOG("    stage: vkCreateImageView result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
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
   LOG("    stage: vkCreateFramebuffer begin\n");
   if (!create_framebuffer || !destroy_framebuffer ||
       (step_result = create_framebuffer(device, &framebuffer_info, NULL, &framebuffer)) != VK_SUCCESS)
      goto done;
   LOG("    stage: vkCreateFramebuffer result=%d\n", step_result);

   VkBufferCreateInfo buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = IMAGE_BYTES,
      .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
   };
   LOG("    stage: vkCreateBuffer begin\n");
   step_result = create_buffer(device, &buffer_info, NULL, &staging);
   LOG("    stage: vkCreateBuffer result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   VkMemoryRequirements buffer_req;
   LOG("    stage: vkGetBufferMemoryRequirements begin\n");
   get_buffer_memory_requirements(device, staging, &buffer_req);
   LOG("    stage: buffer memory size=%llu typeBits=0x%x\n",
       (unsigned long long)buffer_req.size, buffer_req.memoryTypeBits);
   uint32_t buffer_type = memory_type(mem_props, buffer_req.memoryTypeBits,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
   if (buffer_type == UINT32_MAX)
      goto done;
   VkMemoryAllocateInfo buffer_alloc = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = buffer_req.size,
      .memoryTypeIndex = buffer_type,
   };
   LOG("    stage: vkAllocateMemory(staging) begin\n");
   step_result = allocate_memory(device, &buffer_alloc, NULL, &buffer_mem);
   LOG("    stage: vkAllocateMemory(staging) result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   LOG("    stage: vkBindBufferMemory begin\n");
   step_result = bind_buffer_memory(device, staging, buffer_mem, 0);
   LOG("    stage: vkBindBufferMemory result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
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
      .combinerOps = {op0, op1},
   };
   const VkDynamicState dynamic_states[] = {
      VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR,
   };
   VkPipelineDynamicStateCreateInfo dynamic_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = 1,
      .pDynamicStates = dynamic_states,
   };
   VkGraphicsPipelineCreateInfo pipeline_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = dynamic_rate ? NULL : &fsr,
      .stageCount = 2, .pStages = stages,
      .pVertexInputState = &vertex_input,
      .pInputAssemblyState = &assembly,
      .pViewportState = &viewport_state,
      .pRasterizationState = &raster,
      .pMultisampleState = &ms,
      .pColorBlendState = &blend,
      .pDynamicState = dynamic_rate ? &dynamic_state : NULL,
      .layout = layout,
      .renderPass = render_pass,
      .subpass = 0,
      .basePipelineIndex = -1,
   };
   LOG("    stage: vkCreateGraphicsPipelines rate=%ux%u begin\n", rate.width, rate.height);
   step_result = create_graphics_pipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                           NULL, &pipeline);
   LOG("    stage: vkCreateGraphicsPipelines result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;

   VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family,
   };
   LOG("    stage: vkCreateCommandPool begin\n");
   step_result = create_command_pool(device, &pool_info, NULL, &pool);
   LOG("    stage: vkCreateCommandPool result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
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
   LOG("    stage: vkAllocateCommandBuffers begin\n");
   step_result = alloc_cb(device, &cb_alloc, &cb);
   LOG("    stage: vkAllocateCommandBuffers result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;

   VkCommandBufferBeginInfo begin = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
   };
   LOG("    stage: vkBeginCommandBuffer begin\n");
   step_result = begin_command_buffer(cb, &begin);
   LOG("    stage: vkBeginCommandBuffer result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
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
   cmd_bind_descriptor_sets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
                            0, 1, &invocation_set, 0, NULL);
   if (dynamic_rate) {
      const VkFragmentShadingRateCombinerOpKHR combiners[2] = {op0, op1};
      cmd_set_fragment_shading_rate(cb, &rate, combiners);
      LOG("    stage: vkCmdSetFragmentShadingRateKHR rate=%ux%u ops=%u,%u recorded\n",
          rate.width, rate.height, (unsigned)op0, (unsigned)op1);
   }
   cmd_draw(cb, 3, 1, 0, 0);
   cmd_end_render_pass(cb);

   VkBufferMemoryBarrier counter_barrier = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .buffer = invocation_buffer,
      .offset = 0,
      .size = sizeof(uint32_t),
   };
   cmd_pipeline_barrier(cb, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_HOST_BIT, 0, 0, NULL, 1,
                        &counter_barrier, 0, NULL);

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
   LOG("    stage: vkEndCommandBuffer begin\n");
   step_result = end_command_buffer(cb);
   LOG("    stage: vkEndCommandBuffer result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;

   /* Reset the host-coherent invocation counter before submitting this draw. */
   void *counter_reset = NULL;
   step_result = map_memory(device, invocation_mem, 0, sizeof(uint32_t), 0,
                            &counter_reset);
   LOG("    stage: map invocation counter for reset result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   *(uint32_t *)counter_reset = 0;
   unmap_memory(device, invocation_mem);

   VkFenceCreateInfo fence_info = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
   if (create_fence(device, &fence_info, NULL, &fence) != VK_SUCCESS)
      goto done;
   VkSubmitInfo submit = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .commandBufferCount = 1,
      .pCommandBuffers = &cb,
   };
   LOG("    stage: vkQueueSubmit begin\n");
   step_result = queue_submit(queue, 1, &submit, fence);
   LOG("    stage: vkQueueSubmit result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   submitted = 1;
   PFN_vkWaitForFences wait_fences = LOAD_D(WaitForFences);
   LOG("    stage: vkWaitForFences begin (30s)\n");
   step_result = wait_fences ? wait_fences(device, 1, &fence, VK_TRUE,
                                           30ull * 1000ull * 1000ull * 1000ull)
                             : VK_ERROR_INITIALIZATION_FAILED;
   LOG("    stage: vkWaitForFences result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;

   void *mapped = NULL;
   LOG("    stage: vkMapMemory begin\n");
   step_result = map_memory(device, buffer_mem, 0, IMAGE_BYTES, 0, &mapped);
   LOG("    stage: vkMapMemory result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   memcpy(pixels, mapped, IMAGE_BYTES);
   unmap_memory(device, buffer_mem);

   void *counter_result = NULL;
   step_result = map_memory(device, invocation_mem, 0, sizeof(uint32_t), 0,
                            &counter_result);
   LOG("    stage: map invocation counter for readback result=%d\n", step_result);
   if (step_result != VK_SUCCESS)
      goto done;
   *invocations = *(const uint32_t *)counter_result;
   unmap_memory(device, invocation_mem);
   ok = 1;

done:
   LOG("    render_rate cleanup: ok=%d submitted=%d\n", ok, submitted);
   if (submitted && !ok && device_wait_idle)
      device_wait_idle(device);
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

static void
equal_neighbor_axes(const uint8_t *p, uint32_t *horizontal, uint32_t *vertical)
{
   uint32_t h = 0, v = 0;
   for (uint32_t y = 0; y < H; y++) {
      for (uint32_t x = 0; x < W; x++) {
         const uint8_t *a = &p[(y * W + x) * 4];
         if (x + 1 < W && memcmp(a, a + 4, 4) == 0) h++;
         if (y + 1 < H && memcmp(a, a + W * 4, 4) == 0) v++;
      }
   }
   *horizontal = h;
   *vertical = v;
}

static uint32_t
equal_neighbors(const uint8_t *p)
{
   uint32_t horizontal, vertical;
   equal_neighbor_axes(p, &horizontal, &vertical);
   return horizontal + vertical;
}

/*
 * dFdx/dFdy are derivatives between fragment invocations, not pixels.
 * For a linear varying spanning [0,1] over the 64x64 target, a WxH
 * fragment therefore produces derivatives W/64 and H/64. The shader
 * stores those values scaled by 64/4 into B/A, giving 8-bit signatures
 * near W*255/4 and H*255/4. A +/-2 UNORM tolerance covers rounding.
 */
static int
derivative_signature_ok(const uint8_t *p, VkExtent2D rate)
{
   const uint8_t *pixel = &p[((H / 2) * W + (W / 2)) * 4];
   const int expected_x = (int)((rate.width * 255u + 2u) / 4u);
   const int expected_y = (int)((rate.height * 255u + 2u) / 4u);
   const int actual_x = pixel[2];
   const int actual_y = pixel[3];
   const int dx = actual_x - expected_x;
   const int dy = actual_y - expected_y;
   LOG("derivative signature rate=%ux%u: dFdx code=%d expected=%d, dFdy code=%d expected=%d result=%s\\n",
       rate.width, rate.height, actual_x, expected_x, actual_y, expected_y,
       (abs(dx) <= 2 && abs(dy) <= 2) ? "PASS" : "FAIL");
   return abs(dx) <= 2 && abs(dy) <= 2;
}

int
main(int argc, char **argv)
{
   const char *case_name = argc > 1 ? argv[1] : "--case=dynamic-keep";
   enum { CASE_KEEP, CASE_MAX, CASE_MIN, CASE_MUL, CASE_REPLACE,
          CASE_DYNAMIC_KEEP, CASE_DYNAMIC_ISOLATION, CASE_RATE_MATRIX, CASE_MIP_LOD } probe_case;

   if (!strcmp(case_name, "--case=keep"))
      probe_case = CASE_KEEP;
   else if (!strcmp(case_name, "--case=max"))
      probe_case = CASE_MAX;
   else if (!strcmp(case_name, "--case=min"))
      probe_case = CASE_MIN;
   else if (!strcmp(case_name, "--case=mul"))
      probe_case = CASE_MUL;
   else if (!strcmp(case_name, "--case=replace"))
      probe_case = CASE_REPLACE;
   else if (!strcmp(case_name, "--case=dynamic-keep"))
      probe_case = CASE_DYNAMIC_KEEP;
   else if (!strcmp(case_name, "--case=dynamic-isolation"))
      probe_case = CASE_DYNAMIC_ISOLATION;
   else if (!strcmp(case_name, "--case=rate-matrix"))
      probe_case = CASE_RATE_MATRIX;
   else if (!strcmp(case_name, "--case=mip-lod"))
      probe_case = CASE_MIP_LOD;
   else if (!strcmp(case_name, "--case=mip-lod"))
      probe_case = CASE_MIP_LOD;
   else {
      fprintf(stderr, "Usage: %s --case=keep|max|min|mul|replace|dynamic-keep|dynamic-isolation|rate-matrix\n", argv[0]);
      return 2;
   }
   log_file = fopen("panvk_vrs_render_probe.log", "w");
   if (!log_file) return 2;
   LOG("PanVK VRS render probe: static and dynamic pipeline-rate cases\n");
   LOG("Vulkan FSR enum ABI: KEEP=%u REPLACE=%u MIN=%u MAX=%u MUL=%u\\n",
       (unsigned)VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR,
       (unsigned)VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR,
       (unsigned)VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR,
       (unsigned)VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR,
       (unsigned)VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR);

   HMODULE loader = LoadLibraryA("vulkan-1.dll");
   if (!loader) {
      LOG("FAIL: LoadLibrary(vulkan-1.dll), error=%lu\n", (unsigned long)GetLastError());
      fclose(log_file); return 1;
   }
   gipa = (PFN_vkGetInstanceProcAddr)GetProcAddress(loader, "vkGetInstanceProcAddr");
   if (!gipa) { LOG("FAIL: vkGetInstanceProcAddr missing\n"); fclose(log_file); return 1; }

   PFN_vkCreateInstance create_instance = (PFN_vkCreateInstance)gipa(VK_NULL_HANDLE, "vkCreateInstance");
   if (!create_instance) { LOG("FAIL: vkCreateInstance missing\n"); fclose(log_file); return 1; }
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
   if (result != VK_SUCCESS) { LOG("FAIL: vkCreateInstance=%d\n", result); fclose(log_file); return 1; }

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
      fclose(log_file); return 1;
   }

   uint32_t pd_count = 0;
   result = enumerate(instance, &pd_count, NULL);
   if (result != VK_SUCCESS || !pd_count) {
      LOG("FAIL: no physical device, result=%d count=%u\n", result, pd_count);
      destroy_instance(instance, NULL); fclose(log_file); return 1;
   }
   VkPhysicalDevice *pds = calloc(pd_count, sizeof(*pds));
   if (!pds) { destroy_instance(instance, NULL); fclose(log_file); return 1; }
   enumerate(instance, &pd_count, pds);

   int passed = 0;
   for (uint32_t d = 0; d < pd_count && !passed; d++) {
      VkPhysicalDeviceFragmentShadingRatePropertiesKHR fsr_props = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR,
      };
      VkPhysicalDeviceDriverProperties driver = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
         .pNext = NULL,
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
      LOG("timestampComputeAndGraphics=%u timestampPeriod=%.9g ns\n",
          props.properties.limits.timestampComputeAndGraphics,
          props.properties.limits.timestampPeriod);

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
      driver.pNext = &fsr_props;
      get_props2(pds[d], &props);

      VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr_features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
      };
      VkPhysicalDeviceFeatures2 features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &fsr_features,
      };
      get_features2(pds[d], &features);
      LOG("pipelineFragmentShadingRate=%u primitiveFragmentShadingRate=%u attachmentFragmentShadingRate=%u\n",
          fsr_features.pipelineFragmentShadingRate,
          fsr_features.primitiveFragmentShadingRate,
          fsr_features.attachmentFragmentShadingRate);
      LOG("nonTrivialCombinerOps=%u strictMultiplyCombiner=%u\n",
          fsr_props.fragmentShadingRateNonTrivialCombinerOps,
          fsr_props.fragmentShadingRateStrictMultiplyCombiner);
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
      uint32_t timestamp_valid_bits = 0;
      for (uint32_t q = 0; q < qcount; q++) {
         if (qprops[q].queueCount && (qprops[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            qf = q;
            timestamp_valid_bits = qprops[q].timestampValidBits;
            break;
         }
      }
      free(qprops);
      LOG("graphicsQueueFamily=%u timestampValidBits=%u\n", qf, timestamp_valid_bits);
      if (qf == UINT32_MAX) { LOG("FAIL: no graphics queue\n"); continue; }
      if (!props.properties.limits.timestampComputeAndGraphics ||
          !props.properties.limits.timestampPeriod || !timestamp_valid_bits)
         LOG("GPU timestamp queries unavailable; use fence timing only (CPU+GPU elapsed, not GPU-only)\n");
      else
         LOG("GPU timestamp queries available; period=%.9g ns, validBits=%u\n",
             props.properties.limits.timestampPeriod, timestamp_valid_bits);

      float priority = 1.0f;
      VkDeviceQueueCreateInfo qci = {
         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = qf, .queueCount = 1, .pQueuePriorities = &priority,
      };
      VkPhysicalDeviceFragmentShadingRateFeaturesKHR enable_fsr = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
         .pipelineFragmentShadingRate = VK_TRUE,
         .primitiveFragmentShadingRate = fsr_features.primitiveFragmentShadingRate,
         .attachmentFragmentShadingRate = fsr_features.attachmentFragmentShadingRate,
      };
      if (!features.features.fragmentStoresAndAtomics) {
         LOG("SKIP: fragmentStoresAndAtomics is not supported; invocation counter unavailable\n");
         continue;
      }
      LOG("fragmentStoresAndAtomics=1 (enabled for SSBO atomic counter)\n");
      VkPhysicalDeviceFeatures2 enable_features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &enable_fsr,
         .features = {.fragmentStoresAndAtomics = VK_TRUE},
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
      PFN_vkCreateBuffer create_buffer = LOAD_D(CreateBuffer);
      PFN_vkDestroyBuffer destroy_buffer = LOAD_D(DestroyBuffer);
      PFN_vkGetBufferMemoryRequirements get_buffer_memory_requirements = LOAD_D(GetBufferMemoryRequirements);
      PFN_vkAllocateMemory allocate_memory = LOAD_D(AllocateMemory);
      PFN_vkFreeMemory free_memory = LOAD_D(FreeMemory);
      PFN_vkBindBufferMemory bind_buffer_memory = LOAD_D(BindBufferMemory);
      PFN_vkCreateDescriptorSetLayout create_descriptor_set_layout = LOAD_D(CreateDescriptorSetLayout);
      PFN_vkDestroyDescriptorSetLayout destroy_descriptor_set_layout = LOAD_D(DestroyDescriptorSetLayout);
      PFN_vkCreateDescriptorPool create_descriptor_pool = LOAD_D(CreateDescriptorPool);
      PFN_vkDestroyDescriptorPool destroy_descriptor_pool = LOAD_D(DestroyDescriptorPool);
      PFN_vkAllocateDescriptorSets allocate_descriptor_sets = LOAD_D(AllocateDescriptorSets);
      PFN_vkUpdateDescriptorSets update_descriptor_sets = LOAD_D(UpdateDescriptorSets);
      PFN_vkCreateRenderPass create_rp = (PFN_vkCreateRenderPass)gdpa(device, "vkCreateRenderPass");
      PFN_vkCreateShaderModule create_sm = (PFN_vkCreateShaderModule)gdpa(device, "vkCreateShaderModule");
      PFN_vkCreatePipelineLayout create_pl = (PFN_vkCreatePipelineLayout)gdpa(device, "vkCreatePipelineLayout");
      PFN_vkDestroyShaderModule destroy_sm = (PFN_vkDestroyShaderModule)gdpa(device, "vkDestroyShaderModule");
      PFN_vkDestroyPipelineLayout destroy_pl = (PFN_vkDestroyPipelineLayout)gdpa(device, "vkDestroyPipelineLayout");
      PFN_vkDestroyRenderPass destroy_rp = (PFN_vkDestroyRenderPass)gdpa(device, "vkDestroyRenderPass");
      PFN_vkDestroyDevice destroy_dev = (PFN_vkDestroyDevice)gdpa(device, "vkDestroyDevice");
      if (!get_queue || !create_buffer || !destroy_buffer ||
          !get_buffer_memory_requirements || !allocate_memory || !free_memory ||
          !bind_buffer_memory || !create_descriptor_set_layout ||
          !destroy_descriptor_set_layout || !create_descriptor_pool ||
          !destroy_descriptor_pool || !allocate_descriptor_sets ||
          !update_descriptor_sets || !create_rp || !create_sm || !create_pl ||
          !destroy_sm || !destroy_pl || !destroy_rp || !destroy_dev) {
         LOG("FAIL: render setup entrypoint missing\n");
         if (destroy_dev) destroy_dev(device, NULL);
         continue;
      }
      VkQueue queue = VK_NULL_HANDLE;
      get_queue(device, qf, 0, &queue);

      VkDescriptorSetLayoutBinding counter_bindings[2] = {{
         .binding = 0,
         .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
         .descriptorCount = 1,
         .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
      }, {
         .binding = 1, .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
      }};
      VkDescriptorSetLayoutCreateInfo dslci = {
         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
         .bindingCount = 2, .pBindings = counter_bindings,
      };
      VkDescriptorSetLayout invocation_layout = VK_NULL_HANDLE;
      result = create_descriptor_set_layout(device, &dslci, NULL, &invocation_layout);
      LOG("vkCreateDescriptorSetLayout(invocation counter)=%d\n", result);
      if (result != VK_SUCCESS) { destroy_dev(device, NULL); continue; }

      VkDescriptorPoolSize pool_sizes[2] = {
         {.type=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,.descriptorCount=1},
         {.type=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,.descriptorCount=1},
      };
      VkDescriptorPoolCreateInfo dpci = {
         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
         .maxSets = 1,
         .poolSizeCount = 2,
         .pPoolSizes = pool_sizes,
      };
      VkDescriptorPool invocation_pool = VK_NULL_HANDLE;
      result = create_descriptor_pool(device, &dpci, NULL, &invocation_pool);
      LOG("vkCreateDescriptorPool(invocation counter)=%d\n", result);
      if (result != VK_SUCCESS) {
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_dev(device, NULL); continue;
      }

      VkDescriptorSetAllocateInfo dsai = {
         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
         .descriptorPool = invocation_pool,
         .descriptorSetCount = 1,
         .pSetLayouts = &invocation_layout,
      };
      VkDescriptorSet invocation_set = VK_NULL_HANDLE;
      result = allocate_descriptor_sets(device, &dsai, &invocation_set);
      LOG("vkAllocateDescriptorSets(invocation counter)=%d\n", result);
      if (result != VK_SUCCESS) {
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_dev(device, NULL); continue;
      }

      VkBufferCreateInfo counter_buffer_info = {
         .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         .size = sizeof(uint32_t),
         .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
         .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      };
      VkBuffer invocation_buffer = VK_NULL_HANDLE;
      result = create_buffer(device, &counter_buffer_info, NULL, &invocation_buffer);
      LOG("vkCreateBuffer(invocation counter)=%d\n", result);
      if (result != VK_SUCCESS) {
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_dev(device, NULL); continue;
      }

      VkMemoryRequirements counter_req;
      get_buffer_memory_requirements(device, invocation_buffer, &counter_req);
      uint32_t counter_type = memory_type(
         &mem_props, counter_req.memoryTypeBits,
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      if (counter_type == UINT32_MAX) {
         LOG("SKIP: no host-visible coherent memory for invocation counter\n");
         destroy_buffer(device, invocation_buffer, NULL);
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_dev(device, NULL); continue;
      }
      VkMemoryAllocateInfo counter_alloc = {
         .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
         .allocationSize = counter_req.size,
         .memoryTypeIndex = counter_type,
      };
      VkDeviceMemory invocation_mem = VK_NULL_HANDLE;
      result = allocate_memory(device, &counter_alloc, NULL, &invocation_mem);
      LOG("vkAllocateMemory(invocation counter)=%d\n", result);
      if (result == VK_SUCCESS)
         result = bind_buffer_memory(device, invocation_buffer, invocation_mem, 0);
      LOG("vkBindBufferMemory(invocation counter)=%d\n", result);
      if (result != VK_SUCCESS) {
         if (invocation_mem) free_memory(device, invocation_mem, NULL);
         destroy_buffer(device, invocation_buffer, NULL);
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_dev(device, NULL); continue;
      }

      VkImage mip_image=VK_NULL_HANDLE;
      VkImageView mip_view=VK_NULL_HANDLE;
      VkDeviceMemory mip_image_mem=VK_NULL_HANDLE;
      VkSampler mip_sampler=VK_NULL_HANDLE;
      int mip_resources_ok=1;
      if (probe_case==CASE_MIP_LOD) {
         VkFormatProperties mip_fp;
         get_format_props(pds[d],VK_FORMAT_R8G8B8A8_UNORM,&mip_fp);
         VkFormatFeatureFlags mip_need=VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
         if ((mip_fp.optimalTilingFeatures&mip_need)!=mip_need) {
            LOG("SKIP: RGBA8 sampled-image/transfer-dst support missing (0x%x)\\n",mip_fp.optimalTilingFeatures);
            mip_resources_ok=0;
         }
      }
      VkDescriptorBufferInfo counter_desc = {
         .buffer = invocation_buffer,
         .offset = 0,
         .range = sizeof(uint32_t),
      };
      VkWriteDescriptorSet counter_write = {
         .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
         .dstSet = invocation_set,
         .dstBinding = 0,
         .descriptorCount = 1,
         .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
         .pBufferInfo = &counter_desc,
      };
      update_descriptor_sets(device, 1, &counter_write, 0, NULL);
      if (probe_case==CASE_MIP_LOD && mip_resources_ok) {
         mip_resources_ok=create_mip_texture(device,&mem_props,get_device_proc_addr,queue,qf,
            &mip_image,&mip_image_mem,&mip_view,&mip_sampler);
         LOG("mip texture upload/setup=%s\\n",mip_resources_ok?"PASS":"FAIL");
         if (mip_resources_ok) {
            VkDescriptorImageInfo image_info={.sampler=mip_sampler,.imageView=mip_view,
               .imageLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
            VkWriteDescriptorSet image_write={.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
               .dstSet=invocation_set,.dstBinding=1,.descriptorCount=1,
               .descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
               .pImageInfo=&image_info};
            update_descriptor_sets(device,1,&image_write,0,NULL);
         }
      }

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
      if (result != VK_SUCCESS) {
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_buffer(device, invocation_buffer, NULL);
         free_memory(device, invocation_mem, NULL);
         destroy_dev(device, NULL);
         continue;
      }

      VkShaderModuleCreateInfo smv={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_vert_spv),.pCode=vrs_vert_spv};
      VkShaderModuleCreateInfo smf={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_frag_spv),.pCode=vrs_frag_spv};
      VkShaderModuleCreateInfo smf_constant={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_frag_constant_spv),.pCode=vrs_frag_constant_spv};
      VkShaderModuleCreateInfo smf_mip={.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,.codeSize=sizeof(vrs_frag_mip_spv),.pCode=vrs_frag_mip_spv};
      VkShaderModule vs=VK_NULL_HANDLE,fs=VK_NULL_HANDLE,fs_constant=VK_NULL_HANDLE,fs_mip=VK_NULL_HANDLE;
      result=create_sm(device,&smv,NULL,&vs);
      if(result==VK_SUCCESS)result=create_sm(device,&smf,NULL,&fs);
      if(result==VK_SUCCESS && probe_case==CASE_DYNAMIC_ISOLATION)
         result=create_sm(device,&smf_constant,NULL,&fs_constant);
      if(result==VK_SUCCESS && probe_case==CASE_MIP_LOD && mip_resources_ok)
         result=create_sm(device,&smf_mip,NULL,&fs_mip);
      VkPipelineLayout pl=VK_NULL_HANDLE;
      VkPipelineLayoutCreateInfo plci = {
         .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
         .setLayoutCount = 1,
         .pSetLayouts = &invocation_layout,
      };
      if(result==VK_SUCCESS)result=create_pl(device,&plci,NULL,&pl);
      LOG("shader modules/pipeline layout=%d\n",result);
      if (probe_case==CASE_MIP_LOD && !mip_resources_ok) result=VK_ERROR_FORMAT_NOT_SUPPORTED;
      if (result != VK_SUCCESS) {
         if (pl) destroy_pl(device, pl, NULL);
         if (fs_constant) destroy_sm(device, fs_constant, NULL);
         if (fs_mip) destroy_sm(device, fs_mip, NULL);
         if (mip_sampler) { PFN_vkDestroySampler x=(PFN_vkDestroySampler)get_device_proc_addr(device,"vkDestroySampler"); if(x)x(device,mip_sampler,NULL); }
         if (mip_view) { PFN_vkDestroyImageView x=(PFN_vkDestroyImageView)get_device_proc_addr(device,"vkDestroyImageView"); if(x)x(device,mip_view,NULL); }
         if (mip_image) { PFN_vkDestroyImage x=(PFN_vkDestroyImage)get_device_proc_addr(device,"vkDestroyImage"); if(x)x(device,mip_image,NULL); }
         if (mip_image_mem) { PFN_vkFreeMemory x=(PFN_vkFreeMemory)get_device_proc_addr(device,"vkFreeMemory"); if(x)x(device,mip_image_mem,NULL); }
         if (fs) destroy_sm(device, fs, NULL);
         if (vs) destroy_sm(device, vs, NULL);
         destroy_rp(device, rp, NULL);
         destroy_descriptor_pool(device, invocation_pool, NULL);
         destroy_descriptor_set_layout(device, invocation_layout, NULL);
         destroy_buffer(device, invocation_buffer, NULL);
         free_memory(device, invocation_mem, NULL);
         destroy_dev(device, NULL);
         continue;
      }

      LOG("selected render case: %s\n", case_name);
      uint8_t *image_1x1 = malloc(IMAGE_BYTES);
      uint8_t *image_2x2 = malloc(IMAGE_BYTES);
      uint8_t *image_target = malloc(IMAGE_BYTES);
      if (!image_1x1 || !image_2x2 || !image_target) {
         LOG("FAIL: host allocation\n");
      } else {
         const VkFragmentShadingRateCombinerOpKHR keep =
            VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR;
         uint32_t invocations_1x1 = 0, invocations_2x2 = 0, invocations_target = 0;
         int control_ok = render_rate(device, &mem_props, gdpa, queue, qf,
                                      format, rp, pl, vs, fs, keep, keep,
                                      (VkExtent2D){1,1}, false, invocation_set,
                                      invocation_buffer, invocation_mem,
                                      &invocations_1x1, image_1x1);
         LOG("1x1 KEEP control submit/fence/readback=%s\n",
             control_ok ? "PASS" : "FAIL");
         if (control_ok) {
            LOG("1x1 KEEP checksum=0x%016llx equal-neighbors=%u\n",
                (unsigned long long)checksum(image_1x1),
                equal_neighbors(image_1x1));
         }

         int vrs_ok = control_ok &&
            render_rate(device, &mem_props, gdpa, queue, qf, format, rp, pl,
                        vs, fs, keep, keep, (VkExtent2D){2,2}, false, invocation_set,
                        invocation_buffer, invocation_mem, &invocations_2x2,
                        image_2x2);
         LOG("2x2 KEEP submit/fence/readback=%s\n",
             vrs_ok ? "PASS" : "FAIL");
         uint64_t hash_1x1 = control_ok ? checksum(image_1x1) : 0;
         uint64_t hash_2x2 = vrs_ok ? checksum(image_2x2) : 0;
         uint32_t equal_1x1 = control_ok ? equal_neighbors(image_1x1) : 0;
         uint32_t equal_2x2 = vrs_ok ? equal_neighbors(image_2x2) : 0;
         if (vrs_ok) {
            LOG("2x2 KEEP checksum=0x%016llx equal-neighbors=%u\n",
                (unsigned long long)hash_2x2, equal_2x2);
         }
         int derivative_control_ok = control_ok &&
            derivative_signature_ok(image_1x1, (VkExtent2D){1,1});
         int derivative_2x2_ok = vrs_ok &&
            derivative_signature_ok(image_2x2, (VkExtent2D){2,2});
         int basic = control_ok && vrs_ok &&
                     hash_1x1 != hash_2x2 &&
                     equal_2x2 > equal_1x1 + W * H / 2;
         LOG("2x2 KEEP pixel-signature=%s (control=%u vrs=%u)\n",
             basic ? "PASS" : "FAIL", equal_1x1, equal_2x2);
         LOG("1x1 KEEP fragment-invocations=%u\n", invocations_1x1);
         LOG("2x2 KEEP fragment-invocations=%u\n", invocations_2x2);
         int invocation_reduction = control_ok && vrs_ok &&
                                    invocations_1x1 > 0 && invocations_2x2 > 0 &&
                                    invocations_2x2 * 2u < invocations_1x1;
         LOG("2x2 KEEP invocation-reduction=%s (1x1=%u 2x2=%u; requires >2x reduction)\n",
             invocation_reduction ? "PASS" : "FAIL",
             invocations_1x1, invocations_2x2);

          if (probe_case == CASE_KEEP) {
             passed = basic && invocation_reduction;
          } else if (probe_case == CASE_DYNAMIC_KEEP) {
             LOG("starting dynamic-state 2x2 KEEP render case\n");
             int dynamic_ok = render_rate(device, &mem_props, gdpa, queue, qf,
                                          format, rp, pl, vs, fs, keep, keep,
                                          (VkExtent2D){2,2}, true, invocation_set,
                                          invocation_buffer, invocation_mem,
                                          &invocations_target, image_target);
             LOG("2x2 dynamic KEEP submit/fence/readback=%s\n",
                 dynamic_ok ? "PASS" : "FAIL");
             if (dynamic_ok) {
                uint64_t dynamic_hash = checksum(image_target);
                uint32_t dynamic_equal = equal_neighbors(image_target);
                int dynamic_pixels_match = dynamic_hash == hash_2x2;
                int dynamic_invocations_match =
                   invocations_target == invocations_2x2;
                LOG("2x2 dynamic KEEP checksum=0x%016llx equal-neighbors=%u\n",
                    (unsigned long long)dynamic_hash, dynamic_equal);
                LOG("2x2 dynamic KEEP pixel-signature=%s\n",
                    dynamic_pixels_match ? "PASS" : "FAIL");
                LOG("2x2 dynamic KEEP fragment-invocations=%u static=%u comparison=%s\n",
                    invocations_target, invocations_2x2,
                    dynamic_invocations_match ? "PASS" : "FAIL");
                passed = basic && invocation_reduction && dynamic_pixels_match &&
                         dynamic_invocations_match &&
                         dynamic_equal > equal_1x1 + W * H / 2;
             }
          } else if (probe_case == CASE_MIP_LOD) {
             static const VkExtent2D mip_rates[]={{1,1},{2,1},{1,2},{2,2},{4,2},{2,4},{4,4}};
             static const uint8_t signatures[5][4]={{255,0,0,255},{0,255,0,255},
                {0,0,255,255},{255,255,0,255},{255,0,255,255}};
             int mip_matrix_pass=1;
             for(uint32_t r=0;r<sizeof(mip_rates)/sizeof(mip_rates[0]);r++) {
                VkExtent2D rate=mip_rates[r];
                uint32_t major=rate.width>rate.height?rate.width:rate.height;
                uint32_t expected_level=major==1?2:(major==2?3:4);
                uint8_t *static_pixels=malloc(IMAGE_BYTES);
                uint8_t *dynamic_pixels=malloc(IMAGE_BYTES);
                uint32_t static_invocations=0,dynamic_invocations=0;
                int static_ok=static_pixels&&render_rate(device,&mem_props,get_device_proc_addr,
                   queue,qf,format,rp,pl,vs,fs_mip,keep,keep,rate,false,
                   invocation_set,invocation_buffer,invocation_mem,&static_invocations,static_pixels);
                int dynamic_ok=static_ok&&dynamic_pixels&&render_rate(device,&mem_props,get_device_proc_addr,
                   queue,qf,format,rp,pl,vs,fs_mip,keep,keep,rate,true,
                   invocation_set,invocation_buffer,invocation_mem,&dynamic_invocations,dynamic_pixels);
                const uint8_t *sc=static_ok?&static_pixels[((H/2)*W+(W/2))*4]:NULL;
                const uint8_t *dc=dynamic_ok?&dynamic_pixels[((H/2)*W+(W/2))*4]:NULL;
                int static_match=sc&&!memcmp(sc,signatures[expected_level],4);
                int dynamic_match=dc&&!memcmp(dc,signatures[expected_level],4);
                int image_match=static_ok&&dynamic_ok&&!memcmp(static_pixels,dynamic_pixels,IMAGE_BYTES);
                LOG("implicit mip LOD %ux%u expected-level=%u static=%s dynamic=%s static/dynamic=%s invocations=%u/%u\\n",
                    rate.width,rate.height,expected_level,static_match?"PASS":"FAIL",
                    dynamic_match?"PASS":"FAIL",image_match?"MATCH":"MISMATCH",
                    static_invocations,dynamic_invocations);
                if(sc)LOG(" static center RGBA=%u,%u,%u,%u\\n",sc[0],sc[1],sc[2],sc[3]);
                if(dc)LOG(" dynamic center RGBA=%u,%u,%u,%u\\n",dc[0],dc[1],dc[2],dc[3]);
                mip_matrix_pass &= static_match&&dynamic_match&&image_match;
                free(static_pixels);free(dynamic_pixels);
             }
             LOG("implicit mip LOD matrix: %s\\n",mip_matrix_pass?"PASS":"FAIL");
             passed=mip_matrix_pass;
          } else if (probe_case == CASE_DYNAMIC_ISOLATION) {
             uint8_t *constant_static = malloc(IMAGE_BYTES);
             uint8_t *constant_dynamic = malloc(IMAGE_BYTES);
             uint32_t constant_static_invocations = 0;
             uint32_t constant_dynamic_invocations = 0;
             if (!constant_static || !constant_dynamic) {
                LOG("FAIL: host allocation for constant-colour isolation images\n");
             } else {
                LOG("starting constant-colour dynamic FSR isolation case\n");
                int constant_static_ok =
                   render_rate(device, &mem_props, gdpa, queue, qf, format,
                               rp, pl, vs, fs_constant, keep, keep,
                               (VkExtent2D){2,2}, false, invocation_set,
                               invocation_buffer, invocation_mem,
                               &constant_static_invocations, constant_static);
                int constant_dynamic_ok = constant_static_ok &&
                   render_rate(device, &mem_props, gdpa, queue, qf, format,
                               rp, pl, vs, fs_constant, keep, keep,
                               (VkExtent2D){2,2}, true, invocation_set,
                               invocation_buffer, invocation_mem,
                               &constant_dynamic_invocations, constant_dynamic);
                uint64_t constant_static_hash = constant_static_ok ?
                   checksum(constant_static) : 0;
                uint64_t constant_dynamic_hash = constant_dynamic_ok ?
                   checksum(constant_dynamic) : 0;
                int constant_pixels_match = constant_static_ok &&
                   constant_dynamic_ok &&
                   constant_static_hash == constant_dynamic_hash;
                int constant_invocations_match = constant_static_ok &&
                   constant_dynamic_ok &&
                   constant_static_invocations == constant_dynamic_invocations;
                LOG("constant 2x2 static submit/fence/readback=%s\n",
                    constant_static_ok ? "PASS" : "FAIL");
                LOG("constant 2x2 dynamic submit/fence/readback=%s\n",
                    constant_dynamic_ok ? "PASS" : "FAIL");
                LOG("constant static checksum=0x%016llx dynamic=0x%016llx pixel-comparison=%s\n",
                    (unsigned long long)constant_static_hash,
                    (unsigned long long)constant_dynamic_hash,
                    constant_pixels_match ? "PASS" : "FAIL");
                LOG("constant fragment-invocations static=%u dynamic=%u comparison=%s\n",
                    constant_static_invocations, constant_dynamic_invocations,
                    constant_invocations_match ? "PASS" : "FAIL");
                int dynamic_coordinate_ok =
                   render_rate(device, &mem_props, gdpa, queue, qf, format,
                               rp, pl, vs, fs, keep, keep,
                               (VkExtent2D){2,2}, true, invocation_set,
                               invocation_buffer, invocation_mem,
                               &invocations_target, image_target);
                int coordinate_invocations_match = dynamic_coordinate_ok &&
                   invocations_target == invocations_2x2;
                int coordinate_shader_differs = dynamic_coordinate_ok &&
                   checksum(image_target) != hash_2x2;
                LOG("coordinate shader dynamic render=%s; pixel mismatch=%s\n",
                    dynamic_coordinate_ok ? "PASS" : "FAIL",
                    coordinate_shader_differs ? "YES" : "NO");
                LOG("coordinate shader dynamic fragment-invocations=%u static=%u comparison=%s\n",
                    invocations_target, invocations_2x2,
                    coordinate_invocations_match ? "PASS" : "FAIL");
                passed = basic && invocation_reduction && constant_pixels_match &&
                         constant_invocations_match &&
                         constant_dynamic_invocations == invocations_2x2 &&
                         coordinate_invocations_match && coordinate_shader_differs;
             }
             free(constant_static);
             free(constant_dynamic);
          } else if (probe_case == CASE_RATE_MATRIX) {
             /*
              * Exercise the advertised pipeline-only rate matrix on real GPU
              * work: static and dynamic 2x1, 1x2 and 4x4. Each dynamic result
              * must match its static counterpart in both pixels and invocation
              * count. The 1x1 control is shared with the existing probe.
              */
             static const VkExtent2D matrix_rates[] = {
                {2, 1}, {1, 2}, {4, 2}, {2, 4}, {4, 4},
             };
             int matrix_pass = basic && invocation_reduction &&
                               derivative_control_ok && derivative_2x2_ok;
             for (uint32_t r = 0; r < (uint32_t)(sizeof(matrix_rates) / sizeof(matrix_rates[0])); r++) {
                const VkExtent2D rate = matrix_rates[r];
                const uint32_t area = rate.width * rate.height;
                uint32_t static_invocations = 0, dynamic_invocations = 0;
                LOG("rate-matrix: testing %ux%u static KEEP\\n",
                    rate.width, rate.height);
                int static_ok = render_rate(
                   device, &mem_props, gdpa, queue, qf, format, rp, pl, vs, fs,
                   keep, keep, rate, false, invocation_set, invocation_buffer,
                   invocation_mem, &static_invocations, image_target);
                uint64_t static_hash = static_ok ? checksum(image_target) : 0;
                uint32_t static_equal = static_ok ? equal_neighbors(image_target) : 0;
                uint32_t static_horizontal = 0, static_vertical = 0;
                if (static_ok)
                   equal_neighbor_axes(image_target, &static_horizontal,
                                       &static_vertical);
                uint32_t expected_invocations =
                   (invocations_1x1 + area / 2) / area;
                uint32_t expected_horizontal =
                   (W / rate.width) * H * (rate.width - 1);
                uint32_t expected_vertical =
                   W * (H / rate.height) * (rate.height - 1);
                int static_pixels_ok = static_ok && static_hash != hash_1x1 &&
                   static_equal > equal_1x1 + W * H / 4;
                int static_shape_ok = static_ok &&
                   static_horizontal == expected_horizontal &&
                   static_vertical == expected_vertical;
                int static_count_ok = static_ok &&
                   static_invocations == expected_invocations;
                int static_derivative_ok = static_ok &&
                   derivative_signature_ok(image_target, rate);
                LOG("%ux%u static: render=%s checksum=0x%016llx equal-neighbors=%u horizontal=%u/%u vertical=%u/%u shape=%s invocations=%u expected=%u pixels=%s count=%s\\n",
                    rate.width, rate.height, static_ok ? "PASS" : "FAIL",
                    (unsigned long long)static_hash, static_equal,
                    static_horizontal, expected_horizontal,
                    static_vertical, expected_vertical,
                    static_shape_ok ? "PASS" : "FAIL",
                    static_invocations, expected_invocations,
                    static_pixels_ok ? "PASS" : "FAIL",
                    static_count_ok ? "PASS" : "FAIL");

                int dynamic_ok = static_ok && render_rate(
                   device, &mem_props, gdpa, queue, qf, format, rp, pl, vs, fs,
                   keep, keep, rate, true, invocation_set, invocation_buffer,
                   invocation_mem, &dynamic_invocations, image_2x2);
                uint64_t dynamic_hash = dynamic_ok ? checksum(image_2x2) : 0;
                int dynamic_pixels_ok = dynamic_ok && dynamic_hash == static_hash;
                int dynamic_count_ok = dynamic_ok &&
                   dynamic_invocations == static_invocations;
                int dynamic_derivative_ok = dynamic_ok &&
                   derivative_signature_ok(image_2x2, rate);
                LOG("%ux%u dynamic: render=%s checksum=0x%016llx invocations=%u static=%u pixels=%s count=%s\\n",
                    rate.width, rate.height, dynamic_ok ? "PASS" : "FAIL",
                    (unsigned long long)dynamic_hash, dynamic_invocations,
                    static_invocations,
                    dynamic_pixels_ok ? "PASS" : "FAIL",
                    dynamic_count_ok ? "PASS" : "FAIL");
                LOG("%ux%u derivative signature: static=%s dynamic=%s\\n",
                    rate.width, rate.height,
                    static_derivative_ok ? "PASS" : "FAIL",
                    dynamic_derivative_ok ? "PASS" : "FAIL");

                matrix_pass &= static_pixels_ok && static_shape_ok &&
                               static_count_ok && dynamic_pixels_ok &&
                               dynamic_count_ok && static_derivative_ok &&
                               dynamic_derivative_ok;
             }
             LOG("advertised rate matrix: %s\\n",
                 matrix_pass ? "PASS" : "FAIL");
             passed = matrix_pass;
          } else if (!fsr_features.primitiveFragmentShadingRate &&
                     !fsr_features.attachmentFragmentShadingRate) {
             LOG("SKIP: neither primitive nor attachment fragment shading rate is supported; no legal non-KEEP combiner input exists\n");
             passed = basic && invocation_reduction;
          } else if (!fsr_props.fragmentShadingRateNonTrivialCombinerOps &&
                     probe_case != CASE_REPLACE) {
             LOG("SKIP: MIN/MAX/MUL require fragmentShadingRateNonTrivialCombinerOps; KEEP and REPLACE are the only permitted ops\n");
             passed = basic && invocation_reduction;
          } else if (basic) {
             VkFragmentShadingRateCombinerOpKHR op0 = keep;
             VkFragmentShadingRateCombinerOpKHR op1 = keep;
             VkFragmentShadingRateCombinerOpKHR requested_op = keep;
             const char *label = NULL;
             uint64_t expected_hash = hash_2x2;
             unsigned op_slot = fsr_features.primitiveFragmentShadingRate ? 0 : 1;
             switch (probe_case) {
             case CASE_MAX:
                requested_op = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR;
                label = "2x2 MAX/KEEP";
                expected_hash = hash_2x2;
                break;
             case CASE_MIN:
                requested_op = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR;
                label = "2x2 MIN/KEEP";
                expected_hash = hash_1x1;
                break;
             case CASE_MUL:
                requested_op = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR;
                label = "2x2 MUL/KEEP";
                expected_hash = hash_2x2;
                break;
             case CASE_REPLACE:
                requested_op = VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR;
                label = op_slot == 0 ? "2x2 REPLACE/KEEP (primitive input)" :
                                       "2x2 KEEP/REPLACE (attachment input)";
                expected_hash = hash_1x1;
                break;
             default:
                break;
             }
             if (op_slot == 0)
                op0 = requested_op;
             else
                op1 = requested_op;

             LOG("starting isolated combiner case: %s\n", label);
             LOG("Vulkan FSR requested op0=%u op1=%u slot=%u\n",
                 (unsigned)op0, (unsigned)op1, op_slot);
             int target_ok = render_rate(device, &mem_props, gdpa, queue, qf,
                                         format, rp, pl, vs, fs, op0, op1,
                                         (VkExtent2D){2,2}, false, invocation_set,
                                         invocation_buffer, invocation_mem,
                                         &invocations_target, image_target);
             LOG("%s submit/fence/readback=%s\n",
                label, target_ok ? "PASS" : "FAIL");
            if (target_ok) {
               uint64_t target_hash = checksum(image_target);
               uint32_t target_equal = equal_neighbors(image_target);
               LOG("%s checksum=0x%016llx equal-neighbors=%u\n",
                   label, (unsigned long long)target_hash, target_equal);
               uint32_t expected_invocations =
                  expected_hash == hash_2x2 ? invocations_2x2 : invocations_1x1;
               int target_pixel_ok = target_hash == expected_hash;
               int target_invocations_ok = invocations_target == expected_invocations;
               LOG("%s fragment-invocations=%u expected=%u\n",
                   label, invocations_target, expected_invocations);
               LOG("%s pixel comparison=%s expected-checksum=0x%016llx\n",
                   label, target_pixel_ok ? "PASS" : "FAIL",
                   (unsigned long long)expected_hash);
               LOG("%s invocation comparison=%s\n",
                   label, target_invocations_ok ? "PASS" : "FAIL");
               passed = invocation_reduction && target_pixel_ok && target_invocations_ok;
            }
         }
      }
      free(image_1x1);
      free(image_2x2);
      free(image_target);
      if (fs_constant) destroy_sm(device,fs_constant,NULL);
      if (fs_mip) destroy_sm(device,fs_mip,NULL);
      if (mip_sampler) { PFN_vkDestroySampler x=(PFN_vkDestroySampler)get_device_proc_addr(device,"vkDestroySampler"); if(x)x(device,mip_sampler,NULL); }
      if (mip_view) { PFN_vkDestroyImageView x=(PFN_vkDestroyImageView)get_device_proc_addr(device,"vkDestroyImageView"); if(x)x(device,mip_view,NULL); }
      if (mip_image) { PFN_vkDestroyImage x=(PFN_vkDestroyImage)get_device_proc_addr(device,"vkDestroyImage"); if(x)x(device,mip_image,NULL); }
      if (mip_image_mem) { PFN_vkFreeMemory x=(PFN_vkFreeMemory)get_device_proc_addr(device,"vkFreeMemory"); if(x)x(device,mip_image_mem,NULL); }
      destroy_sm(device,vs,NULL);destroy_sm(device,fs,NULL);
      destroy_pl(device,pl,NULL);
      destroy_rp(device,rp,NULL);
      destroy_descriptor_pool(device, invocation_pool, NULL);
      destroy_descriptor_set_layout(device, invocation_layout, NULL);
      destroy_buffer(device, invocation_buffer, NULL);
      free_memory(device, invocation_mem, NULL);
      destroy_dev(device,NULL);
   }
   free(pds);
   destroy_instance(instance,NULL);
   LOG("\n%s\n",passed?"VRS GPU RENDER PASS":"VRS GPU RENDER FAIL");
   fclose(log_file);
   return passed?0:1;
}

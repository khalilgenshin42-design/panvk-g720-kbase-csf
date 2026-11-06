/*
 * PanVK VK_KHR_fragment_shading_rate smoke probe for Winlator.
 *
 * Dynamically loads the Vulkan loader so this executable can be run inside
 * Winlator with the custom PanVK driver selected, without linking against or
 * replacing the system Vulkan loader.
 *
 * SPDX-License-Identifier: MIT
 */
#define WIN32_LEAN_AND_MEAN
#define VK_NO_PROTOTYPES
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

static FILE *log_file;

#define LOG(...) do { \
   if (log_file) { \
      fprintf(log_file, __VA_ARGS__); \
      fflush(log_file); \
   } \
} while (0)

#define LOAD_INSTANCE(instance, name) \
   ((PFN_vk##name)gipa((instance), "vk" #name))

static int
has_device_extension(PFN_vkEnumerateDeviceExtensionProperties enumerate,
                     VkPhysicalDevice physical_device, const char *name)
{
   uint32_t count = 0;
   if (enumerate(physical_device, NULL, &count, NULL) != VK_SUCCESS)
      return 0;

   VkExtensionProperties *extensions =
      calloc(count ? count : 1, sizeof(*extensions));
   if (!extensions)
      return 0;

   VkResult result =
      enumerate(physical_device, NULL, &count, extensions);
   int found = 0;
   if (result == VK_SUCCESS) {
      for (uint32_t i = 0; i < count; i++) {
         if (strcmp(extensions[i].extensionName, name) == 0) {
            found = 1;
            break;
         }
      }
   }

   free(extensions);
   return found;
}

int
main(void)
{
   log_file = fopen("panvk_vrs_probe.log", "w");
   if (!log_file)
      return 2;

   LOG("PanVK VRS probe: VK_KHR_fragment_shading_rate\n");

   HMODULE loader = LoadLibraryA("vulkan-1.dll");
   if (!loader) {
      LOG("FAIL: could not load vulkan-1.dll (Win32 error %lu)\n",
          (unsigned long)GetLastError());
      fclose(log_file);
      return 1;
   }

   PFN_vkGetInstanceProcAddr gipa =
      (PFN_vkGetInstanceProcAddr)GetProcAddress(loader,
                                                "vkGetInstanceProcAddr");
   if (!gipa) {
      LOG("FAIL: vkGetInstanceProcAddr missing from Vulkan loader\n");
      fclose(log_file);
      return 1;
   }

   PFN_vkCreateInstance create_instance =
      (PFN_vkCreateInstance)gipa(VK_NULL_HANDLE, "vkCreateInstance");
   if (!create_instance) {
      LOG("FAIL: vkCreateInstance entrypoint unavailable\n");
      fclose(log_file);
      return 1;
   }

   VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "PanVK VRS probe",
      .applicationVersion = 1,
      .pEngineName = "PanVK VRS probe",
      .engineVersion = 1,
      .apiVersion = VK_API_VERSION_1_3,
   };

   VkInstanceCreateInfo instance_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &app_info,
   };

   VkInstance instance = VK_NULL_HANDLE;
   VkResult result = create_instance(&instance_info, NULL, &instance);
   if (result != VK_SUCCESS) {
      LOG("FAIL: vkCreateInstance returned %d\n", result);
      fclose(log_file);
      return 1;
   }

   PFN_vkEnumeratePhysicalDevices enumerate_physical_devices =
      LOAD_INSTANCE(instance, EnumeratePhysicalDevices);
   PFN_vkEnumerateDeviceExtensionProperties enumerate_device_extensions =
      LOAD_INSTANCE(instance, EnumerateDeviceExtensionProperties);
   PFN_vkGetPhysicalDeviceFeatures2 get_features2 =
      LOAD_INSTANCE(instance, GetPhysicalDeviceFeatures2);
   PFN_vkGetPhysicalDeviceProperties2 get_properties2 =
      LOAD_INSTANCE(instance, GetPhysicalDeviceProperties2);
   PFN_vkGetPhysicalDeviceQueueFamilyProperties get_queue_properties =
      LOAD_INSTANCE(instance, GetPhysicalDeviceQueueFamilyProperties);
   PFN_vkCreateDevice create_device = LOAD_INSTANCE(instance, CreateDevice);
   PFN_vkDestroyInstance destroy_instance =
      LOAD_INSTANCE(instance, DestroyInstance);
   PFN_vkGetDeviceProcAddr get_device_proc_addr =
      LOAD_INSTANCE(instance, GetDeviceProcAddr);
   PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR get_shading_rates =
      (PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR)
         gipa(instance, "vkGetPhysicalDeviceFragmentShadingRatesKHR");

   if (!enumerate_physical_devices || !enumerate_device_extensions ||
       !get_features2 || !get_properties2 || !get_queue_properties ||
       !create_device || !destroy_instance || !get_device_proc_addr) {
      LOG("FAIL: required Vulkan core entrypoint unavailable\n");
      if (destroy_instance)
         destroy_instance(instance, NULL);
      fclose(log_file);
      return 1;
   }

   uint32_t physical_device_count = 0;
   result = enumerate_physical_devices(instance, &physical_device_count, NULL);
   if (result != VK_SUCCESS || physical_device_count == 0) {
      LOG("FAIL: no Vulkan physical devices (result %d)\n", result);
      destroy_instance(instance, NULL);
      fclose(log_file);
      return 1;
   }

   VkPhysicalDevice *physical_devices =
      calloc(physical_device_count, sizeof(*physical_devices));
   if (!physical_devices) {
      LOG("FAIL: out of host memory enumerating devices\n");
      destroy_instance(instance, NULL);
      fclose(log_file);
      return 1;
   }

   result = enumerate_physical_devices(instance, &physical_device_count,
                                        physical_devices);
   if (result != VK_SUCCESS) {
      LOG("FAIL: physical-device enumeration returned %d\n", result);
      free(physical_devices);
      destroy_instance(instance, NULL);
      fclose(log_file);
      return 1;
   }

   int passed = 0;
   for (uint32_t d = 0; d < physical_device_count; d++) {
      VkPhysicalDevice physical_device = physical_devices[d];
      VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsr_features = {
         .sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
      };
      VkPhysicalDeviceFeatures2 features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &fsr_features,
      };
      VkPhysicalDeviceFragmentShadingRatePropertiesKHR fsr_properties = {
         .sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR,
      };
      VkPhysicalDeviceDriverProperties driver_properties = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
      };
      VkPhysicalDeviceProperties2 properties = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
         .pNext = &driver_properties,
      };

      get_properties2(physical_device, &properties);
      LOG("\nDevice %u: %s\n", d, properties.properties.deviceName);
      LOG("  API version: %u.%u.%u\n",
          VK_VERSION_MAJOR(properties.properties.apiVersion),
          VK_VERSION_MINOR(properties.properties.apiVersion),
          VK_VERSION_PATCH(properties.properties.apiVersion));
      LOG("  vendorID/deviceID: 0x%04x/0x%04x\n",
          properties.properties.vendorID, properties.properties.deviceID);
      LOG("  driverVersion: 0x%08x\n", properties.properties.driverVersion);
      LOG("  Driver: %s (%s)\n", driver_properties.driverName,
          driver_properties.driverInfo);

      uint32_t ext_count = 0;
      VkResult ext_result =
         enumerate_device_extensions(physical_device, NULL, &ext_count, NULL);
      LOG("  device extension enumeration: result=%d count=%u\n",
          ext_result, ext_count);
      if (ext_result == VK_SUCCESS && ext_count) {
         VkExtensionProperties *all_exts =
            calloc(ext_count, sizeof(*all_exts));
         if (all_exts) {
            uint32_t returned = ext_count;
            ext_result = enumerate_device_extensions(physical_device, NULL,
                                                      &returned, all_exts);
            if (ext_result == VK_SUCCESS) {
               for (uint32_t e = 0; e < returned; e++)
                  LOG("    %s (spec %u)\n", all_exts[e].extensionName,
                      all_exts[e].specVersion);
            } else {
               LOG("  full extension enumeration failed: %d\n", ext_result);
            }
            free(all_exts);
         } else {
            LOG("  full extension list allocation failed\n");
         }
      }

      int extension_present =
         has_device_extension(enumerate_device_extensions, physical_device,
                              VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
      LOG("  VK_KHR_fragment_shading_rate: %s\n",
          extension_present ? "present" : "missing");

      if (!extension_present) {
         continue;
      }

      driver_properties.pNext = &fsr_properties;
      get_properties2(physical_device, &properties);

      if (!get_shading_rates) {
         LOG("  FAIL: vkGetPhysicalDeviceFragmentShadingRatesKHR unavailable\n");
         continue;
      }

      get_features2(physical_device, &features);
      LOG("  pipelineFragmentShadingRate: %s\n",
          fsr_features.pipelineFragmentShadingRate ? "true" : "false");
      LOG("  primitiveFragmentShadingRate: %s\n",
          fsr_features.primitiveFragmentShadingRate ? "true" : "false");
      LOG("  attachmentFragmentShadingRate: %s\n",
          fsr_features.attachmentFragmentShadingRate ? "true" : "false");
      LOG("  maxFragmentSize: %ux%u, aspect ratio %u\n",
          fsr_properties.maxFragmentSize.width,
          fsr_properties.maxFragmentSize.height,
          fsr_properties.maxFragmentSizeAspectRatio);
      LOG("  max coverage samples: %u, rasterization samples: 0x%x\n",
          fsr_properties.maxFragmentShadingRateCoverageSamples,
          fsr_properties.maxFragmentShadingRateRasterizationSamples);

      uint32_t rate_count = 0;
      result = get_shading_rates(physical_device, &rate_count, NULL);
      if (result != VK_SUCCESS || rate_count == 0) {
         LOG("  FAIL: rate count query returned %d, count %u\n",
             result, rate_count);
         continue;
      }

      VkPhysicalDeviceFragmentShadingRateKHR *rates =
         calloc(rate_count, sizeof(*rates));
      if (!rates) {
         LOG("  FAIL: out of host memory allocating rate list\n");
         continue;
      }

      for (uint32_t i = 0; i < rate_count; i++) {
         rates[i].sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_KHR;
      }

      result = get_shading_rates(physical_device, &rate_count, rates);
      if (result != VK_SUCCESS) {
         LOG("  FAIL: rate list query returned %d\n", result);
         free(rates);
         continue;
      }

      LOG("  Supported rates:\n");
      for (uint32_t i = 0; i < rate_count; i++) {
         LOG("    %ux%u, sampleCounts=0x%x\n",
             rates[i].fragmentSize.width, rates[i].fragmentSize.height,
             rates[i].sampleCounts);
      }
      free(rates);

      if (!fsr_features.pipelineFragmentShadingRate) {
         LOG("  FAIL: pipeline FSR feature is not supported\n");
         continue;
      }

      uint32_t queue_count = 0;
      get_queue_properties(physical_device, &queue_count, NULL);
      VkQueueFamilyProperties *queues =
         calloc(queue_count ? queue_count : 1, sizeof(*queues));
      if (!queues) {
         LOG("  FAIL: out of host memory allocating queue list\n");
         continue;
      }
      get_queue_properties(physical_device, &queue_count, queues);

      uint32_t graphics_queue = UINT32_MAX;
      for (uint32_t q = 0; q < queue_count; q++) {
         if (queues[q].queueCount &&
             (queues[q].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            graphics_queue = q;
            break;
         }
      }
      free(queues);

      if (graphics_queue == UINT32_MAX) {
         LOG("  FAIL: no graphics queue family\n");
         continue;
      }

      float priority = 1.0f;
      VkDeviceQueueCreateInfo queue_info = {
         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
         .queueFamilyIndex = graphics_queue,
         .queueCount = 1,
         .pQueuePriorities = &priority,
      };
      VkPhysicalDeviceFragmentShadingRateFeaturesKHR enable_fsr = {
         .sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR,
         .pipelineFragmentShadingRate = VK_TRUE,
      };
      VkPhysicalDeviceFeatures2 enable_features = {
         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
         .pNext = &enable_fsr,
      };
      const char *enabled_extensions[] = {
         VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
      };
      VkDeviceCreateInfo device_info = {
         .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
         .pNext = &enable_features,
         .queueCreateInfoCount = 1,
         .pQueueCreateInfos = &queue_info,
         .enabledExtensionCount = 1,
         .ppEnabledExtensionNames = enabled_extensions,
      };

      VkDevice device = VK_NULL_HANDLE;
      result = create_device(physical_device, &device_info, NULL, &device);
      LOG("  vkCreateDevice with pipelineFragmentShadingRate: %d\n", result);
      if (result == VK_SUCCESS) {
         PFN_vkDestroyDevice destroy_device =
            (PFN_vkDestroyDevice)get_device_proc_addr(device, "vkDestroyDevice");
         PFN_vkCreateCommandPool create_command_pool =
            (PFN_vkCreateCommandPool)get_device_proc_addr(device,
                                                           "vkCreateCommandPool");
         PFN_vkAllocateCommandBuffers allocate_command_buffers =
            (PFN_vkAllocateCommandBuffers)get_device_proc_addr(
               device, "vkAllocateCommandBuffers");
         PFN_vkBeginCommandBuffer begin_command_buffer =
            (PFN_vkBeginCommandBuffer)get_device_proc_addr(
               device, "vkBeginCommandBuffer");
         PFN_vkEndCommandBuffer end_command_buffer =
            (PFN_vkEndCommandBuffer)get_device_proc_addr(
               device, "vkEndCommandBuffer");
         PFN_vkDestroyCommandPool destroy_command_pool =
            (PFN_vkDestroyCommandPool)get_device_proc_addr(
               device, "vkDestroyCommandPool");
         PFN_vkCmdSetFragmentShadingRateKHR cmd_set_shading_rate =
            (PFN_vkCmdSetFragmentShadingRateKHR)get_device_proc_addr(
               device, "vkCmdSetFragmentShadingRateKHR");

         if (create_command_pool && allocate_command_buffers &&
             begin_command_buffer && end_command_buffer &&
             destroy_command_pool && cmd_set_shading_rate) {
            VkCommandPoolCreateInfo pool_info = {
               .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
               .queueFamilyIndex = graphics_queue,
            };
            VkCommandPool pool = VK_NULL_HANDLE;
            VkResult pool_result =
               create_command_pool(device, &pool_info, NULL, &pool);

            if (pool_result == VK_SUCCESS) {
               VkCommandBufferAllocateInfo alloc_info = {
                  .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                  .commandPool = pool,
                  .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                  .commandBufferCount = 1,
               };
               VkCommandBuffer command_buffer = VK_NULL_HANDLE;
               VkResult alloc_result =
                  allocate_command_buffers(device, &alloc_info,
                                            &command_buffer);
               if (alloc_result == VK_SUCCESS) {
                  VkCommandBufferBeginInfo begin_info = {
                     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                  };
                  VkResult begin_result =
                     begin_command_buffer(command_buffer, &begin_info);
                  if (begin_result == VK_SUCCESS) {
                     VkExtent2D fragment_size = {2, 2};
                     VkFragmentShadingRateCombinerOpKHR combiners[2] = {
                        VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR,
                        VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR,
                     };
                     cmd_set_shading_rate(command_buffer, &fragment_size,
                                          combiners);
                     VkResult end_result =
                        end_command_buffer(command_buffer);
                     LOG("  vkCmdSetFragmentShadingRateKHR record/end: %d\n",
                         end_result);
                     if (end_result == VK_SUCCESS) {
                        passed = 1;
                        LOG("  PASS: extension query, device creation, and "
                            "dynamic FSR command recording\n");
                     }
                  } else {
                     LOG("  FAIL: vkBeginCommandBuffer returned %d\n",
                         begin_result);
                  }
               } else {
                  LOG("  FAIL: vkAllocateCommandBuffers returned %d\n",
                      alloc_result);
               }
               destroy_command_pool(device, pool, NULL);
            } else {
               LOG("  FAIL: vkCreateCommandPool returned %d\n",
                   pool_result);
            }
         } else {
            LOG("  FAIL: FSR command or command-buffer entrypoint missing\n");
         }

         if (destroy_device)
            destroy_device(device, NULL);
         if (passed)
            break;
      }
   }

   free(physical_devices);
   destroy_instance(instance, NULL);
   LOG("\n%s\n", passed ? "VRS PROBE PASS" : "VRS PROBE FAIL");
   fclose(log_file);
   return passed ? 0 : 1;
}

#!/usr/bin/env python3
"""Apply PanVK-specific capability and BC image-limit fixes to bionic-vulkan-wrapper.

The upstream wrapper deliberately synthesizes Vulkan capabilities for several
vendor drivers. PanVK already reports its real capabilities; these compatibility
overrides must not be applied to it.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(path: Path, old: str, new: str, label: str) -> None:
    source = path.read_text()
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected one match, found {count} in {path}")
    path.write_text(source.replace(old, new, 1))


def replace_braced_block(path: Path, marker: str, replacement: str, label: str) -> None:
    source = path.read_text()
    if source.count(marker) != 1:
        raise SystemExit(f"{label}: expected one marker, found {source.count(marker)}")
    start = source.index(marker)
    brace = source.index("{", start)
    depth = 0
    end = None
    for i in range(brace, len(source)):
        if source[i] == "{":
            depth += 1
        elif source[i] == "}":
            depth -= 1
            if depth == 0:
                end = i + 1
                break
    if end is None:
        raise SystemExit(f"{label}: unmatched braces")
    path.write_text(source[:start] + replacement + source[end:])


objects = ROOT / "src/vulkan/wrapper/wrapper_objects.h"
physical = ROOT / "src/vulkan/wrapper/wrapper_physical_device.c"
device = ROOT / "src/vulkan/wrapper/wrapper_device.c"

replace_once(
    objects,
    "   VkPhysicalDeviceDriverProperties driver_properties;\n",
    "   VkPhysicalDeviceDriverProperties driver_properties;\n"
    "   bool is_panvk;\n",
    "add PanVK identity",
)

replace_once(
    physical,
    "   // Needed by dxvk\n"
    "   exts->EXT_transform_feedback = true;\n"
    "   exts->EXT_host_query_reset = true;\n"
    "   exts->EXT_custom_border_color = true;\n",
    "   // Compatibility aliases are not synthesized for PanVK: its extension\n"
    "   // table above is authoritative and already exposes implemented support.\n"
    "   if (!pdevice->is_panvk) {\n"
    "      exts->EXT_transform_feedback = true;\n"
    "      exts->EXT_host_query_reset = true;\n"
    "      exts->EXT_custom_border_color = true;\n"
    "   }\n",
    "gate compatibility extension aliases",
)

replace_once(
    physical,
    "      vk_physical_device_dispatch_table_load(&pdevice->dispatch_table,\n"
    "                                             get_instance_proc_addr,\n"
    "                                             instance->dispatch_handle);\n\n"
    "      wrapper_setup_device_extensions(pdevice);",
    "      vk_physical_device_dispatch_table_load(&pdevice->dispatch_table,\n"
    "                                             get_instance_proc_addr,\n"
    "                                             instance->dispatch_handle);\n\n"
    "      /* Identify the underlying ICD before applying wrapper compatibility\n"
    "       * overrides. Query the underlying dispatch table directly to avoid\n"
    "       * seeing any wrapper-synthesized properties. */\n"
    "      pdevice->driver_properties = (VkPhysicalDeviceDriverProperties) {\n"
    "         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,\n"
    "      };\n"
    "      pdevice->properties2 = (VkPhysicalDeviceProperties2) {\n"
    "         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,\n"
    "         .pNext = &pdevice->driver_properties,\n"
    "      };\n"
    "      pdevice->dispatch_table.GetPhysicalDeviceProperties2(\n"
    "         pdevice->dispatch_handle, &pdevice->properties2);\n"
    "      pdevice->is_panvk =\n"
    "         pdevice->driver_properties.driverID == VK_DRIVER_ID_MESA_PANVK;\n\n"
    "      wrapper_setup_device_extensions(pdevice);",
    "detect PanVK before feature setup",
)

old_feature_overrides = """      supported_features->geometryShader = true;
      supported_features->presentId = true;
      supported_features->multiViewport = true;
      supported_features->depthClamp = true;
      supported_features->depthBiasClamp = true;
      supported_features->memoryMapPlaced = true;
      supported_features->memoryUnmapReserve = true;
      supported_features->textureCompressionBC = true;
      supported_features->fillModeNonSolid = true;
      supported_features->shaderClipDistance = true;
      supported_features->shaderCullDistance = true;
      supported_features->presentWait = supported_features->timelineSemaphore;
      supported_features->swapchainMaintenance1 = true;
      supported_features->imageCompressionControlSwapchain = false;

      // dxvk extension features support
      supported_features->geometryStreams = true;
      supported_features->transformFeedback = true;
      supported_features->hostQueryReset = true;
      supported_features->customBorderColors = true;
      supported_features->customBorderColorWithoutFormat = true;
      supported_features->dualSrcBlend = true; // Missing on G715 r38p1
      // supported_features->logicOp = true; // Missing on G57 r32p1
      supported_features->multiDrawIndirect = true; // Missing on G57 r32p1
      supported_features->vertexPipelineStoresAndAtomics = true; // Missing on G57 r32p1
      // supported_features->variableMultisampleRate = true; // Missing on G57 r32p1
"""
new_feature_overrides = """      /* These feature aliases are compatibility hacks for other vendor
       * drivers. PanVK already reports its real core and extension features;
       * do not claim viewport, clip/cull-distance, geometry-stream, or
       * vertex-stage atomics support that its backend does not implement. */
      if (!pdevice->is_panvk) {
         supported_features->geometryShader = true;
         supported_features->presentId = true;
         supported_features->multiViewport = true;
         supported_features->depthClamp = true;
         supported_features->depthBiasClamp = true;
         supported_features->memoryMapPlaced = true;
         supported_features->memoryUnmapReserve = true;
         supported_features->textureCompressionBC = true;
         supported_features->fillModeNonSolid = true;
         supported_features->shaderClipDistance = true;
         supported_features->shaderCullDistance = true;
         supported_features->presentWait = supported_features->timelineSemaphore;
         supported_features->swapchainMaintenance1 = true;
         supported_features->geometryStreams = true;
         supported_features->transformFeedback = true;
         supported_features->hostQueryReset = true;
         supported_features->customBorderColors = true;
         supported_features->customBorderColorWithoutFormat = true;
         supported_features->dualSrcBlend = true;
         supported_features->multiDrawIndirect = true;
         supported_features->vertexPipelineStoresAndAtomics = true;
      }
      supported_features->imageCompressionControlSwapchain = false;
"""
replace_once(physical, old_feature_overrides, new_feature_overrides,
             "gate synthetic physical-device features")

replace_once(
    physical,
    "      if (CHECK_FLAG(\"NO_BC123_EMULATION\")) {\n"
    "         WLOG(\"Disabling BC123 emulation (disable by setting NO_BC123_EMULATION=0)\");\n"
    "         pdevice->needs_bc1_emulation = false;\n"
    "      }\n\n"
    "      list_addtail(&pdevice->vk.link, &_instance->physical_devices.list);",
    "      if (CHECK_FLAG(\"NO_BC123_EMULATION\")) {\n"
    "         WLOG(\"Disabling BC123 emulation (disable by setting NO_BC123_EMULATION=0)\");\n"
    "         pdevice->needs_bc1_emulation = false;\n"
    "      }\n\n"
    "      /* Advertise BC compression on PanVK only when the underlying ICD\n"
    "       * supports it natively or the wrapper has both BC1-3 and BC4-7\n"
    "       * emulation paths enabled. The wrapped ICD itself must not be asked\n"
    "       * to enable a feature it does not implement. */\n"
    "      if (pdevice->is_panvk) {\n"
    "         supported_features->textureCompressionBC =\n"
    "            pdevice->base_supported_features.textureCompressionBC ||\n"
    "            (pdevice->needs_bc1_emulation && pdevice->needs_bc4_emulation);\n"
    "      }\n\n"
    "      list_addtail(&pdevice->vk.link, &_instance->physical_devices.list);",
    "gate BC feature on complete emulation",
)

replace_once(
    physical,
    "      pdevice->driver_properties = (VkPhysicalDeviceDriverProperties) {\n"
    "         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,\n"
    "      };\n"
    "      pdevice->properties2 = (VkPhysicalDeviceProperties2) {\n"
    "         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,\n"
    "         .pNext = &pdevice->driver_properties,\n"
    "      };\n\n"
    "      WPDEVICE.GetPhysicalDeviceProperties2(\n"
    "         (VkPhysicalDevice) pdevice, &pdevice->properties2);",
    "      WPDEVICE.GetPhysicalDeviceProperties2(\n"
    "         (VkPhysicalDevice) pdevice, &pdevice->properties2);",
    "remove duplicate driver property initialization",
)

replace_once(
    physical,
    "WRAPPER_GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,\n"
    "                                   VkPhysicalDeviceFeatures2* pFeatures) {                                                              \n"
    "   vk_common_GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);\n"
    "   // Fake select dxvk 1.10.3 mandatory features\n",
    "WRAPPER_GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,\n"
    "                                   VkPhysicalDeviceFeatures2* pFeatures) {\n"
    "   VK_FROM_HANDLE(wrapper_physical_device, pdevice, physicalDevice);\n"
    "   vk_common_GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);\n"
    "   if (pdevice->is_panvk)\n"
    "      return;\n"
    "   // Fake select dxvk 1.10.3 mandatory features\n",
    "stop feature2 spoofing for PanVK",
)

replace_once(
    physical,
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT:\n"
    "      {\n"
    "      \t VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *texel_prop =\n"
    "      \t      (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)prop;\n"
    "      \t texel_prop->storageTexelBufferOffsetAlignmentBytes = 1;\n"
    "      \t texel_prop->uniformTexelBufferOffsetAlignmentBytes = 1;\n"
    "      \t break;\n"
    "      }",
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_PROPERTIES_EXT:\n"
    "      {\n"
    "         if (!pdevice->is_panvk) {\n"
    "            VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *texel_prop =\n"
    "               (VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT *)prop;\n"
    "            texel_prop->storageTexelBufferOffsetAlignmentBytes = 1;\n"
    "            texel_prop->uniformTexelBufferOffsetAlignmentBytes = 1;\n"
    "         }\n"
    "         break;\n"
    "      }",
    "preserve PanVK texel alignment properties",
)

replace_once(
    physical,
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT:\n"
    "      {\n"
    "         VkPhysicalDeviceTransformFeedbackPropertiesEXT *feedback_prop = ",
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT:\n"
    "      {\n"
    "         if (pdevice->is_panvk)\n"
    "            break;\n"
    "         VkPhysicalDeviceTransformFeedbackPropertiesEXT *feedback_prop = ",
    "preserve PanVK transform-feedback properties",
)

replace_once(
    physical,
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES:\n"
    "      {\n"
    "         VkPhysicalDeviceVulkan13Properties *vk13_prop =\n"
    "              (VkPhysicalDeviceVulkan13Properties *)prop;\n"
    "         vk13_prop->storageTexelBufferOffsetAlignmentBytes = 1;\n"
    "         vk13_prop->uniformTexelBufferOffsetAlignmentBytes = 1;\n"
    "         break;\n"
    "      }",
    "      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES:\n"
    "      {\n"
    "         if (!pdevice->is_panvk) {\n"
    "            VkPhysicalDeviceVulkan13Properties *vk13_prop =\n"
    "               (VkPhysicalDeviceVulkan13Properties *)prop;\n"
    "            vk13_prop->storageTexelBufferOffsetAlignmentBytes = 1;\n"
    "            vk13_prop->uniformTexelBufferOffsetAlignmentBytes = 1;\n"
    "         }\n"
    "         break;\n"
    "      }",
    "preserve PanVK Vulkan 1.3 texel alignment properties",
)

replace_braced_block(
    physical,
    "if (result == VK_ERROR_FORMAT_NOT_SUPPORTED && is_bc_image_format(format)) {",
    """if (result == VK_ERROR_FORMAT_NOT_SUPPORTED && is_bc_image_format(format)) {
      VkFormat emulated_format = unwrap_vk_format_physical_device(pdevice, format);
      VkImageUsageFlags emulated_usage =
         usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
      VkImageCreateFlags emulated_flags =
         (flags & ~VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT) |
         VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

      /* Query the actual replacement image, not a fabricated 512-TiB BC
       * resource. This makes maxExtent/maxResourceSize match the RGBA or
       * FP16 image that CreateImage will really allocate. */
      return pdevice->dispatch_table.GetPhysicalDeviceImageFormatProperties(
         pdevice->dispatch_handle, emulated_format, type, tiling,
         emulated_usage, emulated_flags, pImageFormatProperties);
   }""",
    "replace BC image format properties fallback",
)

replace_braced_block(
    physical,
    "if (result == VK_ERROR_FORMAT_NOT_SUPPORTED && is_bc_image_format(pImageFormatInfo->format)) {",
    """if (result == VK_ERROR_FORMAT_NOT_SUPPORTED && is_bc_image_format(pImageFormatInfo->format)) {
      VkFormat emulated_format =
         unwrap_vk_format_physical_device(pdevice, pImageFormatInfo->format);
      VkPhysicalDeviceImageFormatInfo2 emulated_info = *pImageFormatInfo;
      VkImageUsageFlags emulated_usage =
         pImageFormatInfo->usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
      VkImageCreateFlags emulated_flags =
         (pImageFormatInfo->flags & ~VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT) |
         VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
      VkImageFormatListCreateInfo emulated_format_list = {
         .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO,
         .viewFormatCount = 1,
         .pViewFormats = &emulated_format,
      };

      /* Only rewrite a leading format-list structure. If another pNext
       * structure (for example external-memory requirements) is present,
       * preserve the original unsupported result rather than invent support. */
      if (pImageFormatInfo->pNext) {
         const VkBaseInStructure *first =
            (const VkBaseInStructure *)pImageFormatInfo->pNext;
         if (first->sType != VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO)
            return result;
         const VkImageFormatListCreateInfo *source_list =
            (const VkImageFormatListCreateInfo *)first;
         emulated_format_list.pNext = source_list->pNext;
         emulated_info.pNext = &emulated_format_list;
      }

      emulated_info.format = emulated_format;
      emulated_info.usage = emulated_usage;
      emulated_info.flags = emulated_flags;

      return pdevice->dispatch_table.GetPhysicalDeviceImageFormatProperties2(
         pdevice->dispatch_handle, &emulated_info, pImageFormatProperties);
   }""",
    "replace BC image format properties2 fallback",
)

replace_once(
    device,
    "   bool needs_clip_distance_emulation = !wdev->physical->base_supported_features.shaderClipDistance\n"
    "                                        && !CHECK_FLAG(\"DISABLE_CLIP_DISTANCE\");",
    "   /* PanVK's wrapper-side clip-distance pass is currently a no-op. Do not\n"
    "    * run an empty SPIR-V optimizer on every shader module or imply that it\n"
    "    * emulates a feature the underlying ICD does not support. */\n"
    "   bool needs_clip_distance_emulation = !wdev->physical->is_panvk &&\n"
    "      !wdev->physical->base_supported_features.shaderClipDistance &&\n"
    "      !CHECK_FLAG(\"DISABLE_CLIP_DISTANCE\");",
    "disable no-op clip emulation on PanVK",
)

print("Applied PanVK-specific wrapper capability and BC allocation fixes.")

#version 450
layout(set = 0, binding = 0, std430) buffer InvocationCounter {
    uint invocations;
} counter;

layout(set = 0, binding = 1) uniform sampler2D mip_texture;
layout(location = 0) in vec2 test_uv;
layout(location = 0) out vec4 out_color;

void main()
{
    atomicAdd(counter.invocations, 1u);

    /*
     * Deliberately use texture(), not textureLod(): the selected mip level
     * must come from implicit derivatives under the active VRS rate.
     * Every mip level is uploaded as a unique constant RGBA signature.
     */
    out_color = texture(mip_texture, test_uv);
}

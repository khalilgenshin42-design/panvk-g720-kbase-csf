#version 450
layout(set = 0, binding = 0, std430) buffer InvocationCounter {
    uint invocations;
} counter;

layout(location = 0) in vec2 test_uv;
layout(location = 0) out vec4 out_color;

void main()
{
    atomicAdd(counter.invocations, 1u);
    ivec2 p = ivec2(floor(gl_FragCoord.xy));

    /*
     * Vulkan derivatives are measured between fragment invocations. Under
     * coarse VRS, dFdx/dFdy therefore scale with the fragment footprint.
     * Encode normalized footprint width/height in B/A for host validation.
     */
    float dx = dFdx(test_uv.x) * 64.0 / 4.0;
    float dy = dFdy(test_uv.y) * 64.0 / 4.0;
    out_color = vec4(float(p.x & 7) / 7.0,
                     float(p.y & 7) / 7.0,
                     clamp(dx, 0.0, 1.0),
                     clamp(dy, 0.0, 1.0));
}

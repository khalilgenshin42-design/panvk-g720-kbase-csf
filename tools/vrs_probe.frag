#version 450
layout(location = 0) out vec4 out_color;
void main()
{
    ivec2 p = ivec2(floor(gl_FragCoord.xy));
    out_color = vec4(float(p.x & 7) / 7.0,
                     float(p.y & 7) / 7.0,
                     0.25, 1.0);
}

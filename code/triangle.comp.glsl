#version 450

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(rgba8, binding = 0) uniform writeonly image2D outputImage;

layout(push_constant) uniform PushConstants {
    float time;
    uint width;
    uint height;
} pc;

void main() {
    // Get the global invocation ID (pixel coordinates)
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    // Ensure we don't write outside the image bounds
    if (coord.x >= int(pc.width) || coord.y >= int(pc.height)) {
        return;
    }
    
    // Normalize coordinates to [0, 1]
    vec2 uv = vec2(coord) / vec2(pc.width, pc.height);
    
    // Create a grid-like pattern with sine waves
    float waveX = sin(uv.x * 20.0 + pc.time * 2.0);
    float waveY = sin(uv.y * 20.0 + pc.time * 3.0);
    
    // Combine waves to create a pulsing effect
    float pulse = sin(waveX + waveY + pc.time) * 0.5 + 0.5; // Range [0, 1]
    
    // Generate RGB colors based on UV coordinates and pulse
    vec3 color = vec3(
        sin(uv.x * 3.14 + pc.time) * 0.5 + 0.5, // Red oscillates
        cos(uv.y * 3.14 + pc.time * 0.7) * 0.5 + 0.5, // Green oscillates
        pulse // Blue uses the pulsing wave
    );
    
    // Write the color to the storage image
    imageStore(outputImage, coord, vec4(color, 1.0));
}
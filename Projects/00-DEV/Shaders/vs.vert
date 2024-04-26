#version 460 core
/**
*   Object Transformation Vertex Shader
*   Draws a mesh based on input Position and RGB Colour
*   Transformed by push_constant World matrix, and a uniform
*   buffer ViewProjection matrix. 
*   ------------------
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/26
*/

layout(push_constant) uniform PushConstants
{
    mat4 w;
} pushConstants; 

layout(binding = 0) uniform UniformBuffer{
    mat4 vp;
} ubo; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 fragColour; 

void main() {
    mat4 wvp = ubo.vp * pushConstants.w;
    gl_Position = wvp * vec4(inPosition, 1.0);
    fragColour = inColour;
}
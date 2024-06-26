#version 460 core
/**
*   Basic Vertex Shader
*   Draws a mesh at the input Position and RGB Colour.
*   No transformation occurs, so input positions must be
*   given in Normalized Device Coordinates.
*   ------------------
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/25
*/

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 fragColour; 

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColour = inColour;
}
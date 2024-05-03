#version 460 core
/**
*   Basic Fragment Shader
*   Outputs the input RGB fragment colour from previous stages.
*   ------------------
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/26
*/

layout(location = 0) in vec3 fragColour;
layout(location = 0) out vec4 outColour;

void main() {
    outColour = vec4(fragColour, 1.0); 
}
#version 460 core
/**
*   Triangle Vertex Shader
*   Draws a static triangle based on input Vertex Index.
*   As such, 3 vertices must be drawn to display the correct output.
*   ------------------
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/25
*/

layout(location = 0) out vec3 fragColour; 

//Constant Triangle Vertices
const vec2 positions[3] = vec2[3] (
    vec2(-0.6, 0.4),
    vec2(0.0, -0.6),
    vec2(0.6, 0.4)
);

//RGB colours
const vec3 colours[3] = vec3[3](
    vec3(1.0, 0.0, 0.0), 
    vec3(0.0, 1.0, 0.0), 
    vec3(0.0, 0.0, 1.0) 
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColour = colours[gl_VertexIndex];
}
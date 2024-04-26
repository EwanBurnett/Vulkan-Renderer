#version 460 core
/**
*   ------------------*
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/26
*/

layout (location=0) out vec2 outUV; 

layout(push_constant) uniform PushConstants
{
    mat4 w;
} pushConstants; 

layout(binding = 0) uniform UniformBuffer{
    mat4 vp;
} ubo; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

float gridSize = 100000.0; 
float gridCellSize = 0.025; 
vec4 gridColourThin = vec4(0.5, 0.5, 0.5, 1.0);
vec4 gridColourThick = vec4(1.0, 1.0, 1.0, 1.0); 
const float gridMinPixelsBetweenCells = 2.0; 

const vec3 gridRect[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0), 
    vec3(1.0, 0.0, -1.0), 
    vec3(1.0, 0.0, 1.0), 
    vec3(-1.0, 0.0, 1.0) 
);

const int gridIndices[6] = int[6](
    0, 1, 2, 
    2, 3, 0
);

void main(){
    vec3 vertexPos = gridRect[gridIndices[gl_VertexIndex]] * gridSize; 
    gl_Position = ubo.vp * vec4(vertexPos, 1.0); 
    outUV = vertexPos.xz; 
}

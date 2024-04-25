#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColour; 

//Triangle Verts
const vec2 positions[3] = vec2[3] (
    vec2(-0.6, 0.4),
    vec2(0.0, -0.6),
    vec2(0.6, 0.4)
);

const vec3 colours[3] = vec3[3](
    vec3(1.0, 0.0, 0.0), 
    vec3(0.0, 1.0, 0.0), 
    vec3(0.0, 0.0, 1.0) 
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColour = colours[gl_VertexIndex];
}
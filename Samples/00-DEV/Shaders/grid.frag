#version 460 core
/**
*   ------------------*
*   Ewan Burnett (EwanBurnettSK@Outlook.com)
*   2024/04/26
*/

layout (location=0) in vec2 inUV; 
layout(location = 0) out vec4 outColour;

float gridSize = 100000.0; 
float gridCellSize = 0.025; 
vec4 gridColourThin = vec4(0.5, 0.5, 0.5, 1.0);
vec4 gridColourThick = vec4(1.0, 1.0, 1.0, 1.0); 
const float gridMinPixelsBetweenCells = 2.0; 


float log10(float x) {
    return log(x) / log(10.0); 
}

float satf(float x) {
    return clamp(x, 0.0, 1.0); 
}

vec2 satv(vec2 x) {
    return clamp(x, vec2(0.0), vec2(1.0)); 
}

float max2(vec2 v) {
    return max(v.x, v.y); 
}

void main(){
    vec2 dudv = vec2(
        length(vec2(dFdx(inUV.x), dFdy(inUV.x))), 
        length(vec2(dFdx(inUV.y), dFdy(inUV.y)))
    );

    float lodLevel = max(0.0, log10((length(dudv) * gridMinPixelsBetweenCells) / gridCellSize) + 1.0); 
    float lodFade = fract(lodLevel); 

    float lod0 = gridCellSize * pow(10.0, floor(lodLevel + 0)); 
    float lod1 = gridCellSize * pow(10.0, floor(lodLevel + 1)); 
    float lod2 = gridCellSize * pow(10.0, floor(lodLevel + 2)); 

    float lod0a = max2(vec2(1.0) - abs(satv(mod(inUV, lod0) / dudv) * 2.0 - vec2(1.0)));
    float lod1a = max2(vec2(1.0) - abs(satv(mod(inUV, lod1) / dudv) * 2.0 - vec2(1.0)));
    float lod2a = max2(vec2(1.0) - abs(satv(mod(inUV, lod2) / dudv) * 2.0 - vec2(1.0)));

    vec4 c = lod2a > 0.0 ? gridColourThick : lod1a > 0.0 ? mix(gridColourThick, gridColourThin, lodFade) : gridColourThin; 

    float opacityFalloff = (1.0 - satf(length(inUV) / gridSize)); 

    c.a *= lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a : (lod0a * (1.0 - lodFade)); 
    c.a *= opacityFalloff; 

    outColour = c; 
}


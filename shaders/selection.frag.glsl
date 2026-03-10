#version 320 es
precision mediump float;

in vec2 vPos;
out vec4 FragColor;

uniform float uTime;
uniform vec4 uColor1;
uniform vec4 uColor2;
uniform vec2 uMin;
uniform vec2 uMax;

void main()
{
    float borderWidth = 0.008;


    float distX = min(vPos.x - uMin.x, uMax.x - vPos.x);
    float distY = min(vPos.y - uMin.y, uMax.y - vPos.y);
    float dist  = min(distX, distY);

    if (dist > borderWidth)
        discard;

    float dash_length = 0.025;
    float gap_length  = 0.015;
    float total       = dash_length + gap_length;

    float speed       = 0.3;

    float distance_along = (vPos.x + vPos.y) * 20.0;

    float t = mod( distance_along * total + uTime * speed, total);

    if (t < dash_length)
        FragColor = uColor2;
    else
        FragColor = uColor1;
}
#version 320 es
precision mediump float;

uniform vec4 uColor; // RGBA

out vec4 FragColor;

void main() {
    FragColor = uColor;
}
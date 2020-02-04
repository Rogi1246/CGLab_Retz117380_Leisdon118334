#version 150

// creating FrameBufferobject with Texture as Color Attachment
// get Texture coordinates
in vec2 pass_TextureCoords;

out vec4 out_Color;

// again: uniform sampler variable 
uniform sampler2D ColorText;

void main() {
    float x = pass_TextureCoords.x;
    float y = pass_TextureCoords.y;


    out_Color = texture(ColorText, vec2(x,y));
}
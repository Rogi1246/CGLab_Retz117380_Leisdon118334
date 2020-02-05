#version 150

// creating FrameBufferobject with Texture as Color Attachment
// get Texture coordinates
in vec2 pass_TextureCoords;

out vec4 out_Color;

// again: uniform sampler variable 
uniform sampler2D colorText;

void main() {
    float x = pass_TextureCoords.x;
    float y = pass_TextureCoords.y;

    //mirror vertical
    //out_Color = texture(colorText, vec2(640-x,y));
    //mirror horizonal
    //out_Color = texture(colorText, vec2(x,480-y));

    //inverse:
    //out_Color = vec4(vec3(1.0 - texture(colorText, vec2(x,y))), 1.0);
    
    //greyscale:
    //vec4 FragColor = texture(colorText, vec2(x,y));
    //float average = (0.2126 *FragColor.r + 0.7152*FragColor.g + 0.0722*FragColor.b) / 3.0;
    //out_Color = vec4(average,average,average,1.0);
    
    out_Color = texture(colorText, vec2(x,y));
}
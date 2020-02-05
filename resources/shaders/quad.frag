#version 150

// creating FrameBufferobject with Texture as Color Attachment
// get Texture coordinates
in vec2 pass_TextureCoords;


out vec4 out_Color;

// again: uniform sampler variable 
uniform sampler2D colorText;
uniform int greyscale;
uniform int horizontal;
uniform int vertical;
uniform int blur; 



void main() {
    const float offset = 1.0 / 300.0;
    float x = pass_TextureCoords.x;
    float y = pass_TextureCoords.y;
    vec2 coord_edit = vec2(x,y);

    out_Color = texture(colorText, vec2(x,y));
    
    
    if(horizontal == 1){
        out_Color = texture(colorText, vec2(x,480-y));  
        coord_edit = vec2(x,480-y);  
    }
    if(vertical == 1){
        out_Color = texture(colorText, vec2(640-x,y));
        coord_edit = vec2(640-x,y);
    }
    if(horizontal == 1 && vertical == 1 ){
        out_Color = texture(colorText, vec2(640-x,480-y));
        coord_edit = vec2(640-x,480-y);
    }
    if(blur == 1){
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset));  // bottom-right

        float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16 );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
        sampleTex[i] = vec3(texture(colorText, coord_edit.st + offsets[i]));
        }   
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];
    
        out_Color = vec4(col, 1.0);

    }
    //inverse:
    //out_Color = vec4(vec3(1.0 - texture(colorText, vec2(x,y))), 1.0);
    
    //greyscale:
    if(greyscale == 1){
        float average = (0.2126 *out_Color.r + 0.7152*out_Color.g + 0.0722*out_Color.b) / 3.0;
        out_Color = vec4(average,average,average,1.0);
    }
    

    
}
#version 150
#extension GL_ARB_explicit_attrib_location : require
// for extension info see skybox vertexshader

// get the vertex attributes
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TextureCoords;

out vec2 pass_TextureCoords; // what's then passed to frag

void main(void) {
    gl_Position = vec4(in_Position, 1.0);

    pass_TextureCoords = in_TextureCoords;

}
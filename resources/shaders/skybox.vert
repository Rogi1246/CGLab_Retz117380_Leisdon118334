#version 150
#extension GL_ARB_explicit_attrib_location : require
// Khronos information about extension :
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_explicit_attrib_location.txt
// VAO attributes

layout(location = 0) in vec4 in_Position;

uniform mat4 ProjectionMat;
uniform mat4 ViewMat;

out vec3 camDirection;

void main() {
    mat4 inverseProj = inverse(ProjectionMat);
    mat3 inverseModView = transpose(mat3(ViewMat));
    vec3 unprojected = (inverseProj * in_Position).xyz;
    camDirection = inverseModView * unprojected;
    gl_Position = in_Position;
}

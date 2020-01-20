#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TextCoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
//uniform mat4 NormalMatrix;

//input planet color for fragment shader
//uniform vec3 PlanetColor;

//pass to fragment shader
out vec3 pass_Normal;
out vec3 pass_FragmentPos;
out vec3 pass_CameraPos;
out vec2 pass_TextCoord;
//out vec3 Planet_Color;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	//pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	pass_Normal = mat3(ModelMatrix) * in_Normal;
	pass_FragmentPos = vec3(ModelMatrix * vec4(in_Position, 1.0));
	pass_CameraPos = (ModelMatrix * ViewMatrix * vec4(in_Position, 1.0)).xyz;
	//Planet_Color = PlanetColor;
	pass_TextCoord = in_TextCoord;
}

#version 150

in  vec3 pass_Normal;
in vec3 Planet_Color;
out vec4 out_Color;


void main() {
  //out color just the planet color at the moment ^^
  out_Color = vec4(Planet_Color, 1.0);
  
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}

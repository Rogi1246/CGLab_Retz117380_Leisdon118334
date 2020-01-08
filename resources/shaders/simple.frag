#version 150

in vec3 pass_Normal;
in vec3 pass_FragmentPos;
in vec3 pass_CameraPos;

out vec4 out_Color;

//incoming light-values
uniform vec3 lightSrc;
uniform vec3 lightCol;
uniform float lightInt;
//diffuse Color
uniform vec3 diffCol;

//shaderSwitch -- celshading stuff
// uniform int shaderSwitch; //b-phong : 1 | cel : 2
//const int shadeLevel = 3;
//float outLine = 1.0;

//define specular color
const vec3 specCol = vec3(1.0, 1.0, 1.0); //make it white


void main() {
  //float reflect = 10.0; // rho-value
  float brightness = 0.5;  //alpha -- aka specular strength

  //ambient value
  vec3 ambiance = 0.1 * lightCol * diffCol;

  vec3 normal = normalize(pass_Normal);
  vec3 lightDirection = normalize(lightSrc - pass_FragmentPos);

  //calculate actual diffuse impact
  //since dot product becomes negative when angle is above 90
  //use max function to return highest of both parameters
  float diff = max(dot(normal, lightDirection), 0.0);
  vec3 diffuse = diff * lightCol;

  //specular light
  vec3 cameraDirection = normalize(pass_CameraPos - pass_FragmentPos);
  vec3 halfwayDirection = normalize(lightDirection + cameraDirection);
  float spec = pow(max(dot(normal, halfwayDirection), 0.0), brightness);
  vec3 specular = lightCol * spec;

  vec3 result_col = (ambiance+diffuse+specular)*diffCol;
  out_Color = vec4(result_col, 1.0);
}

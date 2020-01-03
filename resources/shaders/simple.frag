#version 150

in vec3 pass_Normal;
in vec3 pass_FragmentPos;
in vec3 pass_CameraPos;
//in vec3 Planet_Color;

out vec4 out_Color;

//light-values
uniform vec3 lightSrc;
uniform vec3 lightCol;
uniform float lightInt;
//diffuse Color and shaderSwitch uniforms
uniform vec3 diffCol;
uniform int shaderSwitch; //b-phong : 1 | cel : 2
//cel-shading stuff
const int shadeLevel = 3;
float outLine = 1.0;

void main() {
  //out_Color = vec4(abs(normalize(pass_Normal)),1.0);

  //Shading-values
  vec3 ambCol = diffCol*0.4;
  vec3 specCol = vec3(1.0, 1.0, 1.0); //make it white 
  float brightness = 30.0f; //and make it shine

  vec3 lightDir = lightSrc - pass_FragmentPos;
  float dist = length(lightDir);

  //normalizing 
  vec3 normal = normalize(pass_Normal);
  vec3 light = normalize(lightDir);
  vec3 cam = normalize(pass_CameraPos - pass_FragmentPos);
  vec3 halfWayThere = normalize(cam + light);

  //dot-products
  float diffAngle = max(0.0, dot(light, normal));
  float specAngle = pow(max(0.0, dot(normal, halfWayThere)), brightness);

  //cel-shader
  if(shaderSwitch == 2) {
    //discretize diffAngle to determine intensity
    diffAngle = floor(diffAngle * shadeLevel) / shadeLevel;

    //discretize specLight
    if(specAngle < 0.5) {
      specAngle = 0.0;
    }

    //outLine
    if(dot(normal, cam) < 0.25) {
      outLine = 0.0;
    }
  }

  //ambiance, diffuse, specular
  vec3 ambient = ambCol;
  vec3 diffuse = diffCol * diffAngle * lightCol * lightInt / dist;
  vec3 specular = specCol * specAngle * lightCol * lightInt / dist;

  //resulting value
  vec3 finalCol = outLine * (ambient + diffuse + specular);
  out_Color = vec4(finalCol, 1.0);
  
  //out color just the planet color at the moment ^^
  //out_Color = vec4(Planet_Color, 1.0);
  
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}

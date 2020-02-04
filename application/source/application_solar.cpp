#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "geometry_node.hpp"

//just global atm, trying out 
const int star_count = 2000;
PointLightNode pointLight;

//add star_object{} in here later 
ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 // add skybox object
 ,skybox_object{}
 // screenquad object
 ,screenquad_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeTextures();
  initializeSkybox();
  initializeGeometry();
  initializeFrameBuffer();
  initializeScreenquad();
  
  initializeStarGeometry();
  initializeShaderPrograms();
  
}

ApplicationSolar::~ApplicationSolar() {
  //delete planet-buffers and vertex-array
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  //do the same for stars (in reverse order of creation)
  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  //finally add deletion of textures 
  //planets
  for(int i = 0; i < texObjects_.size(); i++) {
    glDeleteTextures(1, &texObjects_[i].handle);
  }

  // delete 
  glDeleteBuffers(1, &screenquad_object.vertex_BO);
  glDeleteBuffers(1, &screenquad_object.element_BO);
  glDeleteVertexArrays(1,&screenquad_object.vertex_AO);

}

void ApplicationSolar::render() const {

  // binding the new FrameBuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
  // clear it
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawSkybox();
  renderStars();
  renderPlanets();

  // bind default Framebuffer and render Screenquad
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(m_shaders.at("quad").handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frameBuffer_tex_obj_.handle);
  int color_sampler_loc = glGetUniformLocation(m_shaders.at("quad").handle, "ColorText");
  // use color_sampler_loc
  glUniform1i(color_sampler_loc, 0);
  glBindVertexArray(screenquad_object.vertex_AO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  
}

void ApplicationSolar::loadTextures(){

  //std::cout << "in texture loader: " << std::endl;
    pixel_data sun = texture_loader::file(m_resource_path + "textures/sun2.png");
    textures_.push_back(sun);
    pixel_data mercury = texture_loader::file(m_resource_path + "textures/mercury.png");
    textures_.push_back(mercury);
    pixel_data venus = texture_loader::file(m_resource_path + "textures/venus.png");
    textures_.push_back(venus);
    pixel_data earth = texture_loader::file(m_resource_path + "textures/earth.png");
    textures_.push_back(earth);
    pixel_data moon = texture_loader::file(m_resource_path + "textures/moon.png");
    textures_.push_back(moon);
    pixel_data mars = texture_loader::file(m_resource_path + "textures/mars.png");
    textures_.push_back(mars);
    pixel_data jupiter = texture_loader::file(m_resource_path + "textures/jupiter.png");
    textures_.push_back(jupiter);
    pixel_data saturn = texture_loader::file(m_resource_path + "textures/saturn.png");
    textures_.push_back(saturn);
    pixel_data uranus = texture_loader::file(m_resource_path + "textures/uranus.png");
    textures_.push_back(uranus);
    pixel_data neptune = texture_loader::file(m_resource_path + "textures/pepe2.png");
    textures_.push_back(neptune);
    pixel_data pluto = texture_loader::file(m_resource_path + "textures/pluto.png");
    textures_.push_back(pluto);
  }

void ApplicationSolar::initializeTextures(){
  loadTextures();
  //std::cout  << "textures_ size: "<< textures_.size() << std::endl;
    for(int i = 0; i<textures_.size(); ++i){
      texture_object texture = texture_object{};

      texture.target = GL_TEXTURE0;
      //initialize Texture
      glActiveTexture(texture.target);
      glGenTextures(1, &texture.handle);
      glBindTexture(GL_TEXTURE_2D, texture.handle);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, textures_[i].channels, textures_[i].width, textures_[i].height, 0, textures_[i].channels, textures_[i].channel_type, textures_[i].ptr());
      
      texObjects_.push_back(texture);
      std::cout << "texture init size: " << texObjects_.size() << std::endl;
    }
  }

void ApplicationSolar::initializeScenegraph() {
  
  //std::cout <<"tex objects size: " << texObjects_.size() << std::endl;
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  Node root("Root",nullptr);

  //set lightColor and create PointLight
  glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
  pointLight = {"light", std::make_shared<Node>(root), lightColor, 1.0};
  scenegraphList_.push_back(std::make_shared<PointLightNode>(pointLight));
  //add PointLight to root
  //root.addChild(std::make_shared<PointLightNode>(pointLight));

  //create root-node and initial Scenegraph
  Node sunH{"Sun Holder", std::make_shared<Node>(root)};
  sunH.setRotationSpeed(0.0f);
  sunH.setDistance(0.0f);
  
  GeometryNode sun{"Sun", std::make_shared<Node>(sunH)};
  sun.setGeometry(planet_model);
  sun.setSize(1.0f);
  sun.setPlanetColor(glm::vec3{1.0f, 1.0f, 0.0f});
  sun.setIsSUn(1);
  sun.setPlanetTexture(texObjects_[0]); // THIS BITCH NOT WORKING -- SEGMENTATION FAULT WHEN LOADING TEXTURE
  sunH.addChild(std::make_shared<GeometryNode>(sun));
  scenegraphList_.push_back(std::make_shared<Node>(sunH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(sun));

  //creating planets
  Node mercuryH{"Mercury Holder", std::make_shared<Node>(root)};
  mercuryH.setRotationSpeed(4.7f);
  mercuryH.setDistance(2.0f);

  GeometryNode mercury("Mercury", std::make_shared<Node>(mercuryH));
  mercury.setGeometry(planet_model);
  mercury.setSize(0.07f);
  mercury.setPlanetColor(glm::vec3{0.4f, 0.4f, 0.7f});
  
  mercury.setPlanetTexture(texObjects_[1]);
  mercuryH.addChild(std::make_shared<GeometryNode>(mercury));
  scenegraphList_.push_back(std::make_shared<Node>(mercuryH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(mercury));


  Node venusH{"Venus Holder", std::make_shared<Node>(root)};
  venusH.setRotationSpeed(3.5f);
  venusH.setDistance(3.0f);

  GeometryNode venus("Venus", std::make_shared<Node>(venusH));
  venus.setGeometry(planet_model);
  venus.setSize(0.12f);
  venus.setPlanetColor(glm::vec3{0.8f, 0.7f, 0.0f});
  venus.setPlanetTexture(texObjects_[2]);
  venusH.addChild(std::make_shared<GeometryNode>(venus));
  scenegraphList_.push_back(std::make_shared<Node>(venusH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(venus));

  Node earthH{"Earth Holder", std::make_shared<Node>(root)};
  earthH.setRotationSpeed(2.9f);
  earthH.setDistance(4.5f);

  GeometryNode earth("Earth", std::make_shared<Node>(earthH));
  earth.setGeometry(planet_model);
  earth.setSize(0.17f);
  earth.setPlanetColor(glm::vec3{0.51f, 0.15f, 0.7f});
  earth.setPlanetTexture(texObjects_[3]);
  earthH.addChild(std::make_shared<GeometryNode>(earth));
  scenegraphList_.push_back(std::make_shared<Node>(earthH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(earth));

  Node moonH{"Moon Holder", std::make_shared<Node>(earthH)};
  moonH.setRotationSpeed(2.9f);
  moonH.setDistance(0.4f);

  GeometryNode moon("Moon",std::make_shared<Node>(moonH));
  moon.setGeometry(planet_model);
  moon.setSize(0.05f);
  moon.setIsMoon(true);
  moon.setPlanetColor(glm::vec3{0.1f, 0.2f, 0.1f});
  moon.setPlanetTexture(texObjects_[4]);


  earthH.addChild(std::make_shared<Node>(moonH));
  moonH.addChild(std::make_shared<GeometryNode>(moon));
  scenegraphList_.push_back(std::make_shared<Node>(moonH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(moon));

  Node marsH{"Mars Holder", std::make_shared<Node>(root)};
  marsH.setRotationSpeed(2.4f);
  marsH.setDistance(5.5f);

  GeometryNode mars("Mars", std::make_shared<Node>(marsH));
  mars.setGeometry(planet_model);
  mars.setSize(0.12f);
  mars.setPlanetColor(glm::vec3{0.5f, 0.2f, 0.09f});
  mars.setPlanetTexture(texObjects_[5]);
  marsH.addChild(std::make_shared<GeometryNode>(mars));
  scenegraphList_.push_back(std::make_shared<Node>(marsH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(mars));

  Node jupiterH{"Jupiter Holder", std::make_shared<Node>(root)};
  jupiterH.setRotationSpeed(1.3f);
  jupiterH.setDistance(6.8f);

  GeometryNode jupiter("Jupiter", std::make_shared<Node>(jupiterH));
  jupiter.setGeometry(planet_model);
  jupiter.setSize(0.52f);
  jupiter.setPlanetColor(glm::vec3{ 0.9f, 0.58f, 0.0f});
  jupiter.setPlanetTexture(texObjects_[6]);
  jupiterH.addChild(std::make_shared<GeometryNode>(jupiter));
  scenegraphList_.push_back(std::make_shared<Node>(jupiterH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(jupiter));

  Node saturnH("SaturnHolder", std::make_shared<Node>(root));
  saturnH.setRotationSpeed(0.97f);
  saturnH.setDistance(8.4f);
  
  GeometryNode saturn("Saturn", std::make_shared<Node>(saturnH));
  saturn.setGeometry(planet_model);
  saturn.setSize(0.47f);
  saturn.setRotationSpeed(0.5f);
  saturn.setPlanetColor(glm::vec3{ 0.9f, 0.75f, 0.54f});
  saturn.setPlanetTexture(texObjects_[7]);
  saturnH.addChild(std::make_shared<GeometryNode>(saturn));
  scenegraphList_.push_back(std::make_shared<Node>(saturnH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(saturn));

  Node uranusH{"Uranus Holder", std::make_shared<Node>(root)};
  uranusH.setRotationSpeed(0.68f);
  uranusH.setDistance(9.5f);
   
  GeometryNode uranus("Uranus", std::make_shared<Node>(uranusH));
  uranus.setGeometry(planet_model);
  uranus.setSize(0.35f);
  uranus.setPlanetColor(glm::vec3{0.69f, 0.893f, 0.93f});
  uranus.setPlanetTexture(texObjects_[8]);
  uranusH.addChild(std::make_shared<GeometryNode>(uranus));
  scenegraphList_.push_back(std::make_shared<Node>(uranusH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(uranus));


  Node neptuneH{"Neptune Holder", std::make_shared<Node>(root)};
  neptuneH.setRotationSpeed(0.54f);
  neptuneH.setDistance(11.4f);

  GeometryNode neptune("Neptune", std::make_shared<Node>(neptuneH));
  neptune.setGeometry(planet_model);
  neptune.setSize(0.3f);
  neptune.setPlanetColor(glm::vec3{0.69f, 0.81f, 0.93f});
  neptune.setPlanetTexture(texObjects_[9]);
  neptuneH.addChild(std::make_shared<GeometryNode>(neptune));
  scenegraphList_.push_back(std::make_shared<Node>(neptuneH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(neptune));

  Node plutoH{"Pluto Holder", std::make_shared<Node>(root)};
  plutoH.setRotationSpeed(0.4f);
  plutoH.setDistance(12.5f);

  GeometryNode pluto("Pluto", std::make_shared<Node>(plutoH));
  pluto.setGeometry(planet_model);
  pluto.setSize(0.07f);
  pluto.setPlanetColor(glm::vec3{0.9f, 0.22f, 0.01f});
  pluto.setPlanetTexture(texObjects_[10]);
  plutoH.addChild(std::make_shared<GeometryNode>(pluto));
  scenegraphList_.push_back(std::make_shared<Node>(plutoH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(pluto));

  root.addChild(std::make_shared<Node>(mercuryH));
  root.addChild(std::make_shared<Node>(venusH));
  root.addChild(std::make_shared<Node>(earthH));
  root.addChild(std::make_shared<Node>(marsH));
  root.addChild(std::make_shared<Node>(jupiterH));
  root.addChild(std::make_shared<Node>(saturnH));
  root.addChild(std::make_shared<Node>(neptuneH));
  root.addChild(std::make_shared<Node>(uranusH));
  root.addChild(std::make_shared<Node>(plutoH));

  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(sun));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(mercury));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(venus));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(earth));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(moon));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(mars));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(jupiter));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(saturn));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(uranus));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(neptune));
  scenegraphList_geometry_.push_back(std::make_shared<GeometryNode>(pluto));

  Scenegraph init_Scene{"init_Scene", std::make_shared<Node>(root)};
  scenegraph_ = init_Scene;

}

/////////////////////// RENDER METHODS /////////////////////////////////////////////////////////

void ApplicationSolar::renderPlanets() const {

  for(auto planet : scenegraphList_geometry_){
    if(planet->getChildList().empty()){

      glm::mat4 planetMatrix = planet->getLocalTransform();
      auto parent = planet->getParent();
      //int depth = 2;

      if(planet->getIsMoon() == true){
        auto earthH = parent->getParent();
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * earthH->getRotationSpeed()/4, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, earthH->getDistance()+0.2});
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/4, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
      } else {
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/4, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
      }
     
      planetMatrix = glm::scale(planetMatrix, glm::vec3(planet->getSize(), planet->getSize(), planet->getSize()));
      

      // glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)*planetMatrix);
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

      glUseProgram(m_shaders.at("planet").handle);
      
      // load planet color in the shader (normalized with /255)
      glUniform3f(m_shaders.at("planet").u_locs.at("diffCol"), (planet->getPlanetColor().x), (planet->getPlanetColor().y), (planet->getPlanetColor().z));
      
      glUniform1i(m_shaders.at("planet").u_locs.at("is_sun"), planet->getIsSun());
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                         1, GL_FALSE, glm::value_ptr(planetMatrix));

      //OKAY lets try and bind proper texture-stuff
      //if(planet->getIsSun() == 1) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, planet->getPlanetTexture().handle);
      int sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "textures");

      glUseProgram(m_shaders.at("planet").handle);
      glUniform1i(sampler_location,0);
      //}
       //extra matrix for normal transformation to keep them orthogonal to surface
      
      /*glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                         1, GL_FALSE, glm::value_ptr(normal_matrix));
      */

      // bind the VAO to draw
      glBindVertexArray(planet_object.vertex_AO);

      // draw bound vertex array using bound shader
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    }
  }
}


// renderStars-method so they will show up
void ApplicationSolar::renderStars() const {
  //bind the shader
  glUseProgram(m_shaders.at("star").handle);
  //bind VAO(vertex-array-object) to draw
  glBindVertexArray(star_object.vertex_AO);
  //draw the bound vertex array 
  glPointSize(2.0);
  glDrawArrays(star_object.draw_mode, gl::GLint(0), star_object.num_elements);
}

//////////////////// UPLOAD FUNCTIONS //////////////////////////////////////////////////////////
// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  //binding the planet-shader-programs
  glUseProgram(m_shaders.at("planet").handle);

  //3f 3 float values, 1f 1 float value and so on
  //set the light-source (at the uniform positions)
  glUniform3f(m_shaders.at("planet").u_locs.at("lightSrc"), 0.0f, 0.0f, 0.0f);
  //get the color of the pointLight
  glm::vec3 lightColorF = pointLight.getLightCol();
  //maybe add +0,1,2 to xyz coord again? 
  glUniform3f(m_shaders.at("planet").u_locs.at("lightCol"), (lightColorF.x), (lightColorF.y), (lightColorF.z));
  float lightIntF = pointLight.getLightInt();
  glUniform1f(m_shaders.at("planet").u_locs.at("lightInt"), lightIntF);
  glUniform1i(m_shaders.at("planet").u_locs.at("shaderSwitch"), 1);
  
  int sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "textures");
  glUniform1i(sampler_location, 0);
  // bind shader to which to upload unforms
   glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}


void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

  //bind shader & upload ViewMatrix for stars
  //printf("star view uploading! \n");
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

  //bind & upload ViewMatric for skybox
  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMat"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  printf("planet projection uploading! \n");
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));

  //have to bind the shaders & upload the ProjectionMatrix 
  printf("star projection uploading! \n");
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"), 
                      1, GL_FALSE, glm::value_ptr(m_view_projection));

  // bind and upload projection matrix for skybox
  printf("skybox projection uploading! \n");
  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMat"),
                      1, GL_FALSE, glm::value_ptr(m_view_projection));
}

void ApplicationSolar::drawSkybox() const {
  glDepthMask(GL_FALSE);
  glUseProgram(m_shaders.at("skybox").handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_obj_.handle);
  glBindVertexArray(skybox_object.vertex_AO);
  glDrawElements(skybox_object.draw_mode, skybox_object.num_elements, model::INDEX.type, NULL);
  glDepthMask(GL_TRUE);
}

///////////////////////////// intialisation functions /////////////////////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  //m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  //m_shaders.at("planet").u_locs["PlanetColor"] = -1;
  //now instead of planetcolor, use the actual colors (the shading variables)
  m_shaders.at("planet").u_locs["lightSrc"] = -1;
  m_shaders.at("planet").u_locs["lightCol"] = -1;
  m_shaders.at("planet").u_locs["lightInt"] = -1;
  m_shaders.at("planet").u_locs["diffCol"] = -1;
  m_shaders.at("planet").u_locs["textures"] = -1;
  m_shaders.at("planet").u_locs["shaderSwitch"] = -1;
  m_shaders.at("planet").u_locs["is_sun"] = -1;


  //have to do the same for stars
  //store in container
  m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/star.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/star.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  // now initialize shaders for skybox
  m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/skybox.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});

  // request uniform location for shader program
  m_shaders.at("skybox").u_locs["ProjectionMat"] = -1;
  m_shaders.at("skybox").u_locs["ViewMat"] = -1;

  // stuff for screenquad
  m_shaders.emplace("quad", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/quad.vert"},
                                            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/quad.frag"}}});

  m_shaders.at("quad").u_locs["ColorText"] = -1;

}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::TEXCOORD);
  // add skybox model
  model skybox_model = model_loader::obj(m_resource_path + "models/skybox.obj");

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());

  initializeScenegraph();

  // The skybox-cube
  // starting with VAO
  glGenVertexArrays(1, &skybox_object.vertex_AO);
  // bind that 
  glBindVertexArray(skybox_object.vertex_AO);
  // generic buffer
  glGenBuffers(1, &skybox_object.vertex_BO);
  // again : binding as vertex array buffer with all attributes
  glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
  // configuration
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_model.data.size(), skybox_model.data.data(), GL_STATIC_DRAW);
  // activation of first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, skybox_model.vertex_bytes, skybox_model.offsets[model::POSITION]);
  // generate generic buffer
  glGenBuffers(1, &skybox_object.element_BO);
  // bind as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * skybox_model.indices.size(), skybox_model.indices.data(), GL_STATIC_DRAW);
  // store type of primitive to draw
  skybox_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object
  skybox_object.num_elements = GLsizei(skybox_model.indices.size());
}

  ////////////////////// Initialize FrameBuffer //////////////////////

  void ApplicationSolar::initializeFrameBuffer() {
    // resolution 
    GLsizei WIDTH = 640;
    GLsizei HEIGHT = 480;

    // create Textureobject and bind it
    glGenTextures(1, &frameBuffer_tex_obj_.handle);
    glBindTexture(GL_TEXTURE_2D, frameBuffer_tex_obj_.handle);
    // defining the sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // define Tex-data and -format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // generating Renderbufferobject and binding it
    glGenRenderbuffers(1, &rendBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, rendBufferHandle);
    // specify the properties
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT);

    // generating FrameBufferobject and binding it 
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    // specify Tex-object attachments
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBuffer_tex_obj_.handle, 0);

    // specify RenderBuffer-object attachments
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rendBufferHandle);

    // create array for color attachments
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    // receive fragments
    glDrawBuffers(1, draw_buffers);

    // checking for Framebuffer status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << " ------- FRAMEBUFFER NOT COMPLETE! ------- " << std::endl;
    }
    
  }

  //Initialize Screenquad
  void ApplicationSolar::initializeScreenquad() {
    // model screenquad_model = model_loader::obj(m_resource_path + "models/screenquad.obj", model::TEXCOORD);
    
    static const GLfloat screenquad_model[] = {
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// v1
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// v2
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// v4
         1.0f, 1.0f, 0.0f, 1.0f, 1.0f	// v3
     };

    // generating VAO and binding it 
    glGenVertexArrays(1, &screenquad_object.vertex_AO);
    glBindVertexArray(screenquad_object.vertex_AO);

    // generating generic buffer and bind it as Vertex array buffer
    glGenBuffers(1, &screenquad_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
    // configuration
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenquad_model), screenquad_model, GL_STATIC_DRAW);

    // activating first attribute on GPU
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3*sizeof(float)));

  }


  void ApplicationSolar::initializeSkybox() {

    /* used as reference : 
    https://learnopengl.com/Advanced-OpenGL/Cubemaps
    */

    // load the textures for the skybox
    // try with sequence they are looped through
    // POSITIVE_X RIGHT
    pixel_data sideright = texture_loader::file(m_resource_path + "textures/right2.png");
    skybox_contain_pixdata_.push_back(sideright);
    std::cout << "uploaded right pxd" << std::endl;
    //NEGATIVE_X LEFT
    pixel_data sideleft = texture_loader::file(m_resource_path + "textures/left2.png");
    skybox_contain_pixdata_.push_back(sideleft);
    std::cout << "uploaded left pxd" << std::endl;
    //POSITIVE_Y TOP
    pixel_data up = texture_loader::file(m_resource_path + "textures/top2.png");
    skybox_contain_pixdata_.push_back(up);
    std::cout << "uploaded top pxd" << std::endl;
    //NEGATIVE_Y BOTTOM
    pixel_data down = texture_loader::file(m_resource_path + "textures/bottom2.png");
    skybox_contain_pixdata_.push_back(down);
    std::cout << "uploaded bottom pxd" << std::endl;
    //POSITIVE_Z BACK
    pixel_data back = texture_loader::file(m_resource_path + "textures/back2.png");
    skybox_contain_pixdata_.push_back(back);
    std::cout << "uploaded back pxd" << std::endl;
    //NEGATIVE_Z FRONT
    pixel_data front = texture_loader::file(m_resource_path + "textures/front2.png");
    skybox_contain_pixdata_.push_back(front);
    std::cout << "uploaded front pxd" << std::endl;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &skybox_texture_obj_.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_obj_.handle);
    // The WARP_S/T/R set the warping methods for the textures s/t/r coordinates
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    for(unsigned int i = 0; i < skybox_contain_pixdata_.size(); ++i) {
      // starting off with GL_TEXTURE_CUBE_MAP_POSITIVE_X and incrementing it with +i so it will loop through the
      // texture targets 
      // POSITIVE_X being right, NEGATIVE_X being left etc. etc.
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, skybox_contain_pixdata_[i].channels,
                    (GLsizei)skybox_contain_pixdata_[i].width, (GLsizei)skybox_contain_pixdata_[i].height,
                    0, skybox_contain_pixdata_[i].channels, skybox_contain_pixdata_[i].channel_type, skybox_contain_pixdata_[i].ptr());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
  }

  void ApplicationSolar::initializeStarGeometry() {
  model star_model;
  //for indices 
  int index = 0;
  //hold positions and colors for stars (2 * 3-values(coordinates and colors) *count of stars)
  GLfloat starVertexBuffer[2*3*star_count];
  //randomizing values 
  for (int i = 0; i < star_count; i++) {
    //position
    for (int j = 0; j < 3; j++) {
      starVertexBuffer[index]= (static_cast<float>(std::rand() % 100) - (50.0f / 2));
      index++;
    }
    //color
    for (int k = 0; k < 3; k++) {
      starVertexBuffer[index] = ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) / 2.0f + 0.5f);
      index++;
    }
  }

  //vertex array
  glGenVertexArrays(1, &star_object.vertex_AO);
  //bind for attachment
  glBindVertexArray(star_object.vertex_AO);

  //generic Buffer
  glGenBuffers(1, &star_object.vertex_BO);
  //bind as vertex array
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
  //configuration (with target, size, the data itself and how to make use of it)
  glBufferData(GL_ARRAY_BUFFER, sizeof(starVertexBuffer), starVertexBuffer, GL_STATIC_DRAW);

  //activate pos on gpu (at index 0)
  glEnableVertexAttribArray(0);
  //position data (at index, size, with type, if boolean-normalized, stride and a pointer)
  //stride in this context meaning, that moving from one vertex to the next OpenGL has to move along by bytes (each vertex taking up 4)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),(GLvoid*)0);
  //activate col on gpu  (at index 1)
  glEnableVertexAttribArray(1);
  //color data (at index, with size, type, if boolean-normalized, stride (see explanation above) and pointer)
  glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));

  //generic buffer
  glGenBuffers(1, &star_object.element_BO);
  //binding as vertex array buffer w/ all attribs
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
  //configuration 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(starVertexBuffer), starVertexBuffer, GL_STATIC_DRAW);

  //store type-primitive to draw
  star_object.draw_mode = GL_POINTS;
  //transfer indices
  star_object.num_elements = GLsizei(star_count);
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
  //move left
  else if (key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
    uploadView();
  }
  //move right
  else if (key == GLFW_KEY_D  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
    uploadView();
  }
  // move down
  else if (key == GLFW_KEY_LEFT_CONTROL  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.1f, 0.0f});
    uploadView();
  }
  //move up
  else if (key == GLFW_KEY_SPACE  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.1f, 0.0f});
    uploadView();
  }
  //key callback for shader switching
  //Pressing 1 : BPhong
  else if (key == GLFW_KEY_1) {
    glUseProgram(m_shaders.at("planet").handle);
    glUniform1i(m_shaders.at("planet").u_locs.at("shaderSwitch"),1);
    uploadView();
  }
  //Pressing 2 : CelShading
  else if (key == GLFW_KEY_2) {
    glUseProgram(m_shaders.at("planet").handle);
    glUniform1i(m_shaders.at("planet").u_locs.at("shaderSwitch"),2);
    uploadView();
  }

}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

  float angle = 0.04f;
  m_view_transform = glm::rotate(m_view_transform, angle, glm::vec3{-pos_y, -pos_x, 0.0f});
  uploadView();
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}

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


ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  initializeShaderPrograms();
  initializeScenegraph();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  //glUseProgram(m_shaders.at("planet").handle);

  //glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  //model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});
  //glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
  //                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  //glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  //glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
    //                 1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  //glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  //glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  renderPlanets();
}


void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

void ApplicationSolar::initializeScenegraph() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  //create root-node and initial Scenegraph
  GeometryNode sun{"Sun"};
  sun.setGeometry(planet_model);
  sun.setRotationSpeed(0.0f);
  sun.setDistance(0.0f);
  sun.setSize(1.0f);
  //creating planets
  GeometryNode mercury("Mercury");
  mercury.setGeometry(planet_model);
  mercury.setRotationSpeed(4.7f);
  mercury.setDistance(2.0f);
  mercury.setSize(0.07f);

  GeometryNode venus("Venus");
  venus.setGeometry(planet_model);
  venus.setRotationSpeed(3.5f);
  venus.setDistance(3.0f);
  venus.setSize(0.12f);

  GeometryNode earth("Earth");
  earth.setGeometry(planet_model);
  earth.setRotationSpeed(2.9f);
  earth.setDistance(3.5f);
  earth.setSize(0.17f);

  GeometryNode moon("Moon");
  moon.setGeometry(planet_model);
  moon.setRotationSpeed(2.9f);
  moon.setDistance(0.4f);
  moon.setSize(0.05f);

  earth.addChild(std::make_shared<GeometryNode>(moon));

  GeometryNode mars("Mars");
  mars.setGeometry(planet_model);
  mars.setRotationSpeed(2.4f);
  mars.setDistance(4.0f);
  mars.setSize(0.12f);
  
  GeometryNode jupiter("Jupiter");
  jupiter.setGeometry(planet_model);
  jupiter.setRotationSpeed(1.3f);
  jupiter.setDistance(6.0f);
  jupiter.setSize(0.52f);

  GeometryNode saturn("Saturn");
  saturn.setGeometry(planet_model);
  saturn.setRotationSpeed(0.97f);
  saturn.setDistance(7.0f);
  saturn.setSize(0.47f);

  GeometryNode uranus("Uranus");
  uranus.setGeometry(planet_model);
  uranus.setRotationSpeed(0.68f);
  uranus.setDistance(8.0f);
  uranus.setSize(0.35f);

  GeometryNode neptune("Neptune");
  neptune.setGeometry(planet_model);
  neptune.setRotationSpeed(0.54f);
  neptune.setDistance(9.0f);
  neptune.setSize(0.3f);

  GeometryNode pluto("Pluto");
  pluto.setGeometry(planet_model);
  pluto.setRotationSpeed(0.4f);
  pluto.setDistance(10.0f);
  pluto.setSize(0.07f);

  sun.addChild(std::make_shared<GeometryNode>(mercury));
  sun.addChild(std::make_shared<GeometryNode>(venus));
  sun.addChild(std::make_shared<GeometryNode>(earth));
  sun.addChild(std::make_shared<GeometryNode>(mars));
  sun.addChild(std::make_shared<GeometryNode>(saturn));
  sun.addChild(std::make_shared<GeometryNode>(jupiter));
  sun.addChild(std::make_shared<GeometryNode>(neptune));
  sun.addChild(std::make_shared<GeometryNode>(uranus));
  sun.addChild(std::make_shared<GeometryNode>(pluto));

  Scenegraph init_Scene{"init_Scene", std::make_shared<GeometryNode>(sun)};
  scenegraph_ = init_Scene;
}



void addMoon(std::string const& planet_name, std::string const& moon_name) {
  GeometryNode moon{moon_name};
  //scenegraph_.getRoot()->getChildren(planet_name)->addChild(std::make_shared<GeometryNode>(moon));
}

void ApplicationSolar::renderPlanets() const {


  auto sun = scenegraph_.getRoot();
  glm::mat4 sunMatrix = glm::mat4{};
  //sunMatrix = glm::rotate(sunMatrix, float(glfwGetTime()) * sun->getRotationSpeed(), glm::vec3{0.0f, 0.0f, 0.0f}); 
  //sunMatrix = glm::translate(sunMatrix, glm::vec3{0.0f, 0.0f, sun->getDistance()});
  //sunMatrix = glm::scale(sunMatrix, glm::vec3(sun->getSize(), sun->getSize(), sun->getSize()));

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),1, GL_FALSE, glm::value_ptr(sunMatrix));
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)*sunMatrix);
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  glBindVertexArray(planet_object.vertex_AO);
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);



  for(auto& planetIterator : scenegraph_.getRoot()->getChildList()) {
    
    glm::mat4 planetMatrix = planetIterator->getLocalTransform();
    planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * planetIterator->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
    planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, planetIterator->getDistance()+0.2});
    planetMatrix = glm::scale(planetMatrix, glm::vec3(planetIterator->getSize(), planetIterator->getSize(), planetIterator->getSize()));
   
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)*planetMatrix);
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    glUseProgram(m_shaders.at("planet").handle);

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(planetMatrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(planetMatrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

    //planet has moon
    if(planetIterator->getChildList().size()>0){
      for(auto& moonIterator: planetIterator->getChildList()){
      glm::mat4 moonMatrix = planetIterator->getLocalTransform();
      moonMatrix = glm::rotate(moonMatrix, float(glfwGetTime()) * planetIterator->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f});
      moonMatrix = glm::translate(moonMatrix, glm::vec3{0.0f,0.0f,1.0f*planetIterator->getDistance()+0.2});
      
      moonMatrix = glm::rotate(moonMatrix,float(glfwGetTime()*moonIterator->getRotationSpeed()), glm::vec3{0.0f,1.0f,0.0f});
      moonMatrix = glm::translate(moonMatrix, glm::vec3{0.0f, 0.0f, moonIterator->getDistance()});
      moonMatrix = glm::scale(moonMatrix, glm::vec3(moonIterator->getSize(), moonIterator->getSize(), moonIterator->getSize()));
      normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)*moonMatrix);

      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),1, GL_FALSE, glm::value_ptr(moonMatrix));
      glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)*moonMatrix);
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
      glBindVertexArray(planet_object.vertex_AO);
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

      }
    }

  }
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

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

}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

  float angle = 0.01f;
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
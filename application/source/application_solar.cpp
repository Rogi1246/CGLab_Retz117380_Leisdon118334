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
const int star_count = 100;

//add star_object{} in here later 
ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  initializeStarGeometry();
  initializeShaderPrograms();
  initializeScenegraph();
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
}

void ApplicationSolar::render() const {

  renderStars();
  renderPlanets();
  
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

  //bind shader & upload ViewMatrix for stars
  printf("star view uploading! \n");
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
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
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  // glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

void ApplicationSolar::initializeScenegraph() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  Node root("Root",nullptr);
  //create root-node and initial Scenegraph
  Node sunH{"Sun Holder", std::make_shared<Node>(root)};
  sunH.setRotationSpeed(0.0f);
  sunH.setDistance(0.0f);
  
  GeometryNode sun{"Sun", std::make_shared<Node>(sunH)};
  sun.setGeometry(planet_model);
  sun.setSize(1.0f);
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
  mercuryH.addChild(std::make_shared<GeometryNode>(mercury));
  scenegraphList_.push_back(std::make_shared<Node>(mercuryH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(mercury));


  Node venusH{"Venus Holder", std::make_shared<Node>(root)};
  venusH.setRotationSpeed(3.5f);
  venusH.setDistance(3.0f);

  GeometryNode venus("Venus", std::make_shared<Node>(venusH));
  venus.setGeometry(planet_model);
  venus.setSize(0.12f);
  venusH.addChild(std::make_shared<GeometryNode>(venus));
  scenegraphList_.push_back(std::make_shared<Node>(venusH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(venus));

  Node earthH{"Earth Holder", std::make_shared<Node>(root)};
  earthH.setRotationSpeed(2.9f);
  earthH.setDistance(4.5f);

  GeometryNode earth("Earth", std::make_shared<Node>(earthH));
  earth.setGeometry(planet_model);
  earth.setSize(0.17f);
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
  marsH.addChild(std::make_shared<GeometryNode>(mars));
  scenegraphList_.push_back(std::make_shared<Node>(marsH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(mars));

  Node jupiterH{"Jupiter Holder", std::make_shared<Node>(root)};
  jupiterH.setRotationSpeed(1.3f);
  jupiterH.setDistance(6.8f);

  GeometryNode jupiter("Jupiter", std::make_shared<Node>(jupiterH));
  jupiter.setGeometry(planet_model);
  jupiter.setSize(0.52f);
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
  saturnH.addChild(std::make_shared<GeometryNode>(saturn));
  scenegraphList_.push_back(std::make_shared<Node>(saturnH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(saturn));

  Node uranusH{"Uranus Holder", std::make_shared<Node>(root)};
  uranusH.setRotationSpeed(0.68f);
  uranusH.setDistance(9.5f);
   
  GeometryNode uranus("Uranus", std::make_shared<Node>(uranusH));
  uranus.setGeometry(planet_model);
  uranus.setSize(0.35f);
  uranusH.addChild(std::make_shared<GeometryNode>(uranus));
  scenegraphList_.push_back(std::make_shared<Node>(uranusH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(uranus));


  Node neptuneH{"Neptune Holder", std::make_shared<Node>(root)};
  neptuneH.setRotationSpeed(0.54f);
  neptuneH.setDistance(11.4f);

  GeometryNode neptune("Neptune", std::make_shared<Node>(neptuneH));
  neptune.setGeometry(planet_model);
  neptune.setSize(0.3f);
  neptuneH.addChild(std::make_shared<GeometryNode>(neptune));
  scenegraphList_.push_back(std::make_shared<Node>(neptuneH));
  scenegraphList_.push_back(std::make_shared<GeometryNode>(neptune));

  Node plutoH{"Pluto Holder", std::make_shared<Node>(root)};
  plutoH.setRotationSpeed(0.4f);
  plutoH.setDistance(12.5f);

  GeometryNode pluto("Pluto", std::make_shared<Node>(plutoH));
  pluto.setGeometry(planet_model);
  pluto.setSize(0.07f);
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

  Scenegraph init_Scene{"init_Scene", std::make_shared<Node>(root)};
  scenegraph_ = init_Scene;
  for(auto t : scenegraphList_){
   // std::cout <<t->getName() << std::endl;
  }
}


void ApplicationSolar::renderPlanets() const {

  for(auto planet : scenegraphList_){
    if(planet->getChildList().empty()){

      glm::mat4 planetMatrix = planet->getLocalTransform();
      auto parent = planet->getParent();
      //int depth = 2;

      if(planet->getIsMoon() == true){
        auto earthH = parent->getParent();
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * earthH->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, earthH->getDistance()+0.2});
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
      } else {
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
      }
      //planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * planet->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
      planetMatrix = glm::scale(planetMatrix, glm::vec3(planet->getSize(), planet->getSize(), planet->getSize()));

      /*while(depth>0 && parent->getParent()!=nullptr){
       // std::cout << planet->getName() << "\n";
       // std::cout << "depth: " << depth << "\n";
       // std::cout << "parent: " << parent->getName() << "\n" <<std::endl;
        if(depth == 1){
         // std::cout << "parent in depth 1: " << parent->getName() << "\n" <<std::endl;
          planetMatrix = planet->getLocalTransform();
          planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
          planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
          planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * planet->getParent()->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
          planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, planet->getParent()->getDistance()+0.2});
          parent = parent->getParent();
          --depth;
        }else{
        planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * parent->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
        planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, parent->getDistance()+0.2});
        parent = parent->getParent();
        --depth;
        }*/
      //}
      //planetMatrix = glm::rotate(planetMatrix, float(glfwGetTime()) * planet->getRotationSpeed()/2, glm::vec3{0.0f, 1.0f, 0.0f}); 
      //planetMatrix = glm::translate(planetMatrix, glm::vec3{0.0f, 0.0f, planet->getDistance()+0.2});
      //planetMatrix = glm::scale(planetMatrix, glm::vec3(planet->getSize(), planet->getSize(), planet->getSize()));

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

  //have to do the same for stars
  //store in container
  m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/star.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/star.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
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
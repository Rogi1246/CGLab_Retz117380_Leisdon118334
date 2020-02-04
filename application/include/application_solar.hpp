#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "node.hpp"
#include "scenegraph.hpp"
#include "geometry_node.hpp"
#include "point_light_node.hpp"
#include "texture_loader.hpp"

#include <vector>

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializeStarGeometry();

  // need to initialize the FrameBuffer
  void initializeFrameBuffer();
  // and the ScreenQuad
  void initializeScreenquad();
  // update uniform values
  void uploadUniforms();

  void initializeScenegraph();

  void renderPlanets() const;
  void renderStars() const;
  // upload projection matrix
  void uploadProjection();
  void loadTextures();
  // now adding Skybox methods
  void initializeSkybox();
  void drawSkybox() const;
  void initializeTextures();
  // upload view matrix
  void uploadView();

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  // add skybox as model
  model_object skybox_object;
  // add screenquad object
  model_object screenquad_object;

  // Handles for Framebuffer and Renderbuffer
  GLuint fboHandle = 0;
  GLuint rendBufferHandle = 0;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
  std::vector<float> stars_;

  Scenegraph scenegraph_;
  std::list<std::shared_ptr<Node>> scenegraphList_;
  std::list<std::shared_ptr<GeometryNode>> scenegraphList_geometry_;
  std::vector<pixel_data> textures_;
  std::vector<texture_object> texObjects_;
  // need vector to hold pixel-data of skybox-tex
  std::vector<pixel_data> skybox_contain_pixdata_;
  // add skybox texture_object
  texture_object skybox_texture_obj_;
  // texture object for framebuffer and screenquad
  texture_object frameBuffer_tex_obj_;
};

#endif
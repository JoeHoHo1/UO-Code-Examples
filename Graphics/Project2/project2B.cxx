#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#define _USE_MATH_DEFINES		//this makes visual studio code happy
#include <math.h>

using std::endl;
using std::cerr;

#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale

/*** UNDEFINE FOLLOWING FOR DEFAULT RUN***/

//#define STOP            //to stop superfast rotation
#define SLOW            //to slow superfast rotation
#define PRETTY          //better shape renders and tweaked light coefficients
//#define HIGHRES         //bigger window
//#define FMA             //for the memes (decided to stop implementing part-way through)

class RenderManager;

void        SetUpDog(int, RenderManager &);
const char *GetVertexShader();
const char *GetFragmentShader();

// This file is split into four parts:
// - Part 1: code to set up spheres and cylinders
// - Part 2: a "RenderManager" module
// - Part 3: main function
// - Part 4: SetUpDog and the shader programs -- things you modify
//
// It is intended that you will only need to modify code in Part 4.
// That said, you will need functions in Part 2 and should review
// those functions.
// Further, you are encouraged to look through the entire code base.
//


//
//
// PART 1: code to set up spheres and cylinders
//
//

class Triangle
{
  public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

std::vector<Triangle> SplitTriangle(std::vector<Triangle> &list)
{
    std::vector<Triangle> output(4*list.size());
    output.resize(4*list.size());
    for (unsigned int i = 0 ; i < list.size() ; i++)
    {
        Triangle t = list[i];
        glm::vec3 vmid1, vmid2, vmid3;
        vmid1 = (t.v0 + t.v1) / 2.0f;
        vmid2 = (t.v1 + t.v2) / 2.0f;
        vmid3 = (t.v0 + t.v2) / 2.0f;
        output[4*i+0].v0 = t.v0;
        output[4*i+0].v1 = vmid1;
        output[4*i+0].v2 = vmid3;
        output[4*i+1].v0 = t.v1;
        output[4*i+1].v1 = vmid2;
        output[4*i+1].v2 = vmid1;
        output[4*i+2].v0 = t.v2;
        output[4*i+2].v1 = vmid3;
        output[4*i+2].v2 = vmid2;
        output[4*i+3].v0 = vmid1;
        output[4*i+3].v1 = vmid2;
        output[4*i+3].v2 = vmid3;
    }
    return output;
}

void PushVertex(std::vector<float>& coords,
                const glm::vec3& v)
{
  coords.push_back(v.x);
  coords.push_back(v.y);
  coords.push_back(v.z);
}

//
// Sets up a cylinder that is the circle x^2+y^2=1 extruded from
// Z=0 to Z=1.
//
void GetCylinderData(std::vector<float>& coords, std::vector<float>& normals)
{
#ifdef PRETTY
    int nfacets = 450;
#else
  int nfacets = 30;
#endif
  for (int i = 0 ; i < nfacets ; i++)
  {
    double angle = 3.14159*2.0*i/nfacets;
    double nextAngle = (i == nfacets-1 ? 0 : 3.14159*2.0*(i+1)/nfacets);
    glm::vec3 fnormal(0.0f, 0.0f, 1.0f);
    glm::vec3 bnormal(0.0f, 0.0f, -1.0f);
    glm::vec3 fv0(0.0f, 0.0f, 1.0f);
    glm::vec3 fv1(cos(angle), sin(angle), 1);
    glm::vec3 fv2(cos(nextAngle), sin(nextAngle), 1);
    glm::vec3 bv0(0.0f, 0.0f, 0.0f);
    glm::vec3 bv1(cos(angle), sin(angle), 0);
    glm::vec3 bv2(cos(nextAngle), sin(nextAngle), 0);
    // top and bottom circle vertices
    PushVertex(coords, fv0);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv1);
    PushVertex(normals, fnormal);
    PushVertex(coords, fv2);
    PushVertex(normals, fnormal);
    PushVertex(coords, bv0);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv1);
    PushVertex(normals, bnormal);
    PushVertex(coords, bv2);
    PushVertex(normals, bnormal);
    // curves surface vertices
    glm::vec3 v1normal(cos(angle), sin(angle), 0);
    glm::vec3 v2normal(cos(nextAngle), sin(nextAngle), 0);
    //fv1 fv2 bv1
    PushVertex(coords, fv1);
    PushVertex(normals, v1normal);
    PushVertex(coords, fv2);
    PushVertex(normals, v2normal);
    PushVertex(coords, bv1);
    PushVertex(normals, v1normal);
    //fv2 bv1 bv2
    PushVertex(coords, fv2);
    PushVertex(normals, v2normal);
    PushVertex(coords, bv1);
    PushVertex(normals, v1normal);
    PushVertex(coords, bv2);
    PushVertex(normals, v2normal);
  }
}

//
// Sets up a sphere with equation x^2+y^2+z^2=1
//
void
GetSphereData(std::vector<float>& coords, std::vector<float>& normals)
{
    int recursionLevel =
#ifdef PRETTY    
        5;
#else
        3;
#endif

  std::vector<Triangle> list;
  {
    Triangle t;
    t.v0 = glm::vec3(1.0f,0.0f,0.0f);
    t.v1 = glm::vec3(0.0f,1.0f,0.0f);
    t.v2 = glm::vec3(0.0f,0.0f,1.0f);
    list.push_back(t);
  }
  for (int r = 0 ; r < recursionLevel ; r++)
  {
      list = SplitTriangle(list);
  }

  for (int octant = 0 ; octant < 8 ; octant++)
  {
    glm::mat4 view(1.0f);
    float angle = 90.0f*(octant%4);
    if(angle != 0.0f)
      view = glm::rotate(view, glm::radians(angle), glm::vec3(1, 0, 0));
    if (octant >= 4)
      view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0, 0, 1));
    for(int i = 0; i < list.size(); i++)
    {
      Triangle t = list[i];
      float mag_reci;
      glm::vec3 v0 = view*glm::vec4(t.v0, 1.0f);
      glm::vec3 v1 = view*glm::vec4(t.v1, 1.0f);
      glm::vec3 v2 = view*glm::vec4(t.v2, 1.0f);
      mag_reci = 1.0f / glm::length(v0);
      v0 = glm::vec3(v0.x * mag_reci, v0.y * mag_reci, v0.z * mag_reci);
      mag_reci = 1.0f / glm::length(v1);
      v1 = glm::vec3(v1.x * mag_reci, v1.y * mag_reci, v1.z * mag_reci);
      mag_reci = 1.0f / glm::length(v2);
      v2 = glm::vec3(v2.x * mag_reci, v2.y * mag_reci, v2.z * mag_reci);
      PushVertex(coords, v0);
      PushVertex(coords, v1);
      PushVertex(coords, v2);
      PushVertex(normals, v0);
      PushVertex(normals, v1);
      PushVertex(normals, v2);
    }
  }
}


//
//
// PART 2: RenderManager module
//
//

void _print_shader_info_log(GLuint shader_index) {
  int max_length = 2048;
  int actual_length = 0;
  char shader_log[2048];
  glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
  printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

class RenderManager
{
  public:
   enum ShapeType
   {
      SPHERE,
      CYLINDER
   };

                 RenderManager();
   void          SetView(glm::vec3 &c, glm::vec3 &, glm::vec3 &);
   void          SetUpGeometry();
   void          SetColor(double r, double g, double b);
   void          Render(ShapeType, glm::mat4 model);
   GLFWwindow   *GetWindow() { return window; };

  private:
   glm::vec3 color;
   GLuint sphereVAO;
   GLuint sphereNumPrimitives;
   GLuint cylinderVAO;
   GLuint cylinderNumPrimitives;
   GLuint mvploc;
   GLuint colorloc;
   GLuint camloc;
   GLuint ldirloc;
   glm::mat4 projection;
   glm::mat4 view;
   GLuint shaderProgram;
   GLFWwindow *window;

   void SetUpWindowAndShaders();
   void MakeModelView(glm::mat4 &);
};

RenderManager::RenderManager()
{
  SetUpWindowAndShaders();
  SetUpGeometry();
  projection = glm::perspective(
        glm::radians(45.0f), (float)1000 / (float)1000,  5.0f, 100.0f);

  // Get a handle for our MVP and color uniforms
  mvploc = glGetUniformLocation(shaderProgram, "MVP");
  colorloc = glGetUniformLocation(shaderProgram, "color");
  camloc = glGetUniformLocation(shaderProgram, "cameraloc");
  ldirloc = glGetUniformLocation(shaderProgram, "lightdir");

#ifdef PRETTY
  glm::vec4 lightcoeff(0.3, 0.7, 0.91, 3.5); // Lighting coeff, Ka, Kd, Ks, alpha
#else
  glm::vec4 lightcoeff(0.3, 0.7, 2.8, 50.5); // Lighting coeff, Ka, Kd, Ks, alpha
#endif
  GLuint lcoeloc = glGetUniformLocation(shaderProgram, "lightcoeff");
  glUniform4fv(lcoeloc, 1, &lightcoeff[0]);
}

void
RenderManager::SetView(glm::vec3 &camera, glm::vec3 &origin, glm::vec3 &up)
{ 
   glm::mat4 v = glm::lookAt(
                       camera, // Camera in world space
                       origin, // looks at the origin
                       up      // and the head is up
                 );
   view = v; 
   glUniform3fv(camloc, 1, &camera[0]);
   // Direction of light
   glm::vec3 lightdir = glm::normalize(camera - origin);   
   glUniform3fv(ldirloc, 1, &lightdir[0]);
};

void
RenderManager::SetUpWindowAndShaders()
{
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef HIGHRES
  window = glfwCreateWindow(1080, 1080, "CIS 441", NULL, NULL);
#else
  window = glfwCreateWindow(700, 700, "CIS 441", NULL, NULL);
#endif
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte *version = glGetString(GL_VERSION);   // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  const char* vertex_shader = GetVertexShader();
  const char* fragment_shader = GetFragmentShader();

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  int params = -1;
  glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
    _print_shader_info_log(vs);
    exit(EXIT_FAILURE);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
  if (GL_TRUE != params) {
    fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
    _print_shader_info_log(fs);
    exit(EXIT_FAILURE);
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, fs);
  glAttachShader(shaderProgram, vs);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
}

void RenderManager::SetColor(double r, double g, double b)
{
   color[0] = r;
   color[1] = g;
   color[2] = b;
}

void RenderManager::MakeModelView(glm::mat4 &model)
{
   glm::mat4 modelview = projection * view * model;
   glUniformMatrix4fv(mvploc, 1, GL_FALSE, &modelview[0][0]);
}

void RenderManager::Render(ShapeType st, glm::mat4 model)
{
   int numPrimitives = 0;
   if (st == SPHERE)
   {
      glBindVertexArray(sphereVAO);
      numPrimitives = sphereNumPrimitives;
   }
   else if (st == CYLINDER)
   {
      glBindVertexArray(cylinderVAO);
      numPrimitives = cylinderNumPrimitives;
   }
   MakeModelView(model);
   glUniform3fv(colorloc, 1, &color[0]);
   glDrawElements(GL_TRIANGLES, numPrimitives, GL_UNSIGNED_INT, NULL);
}

void SetUpVBOs(std::vector<float> &coords, std::vector<float> &normals,
               GLuint &points_vbo, GLuint &normals_vbo, GLuint &index_vbo)
{
  int numIndices = coords.size()/3;
  std::vector<GLuint> indices(numIndices);
  for(int i = 0; i < numIndices; i++)
    indices[i] = i;

  points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW);

  normals_vbo = 0;
  glGenBuffers(1, &normals_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

  index_vbo = 0;    // Index buffer object
  glGenBuffers(1, &index_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void RenderManager::SetUpGeometry()
{
  std::vector<float> sphereCoords;
  std::vector<float> sphereNormals;
  GetSphereData(sphereCoords, sphereNormals);
  sphereNumPrimitives = sphereCoords.size() / 3;
  GLuint sphere_points_vbo, sphere_normals_vbo, sphere_indices_vbo;
  SetUpVBOs(sphereCoords, sphereNormals, 
            sphere_points_vbo, sphere_normals_vbo, sphere_indices_vbo);

  std::vector<float> cylCoords;
  std::vector<float> cylNormals;
  GetCylinderData(cylCoords, cylNormals);
  cylinderNumPrimitives = cylCoords.size() / 3;
  GLuint cyl_points_vbo, cyl_normals_vbo, cyl_indices_vbo;
  SetUpVBOs(cylCoords, cylNormals, 
            cyl_points_vbo, cyl_normals_vbo, cyl_indices_vbo);

  GLuint vao[2];
  glGenVertexArrays(2, vao);

  glBindVertexArray(vao[SPHERE]);
  sphereVAO = vao[SPHERE];
  glBindBuffer(GL_ARRAY_BUFFER, sphere_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_normals_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(vao[CYLINDER]);
  cylinderVAO = vao[CYLINDER];
  glBindBuffer(GL_ARRAY_BUFFER, cyl_points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, cyl_normals_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cyl_indices_vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

//
// PART3: main function
//
int main() 
{
  RenderManager rm;
  GLFWwindow *window = rm.GetWindow();

  glm::vec3 origin(0, 0, 0);
  glm::vec3 up(0, 1, 0);

  int counter=0;
  while (!glfwWindowShouldClose(window)) 
  {
#ifdef STOP
    double angle = 0;
    #else
        #ifdef SLOW
        double angle = counter / 30000.0 * 2 * M_PI;
        #else
        double angle=counter/300.0*2*M_PI;
    #endif
#endif
    counter++;

    glm::vec3 camera(10*sin(angle), 0, 10*cos(angle));
    rm.SetView(camera, origin, up);

    // wipe the drawing surface clear
    glClearColor(0.3, 0.3, 0.8, 1.0);
#ifdef PRETTY
    glClearColor(0.1, 0.5, 0.5, 1.0);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetUpDog(counter, rm);

    // update other events like input handling
    glfwPollEvents();
    // put the stuff we've been drawing onto the display
    glfwSwapBuffers(window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}

glm::mat4 RotateMatrix(float degrees, float x, float y, float z)
{
   glm::mat4 identity(1.0f);
   glm::mat4 rotation = glm::rotate(identity, 
                                    glm::radians(degrees), 
                                    glm::vec3(x, y, z));
   return rotation;
}

glm::mat4 ScaleMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 scale(x, y, z);
   return glm::scale(identity, scale);
}

glm::mat4 TranslateMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 translate(x, y, z);
   return glm::translate(identity, translate);
}

void SetUpEyeball(glm::mat4 modelSoFar, RenderManager &rm)
{
   glm::mat4 scaled10 = ScaleMatrix(0.1, 0.1, 0.1);
   rm.SetColor(1,1,1);
#ifdef FMA
   rm.SetColor(0, 0, 0);
#endif
   rm.Render(RenderManager::SPHERE, modelSoFar*scaled10);

   rm.SetColor(.87, .76, .53);
   glm::mat4 translateBrow = TranslateMatrix(0, 0.2, -0.2);
   //glm::mat4 scaledBrow = ScaleMatrix(0.8, 0.8, 0.6);
   rm.Render(RenderManager::SPHERE, modelSoFar * scaled10 * translateBrow /* scaledBrow*/);
   
   rm.SetColor(.1, 0.05, 0);
#ifdef FMA
   rm.SetColor(1, 1, 1);
#endif
   glm::mat4 translate = TranslateMatrix(0, 0, 0.55);
   glm::mat4 scaled60 = ScaleMatrix(0.8, 0.8, 0.6);
   rm.Render(RenderManager::SPHERE, modelSoFar * scaled10 * translate * scaled60);

#ifndef FMA
   glm::mat4 translate2 = TranslateMatrix(0, 0, .9);
   glm::mat4 scaled30 = ScaleMatrix(0.5, 0.5, 0.3);
   rm.SetColor(0, 0, 0);
   rm.Render(RenderManager::SPHERE, modelSoFar * scaled10 * translate2 * scaled30);
#endif
}

void SetUpEar(glm::mat4 modelSoFar, RenderManager& rm)
{
    glm::mat4 scaled10 = ScaleMatrix(0.05, 0.5, 0.3);
    rm.SetColor(.67, .56, .43);
    rm.Render(RenderManager::SPHERE, modelSoFar * scaled10);
}

void SetUpHead(glm::mat4 modelSoFar, RenderManager &rm)
{
   // place center of head at X=0, Y=1, Z=3
   glm::mat4 translate = TranslateMatrix(0, 1, 3);

   glm::mat4 leftEyeTranslate = TranslateMatrix(-0.2, -0.25, -.735);
   SetUpEyeball(modelSoFar*translate*leftEyeTranslate, rm);

   glm::mat4 rightEyeTranslate = TranslateMatrix(0.2, -0.25, -.735);
   SetUpEyeball(modelSoFar*translate*rightEyeTranslate, rm);

   glm::mat4 rightEarTranslate = TranslateMatrix(0.575, -1.6, -1.2);
   glm::mat4 rotateEar = RotateMatrix(-30, 1, 0, 0);
   SetUpEar(modelSoFar * rotateEar * translate * rightEarTranslate , rm);

   glm::mat4 leftEarTranslate = TranslateMatrix(-0.575, -1.6, -1.2);
   SetUpEar(modelSoFar * rotateEar * translate * leftEarTranslate, rm);

   rm.SetColor(.87, .76, .53);
   glm::mat4 scaledHead = ScaleMatrix(0.6, 0.6, 0.6);
   glm::mat4 headTranslate = TranslateMatrix(0, -.1, 0);
   rm.Render(RenderManager::SPHERE, scaledHead * modelSoFar * translate * headTranslate);

   glm::mat4 snoutTranslate = TranslateMatrix(-.025, -.575, -0.635);
   glm::mat4 rotateSnout = RotateMatrix(45, -.4, 0, 1);
   glm::mat4 scaledSnoutInner = ScaleMatrix(0.1, 0.1, .5);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * snoutTranslate * rotateSnout * scaledSnoutInner);

   glm::mat4 snoutTranslate2 = TranslateMatrix(.025, -.575, -0.635);
   glm::mat4 rotateSnout2 = RotateMatrix(-45, .4, 0, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * snoutTranslate2 * rotateSnout2 * scaledSnoutInner);

   glm::mat4 snoutTranslateOuter = TranslateMatrix(-.18, -.5, -0.725);
   glm::mat4 scaledSnout = ScaleMatrix(0.2, 0.1, .5);
   glm::mat4 rotateSnoutOuter = RotateMatrix(65, 0, 0, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * snoutTranslateOuter * rotateSnoutOuter * scaledSnout);

   glm::mat4 snoutTranslateOuter2 = TranslateMatrix(.18, -.5, -0.725);
   glm::mat4 rotateSnoutOuter2 = RotateMatrix(-65, 0, 0, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * snoutTranslateOuter2 * rotateSnoutOuter2 * scaledSnout);

   glm::mat4 translateLeftJowl = TranslateMatrix(.0875, -.51, -.21);
   glm::mat4 scaledLeftJowl = ScaleMatrix(0.1125, 0.075, .03);
   glm::mat4 rotateLeftJowl = RotateMatrix(20, .05, 2, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftJowl * rotateLeftJowl * scaledLeftJowl);

   glm::mat4 translateRightJowl = TranslateMatrix(-.0875, -.51, -.21);
   glm::mat4 scaledRightJowl = ScaleMatrix(0.1125, 0.075, .03);
   glm::mat4 rotateRightJowl = RotateMatrix(-20, -.05, 2, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightJowl * rotateRightJowl * scaledRightJowl);

   glm::mat4 bridgeTranslate = TranslateMatrix(0, -.38, -0.685);
   glm::mat4 rotateBridge = RotateMatrix(-12.5, -1, 0, 0);
   glm::mat4 scaledBridge = ScaleMatrix(0.21, 0.1088, .40);
   rm.Render(RenderManager::CYLINDER, modelSoFar * translate * bridgeTranslate * rotateBridge * scaledBridge);

   glm::mat4 fillTranslate = TranslateMatrix(0, -.44, -0.33);
   glm::mat4 rotateFill = RotateMatrix(-40, -1, 0, 0);
   glm::mat4 scaledFill = ScaleMatrix(0.21, 0.1, .08);
   rm.Render(RenderManager::CYLINDER, modelSoFar * translate * fillTranslate * rotateFill * scaledFill);

   rm.SetColor(.05, .05, .00);

   glm::mat4 translateGumsUpper = TranslateMatrix(0, -.57, -0.6);
   glm::mat4 rotateGumsUpper = RotateMatrix(-90, -1, 0, 0);
   glm::mat4 scaledGumsUpper = ScaleMatrix(0.24, 0.4, .05);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateGumsUpper * rotateGumsUpper * scaledGumsUpper);

   glm::mat4 translateLeftJowlInner = TranslateMatrix(.085, -.51, -.211);
   glm::mat4 scaledLeftJowlInner = ScaleMatrix(0.1125, 0.075, .03);
   glm::mat4 rotateLeftJowlInner = RotateMatrix(20, .05, 2, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftJowlInner * rotateLeftJowlInner * scaledLeftJowlInner);

   glm::mat4 translateRightJowlInner = TranslateMatrix(-.085, -.51, -.211);
   glm::mat4 scaledRightJowlInner = ScaleMatrix(0.1125, 0.075, .03);
   glm::mat4 rotateRightJowlInner = RotateMatrix(-20, -.05, 2, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightJowlInner * rotateRightJowlInner * scaledRightJowlInner);

   glm::mat4 RightLipTranslate = TranslateMatrix(-.17, -.5, -0.725);
   glm::mat4 scaledLip = ScaleMatrix(0.2, 0.1, .5);
   glm::mat4 rotateRightLip = RotateMatrix(65, 0, 0, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * RightLipTranslate * rotateRightLip * scaledLip);

   glm::mat4 LeftLipTranslate = TranslateMatrix(.17, -.5, -0.725);
   glm::mat4 rotateLeftLip = RotateMatrix(-65, 0, 0, 1);
   rm.Render(RenderManager::SPHERE, modelSoFar * translate * LeftLipTranslate * rotateLeftLip * scaledLip);

   glm::mat4 noseTranslate = TranslateMatrix(0, -.41, -0.25);
   glm::mat4 rotateNose = RotateMatrix(-25, -1, 0, 0);
   glm::mat4 scaledNose = ScaleMatrix(0.1, 0.05, .1);
   rm.Render(RenderManager::CYLINDER, modelSoFar * translate * noseTranslate * rotateNose * scaledNose);
}

void SetUpBody(glm::mat4 modelSoFar, RenderManager& rm)
{
    // place center of body at X=0, Y=0, Z=2
    glm::mat4 translate = TranslateMatrix(0, 0, 2);

    rm.SetColor(.87, .76, .53);
    glm::mat4 neckTranslate = TranslateMatrix(0, -.3, -0.45);
    glm::mat4 rotateNeck = RotateMatrix(-75, 1, 0, 0);
    glm::mat4 scaledNeck = ScaleMatrix(.55, .55, 1);
    rm.Render(RenderManager::CYLINDER, modelSoFar * translate * neckTranslate * rotateNeck * scaledNeck);

    glm::mat4 translateBody = TranslateMatrix(0, -.65, -.8);
    glm::mat4 rotateBody = RotateMatrix(110, 1, 0, 0);
    glm::mat4 scaledBody = ScaleMatrix(.85, .75, 1.9);
    rm.Render(RenderManager::CYLINDER, modelSoFar * translate * translateBody * rotateBody * scaledBody);

    glm::mat4 translateButt = TranslateMatrix(0, -2.5, -1.5);
    glm::mat4 rotateButt = RotateMatrix(20, 1, 0, 0);
    glm::mat4 scaledButt = ScaleMatrix(.9, .45, .8);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateButt * rotateButt * scaledButt);

    rm.SetColor(1, 1, .9);
    glm::mat4 translateBelly = TranslateMatrix(0, -2, -.8);
    glm::mat4 rotateBelly = RotateMatrix(15, 1, 0, 0);
    glm::mat4 scaledBelly = ScaleMatrix(.7, 1, .4);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateBelly * rotateBelly * scaledBelly);

    
}

void SetUpLegs(glm::mat4 modelSoFar, RenderManager& rm)
{
    // place center of body at X=0, Y=0, Z=2
    glm::mat4 translate = TranslateMatrix(0, 0, 2);

    rm.SetColor(.87, .76, .53);
    glm::mat4 translateLeftThigh = TranslateMatrix(-.75, -2.5, -.9);
    glm::mat4 rotateLeftThigh = RotateMatrix(10, 0, 0, 1);
    glm::mat4 scaledLeftThigh = ScaleMatrix(.2, .4, 1);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftThigh * rotateLeftThigh * scaledLeftThigh);

    glm::mat4 translateRightThigh = TranslateMatrix(.75, -2.5, -.9);
    glm::mat4 rotateRightThigh = RotateMatrix(-10, 0, 0, 1);
    glm::mat4 scaledRightThigh = ScaleMatrix(.2, .4, 1);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightThigh * rotateRightThigh * scaledRightThigh);

    glm::mat4 translateLeftCalf = TranslateMatrix(-.8, -2.75, -.7);
    glm::mat4 rotateLeftCalf = RotateMatrix(15, -1, 0, 0);
    glm::mat4 scaledLeftCalf = ScaleMatrix(.15, .3, .7);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftCalf * rotateLeftCalf * scaledLeftCalf);

    glm::mat4 translateRightCalf = TranslateMatrix(.8, -2.75, -.7);
    glm::mat4 rotateRightCalf = RotateMatrix(15, -1, 0, 0);
    glm::mat4 scaledRightCalf = ScaleMatrix(.15, .3, .7);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightCalf * rotateRightCalf * scaledRightCalf);

    glm::mat4 translateLeftFoot = TranslateMatrix(-.8, -3, -.65);
    glm::mat4 scaledLeftFoot = ScaleMatrix(.2, .1, .6);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftFoot * scaledLeftFoot);

    glm::mat4 translateRightFoot = TranslateMatrix(.8, -3, -.65);
    glm::mat4 scaledRightFoot = ScaleMatrix(.2, .1, .6);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightFoot * scaledRightFoot);

    glm::mat4 translateLeftBicep = TranslateMatrix(-.65, -1, -.3);
    glm::mat4 rotateLeftBicep = RotateMatrix(10, 0, 0, .5);
    glm::mat4 scaledLeftBicep = ScaleMatrix(.2, .7, .3);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftBicep * rotateLeftBicep * scaledLeftBicep);

    glm::mat4 translateRightBicep = TranslateMatrix(.65, -1, -.3);
    glm::mat4 rotateRightBicep = RotateMatrix(-10, 0, 0, .5);
    glm::mat4 scaledRightBicep = ScaleMatrix(.2, .7, .3);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightBicep * rotateRightBicep * scaledRightBicep);

    glm::mat4 translateLeftForearm = TranslateMatrix(-.45, -2, -.2);
    glm::mat4 rotateLeftForearm = RotateMatrix(15, -1, 0, 1);
    glm::mat4 scaledLeftForearm = ScaleMatrix(.2, 1, .2);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftForearm * rotateLeftForearm * scaledLeftForearm);

    glm::mat4 translateRightForearm = TranslateMatrix(.45, -2, -.2);
    glm::mat4 rotateRightForearm = RotateMatrix(-15, 1, 0, 1);
    glm::mat4 scaledRightForearm = ScaleMatrix(.2, 1, .2);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightForearm * rotateRightForearm * scaledRightForearm);

    glm::mat4 translateLeftHand = TranslateMatrix(-.3, -3, 0.3);
    glm::mat4 scaledLeftHand = ScaleMatrix(.2, .1, .35);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftHand * scaledLeftHand);

    glm::mat4 translateRightHand = TranslateMatrix(.3, -3, 0.3);
    glm::mat4 scaledRightHand = ScaleMatrix(.2, .1, .35);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightHand * scaledRightHand);

    glm::mat4 translateLeftWrist = TranslateMatrix(-.3, -2.8, 0);
    glm::mat4 rotateLeftWrist = RotateMatrix(40, -1, 0, 0.1);
    glm::mat4 scaledLeftWrist = ScaleMatrix(.15, .35, .15);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateLeftWrist * rotateLeftWrist * scaledLeftWrist);

    glm::mat4 translateRightWrist = TranslateMatrix(.3, -2.8, 0);
    glm::mat4 rotateRightWrist = RotateMatrix(-40, 1, 0, 0.1);
    glm::mat4 scaledRightWrist = ScaleMatrix(.15, .35, .15);
    rm.Render(RenderManager::SPHERE, modelSoFar * translate * translateRightWrist * rotateRightWrist * scaledRightWrist);

    
}

void
SetUpDog(int counter, RenderManager& rm)
{
    glm::mat4 identity(1.0f);

    double var = (counter % 100) / 99.0;
    if ((counter / 100 % 2) == 1)
        var = 1 - var;

    SetUpHead(identity, rm);
    SetUpBody(identity, rm);
    SetUpLegs(identity, rm);

    //tongue
    rm.SetColor(.82, .56, .56);
    glm::mat4 translate = TranslateMatrix(0, 1, 3);
    glm::mat4 translateTongue = TranslateMatrix(0, -.65, -0.625 + (var * .075));
    glm::mat4 rotateTongue = RotateMatrix(-90 - (var * 10), -1, 0, 0);
    glm::mat4 scaledTongue = ScaleMatrix(0.2, 0.4, .05);
    rm.Render(RenderManager::SPHERE, translate * translateTongue * rotateTongue * scaledTongue);

    //jaw
    rm.SetColor(.05, .05, .00);
    glm::mat4 translateGumsLower = TranslateMatrix(0, -.68, -0.65);
    glm::mat4 rotateGumsLower = RotateMatrix(-90 - (var * 3), -1, 0, 0);
    glm::mat4 scaledGumsLower = ScaleMatrix(0.24, 0.4, .05);
    rm.Render(RenderManager::SPHERE, translate * translateGumsLower * rotateGumsLower * scaledGumsLower);

    rm.SetColor(.87, .76, .53);
    glm::mat4 jawTranslate = TranslateMatrix(0, -.7, -0.65);
    glm::mat4 rotateJaw = RotateMatrix(-90 - (var * 3), -1, 0, 0);
    glm::mat4 scaledJaw = ScaleMatrix(0.25, 0.4, .05);
    rm.Render(RenderManager::SPHERE, translate * jawTranslate * rotateJaw * scaledJaw);
    
    //tail
    translate = TranslateMatrix(0, 0, 2);
    glm::mat4 translateTail = TranslateMatrix(.1 - (.2 * var), -2.2, -2.2);
    glm::mat4 rotateTail = RotateMatrix(-30, 1, 1 - (2 * var), 0);
    glm::mat4 scaledTail = ScaleMatrix(.25, .15, .8);
    rm.Render(RenderManager::SPHERE, translate * translateTail * rotateTail * scaledTail);

    glm::mat4 translateTail2 = TranslateMatrix(.5 - (1 * var), -2.55 + (abs(.5 - var) * .15), -3.2);
    glm::mat4 rotateTail2 = RotateMatrix(-30 + (60 * var), 0, 1, 0);
    glm::mat4 scaledTail2 = ScaleMatrix(.15, .1, .5);
    rm.Render(RenderManager::SPHERE, translate * translateTail2 * rotateTail2 * scaledTail2);

    glm::mat4 translateTail3 = TranslateMatrix(.8 -(1.6 * var), -2.4 + (abs(.5 - var) * .15), -3.9);
    glm::mat4 rotateTail3 = RotateMatrix(30, 1, -1 + (2 * var), 0);
    glm::mat4 scaledTail3 = ScaleMatrix(.1, .1, .4);
    rm.Render(RenderManager::SPHERE,translate * translateTail3 * rotateTail3 * scaledTail3);

    //chest
    glm::mat4 translateChest = TranslateMatrix(0, -.7, -0.7 - (var * .015));
    glm::mat4 rotateChest = RotateMatrix(20, 1, 0, 0);
    glm::mat4 scaledChest = ScaleMatrix(.9, 1 - (var * .03), .9);
    rm.Render(RenderManager::SPHERE,  translate * translateChest * rotateChest * scaledChest);
}


    
const char *GetVertexShader()
{
   static char vertexShader[1024];
   strcpy(vertexShader, 
        "#version 400\n"
        "layout (location = 0) in vec3 vertex_position;\n"
        "layout (location = 1) in vec3 vertex_normal;\n"
        "uniform mat4 MVP;\n"
        "uniform vec3 cameraloc;  // Camera position \n"
        "uniform vec3 lightdir;   // Lighting direction \n"
        "uniform vec4 lightcoeff; // Lighting coeff, Ka, Kd, Ks, alpha\n"
        "out float shading_amount;\n"
        "void main() {\n"
        "  vec4 position = vec4(vertex_position, 1.0);\n"
        "  gl_Position = MVP*position;\n"
        "  vec3 L = normalize(cameraloc);\n"
        "  vec3 N = vertex_normal;\n"
        "  float LN = dot(L,N);\n"
        "  vec3 v = normalize(cameraloc - vertex_position);\n"
        "  vec3 r = ((2 * LN * N) - L);\n"
        "  float RV = max(0.0, dot(r, v));\n"
        "  float diff = max(0.0, lightcoeff.y * LN);\n"
        "  float spec = max(0.0, lightcoeff.z * pow(RV, lightcoeff.w));\n"
        "  shading_amount = (lightcoeff.x + diff + spec);\n"
        "}\n"
    );
   return vertexShader;
}

const char *GetFragmentShader()
{
   static char fragmentShader[1024];
   strcpy(fragmentShader,
       "#version 400\n"
       "in float shading_amount;\n"
       "uniform vec3 color;\n"
       "out vec3 frag_color;\n"
       "void main() {\n"
       "  frag_color = color * shading_amount;\n"
       "}\n"
    );
   return fragmentShader;
}


/* Lab 5 base code - transforms using local matrix functions 
   to be written by students - 
   CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#include <stdio.h>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "Texture.h"
#include "Shark.h"
#include "Bubbles.h"
#include "Utilities.h"
#include "Image.h"

using namespace std;
using namespace Eigen;

bool keyToggles[256] = { false }; // only for English keyboards!

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

// GLSL programs
shared_ptr<Program> fadePhongProg;  // Fading objs with phong lighting
shared_ptr<Program> fadeSharkPhongProg;  // Fading sharks with phong lighting
shared_ptr<Program> fadeTexPhongProg;  // Fading, textured objs with phong lighting
shared_ptr<Program> fadeWavePhongProg;  // Fading, waving objs with phong lighting
shared_ptr<Program> tex_prog;          // Render the framebuffer texture to the screen

// OBJ shapes
shared_ptr<Shape> plane;
shared_ptr<Shape> nefertiti;
shared_ptr<Shape> rock0;
shared_ptr<Shape> rock1;
shared_ptr<Shape> rock2;
shared_ptr<Shape> rock3;
shared_ptr<Shape> seaweed;
shared_ptr<Shape> nautilus;
shared_ptr<Shape> submarine;
shared_ptr<Shape> wreck;

// Wrapper classes to pass multiple objs to higher order classes
shared_ptr<SharkShapes> sharkShapes;
shared_ptr<BubblesShapes> bubblesShapes;

// Textures
shared_ptr<Texture> sandTex;
shared_ptr<Texture> surfaceTex;
shared_ptr<Texture> coral0Tex;
shared_ptr<Texture> coral1Tex;
shared_ptr<Texture> coral2Tex;
shared_ptr<Texture> rustedMetalTex0;
shared_ptr<Texture> rustedMetalTex1;
shared_ptr<Texture> wreckTex;

// Object transforms handled in this thread
vector<Matrix4f> rockTransforms;
vector<Matrix4f> seaweedTransforms;
vector<Matrix4f> nautilusTransforms;
vector<Matrix4f> submarineTransforms;

// Higher order obj classes that handle some of their own movement
Shark shark;
vector<Bubbles> bubbles;

// Camera parameters
double mouse[2];
double alpha, beta, alphaClamp;
Vector3d camPos;
Vector3d camDir;
double speed;
float viewDist;
int iter = 0;
int iterations = 720;

int g_width, g_height;
int actualW, actualH;

// These things added for ICEX
// Vector of camera positions and directions, initialized at the beginning
vector<Vector3d> camPosVec;
vector<Vector3d> camDirVec;
// Sara's path manually entered in
// Vector3d camArr[62] = {
// Vector3d(-16.00, 3.00, -16.00),
// Vector3d(-14.49, 3.00, -15.08),
// Vector3d(-15.81, 3.00, -13.96),
// Vector3d(-14.85, 3.00, -12.92),
// Vector3d(-15.79, 3.00, -11.88),
// Vector3d(-14.23, 3.00, -9.62),
// Vector3d(-13.28, 3.00, -7.36),
// Vector3d(-12.85, 3.00, -6.94),
// Vector3d(-12.13, 3.00, -4.77),
// Vector3d(-11.04, 3.00, -4.74),
// Vector3d(-9.28, 3.00, -4.28),
// Vector3d(-8.95, 3.00, -4.05),
// Vector3d(-8.59, 3.00, -4.60),
// Vector3d(-6.57, 3.00, -4.35),
// Vector3d(-4.33, 3.00, -5.41),
// Vector3d(-3.73, 3.00, -4.70),
// Vector3d(-1.15, 3.00, -4.67),
// Vector3d(-0.84, 3.00, -2.06),
// Vector3d(0.70, 3.00, -2.04),
// Vector3d(1.55, 3.00, -3.02),
// Vector3d(0.36, 3.00, -1.90),
// Vector3d(1.74, 3.00, -0.56),
// Vector3d(2.81, 3.00, 0.08),
// Vector3d(2.22, 3.00, 1.07),
// Vector3d(3.83, 3.00, 2.13),
// Vector3d(4.93, 3.00, 3.69),
// Vector3d(5.50, 3.00, 3.34),
// Vector3d(4.88, 3.00, 3.83),
// Vector3d(3.94, 3.00, 4.27),
// Vector3d(2.67, 3.00, 5.91),
// Vector3d(3.31, 3.00, 6.80),
// Vector3d(4.64, 3.00, 7.78),
// Vector3d(2.68, 3.00, 8.45),
// Vector3d(3.03, 3.00, 9.84),
// Vector3d(1.48, 3.00, 10.30),
// Vector3d(1.98, 3.00, 10.93),
// Vector3d(2.43, 3.00, 11.67),
// Vector3d(3.74, 3.00, 12.38),
// Vector3d(4.86, 3.00, 12.93),
// Vector3d(5.91, 3.00, 13.55),
// Vector3d(6.30, 3.00, 12.24),
// Vector3d(7.66, 3.00, 11.72),
// Vector3d(8.24, 3.00, 11.21),
// Vector3d(9.17, 3.00, 11.93),
// Vector3d(10.90, 3.00, 12.60),
// Vector3d(9.02, 3.00, 13.36),
// Vector3d(10.76, 3.00, 14.17),
// Vector3d(11.74, 3.00, 14.41),
// Vector3d(12.98, 3.00, 15.50),
// Vector3d(12.21, 3.00, 13.56),
// Vector3d(13.47, 3.00, 12.54),
// Vector3d(14.12, 3.00, 11.33),
// Vector3d(15.13, 3.00, 12.40),
// Vector3d(15.19, 3.00, 13.37),
// Vector3d(15.14, 3.00, 14.69),
// Vector3d(13.46, 3.00, 14.17),
// Vector3d(14.61, 3.00, 15.77),
// Vector3d(13.19, 3.00, 14.69),
// Vector3d(13.55, 3.00, 15.48),
// Vector3d(14.43, 3.00, 15.23),
// Vector3d(15.11, 3.00, 15.32),
// Vector3d(15.99, 3.00, 15.96)
// };
// framebuffer and texture to render to
GLuint framebuffer, renderTexture, depthRenderBuffer;

//geometry for texture render
GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;

//geometry for path render
GLuint path_VertexArrayID;
GLuint path_vertexbuffer;
GLfloat *path_vertices;
//GLfloat path_vertices[150] = {
//    -16.00, 3.00, -16.00,
//    -14.49, 3.00, -15.08,
//    -15.63, 3.00, -14.04,
//    -14.11, 3.00, -12.46,
//    -13.64, 3.00, -10.62,
//    -11.52, 3.00, -9.14,
//    -11.31, 3.00, -9.87,
//    -11.57, 3.00, -7.65,
//    -9.65, 3.00, -6.20,
//    -7.32, 3.00, -4.40,
//    -5.33, 3.00, -4.89,
//    -5.89, 3.00, -4.45,
//    -6.79, 3.00, -2.47,
//    -5.14, 3.00, -1.04,
//    -4.65, 3.00, -0.34,
//    -5.21, 3.00, 0.07,
//    -4.25, 3.00, 0.28,
//    -4.36, 3.00, 1.24,
//    -4.72, 3.00, 2.72,
//    -3.52, 3.00, 3.67,
//    -3.35, 3.00, 3.75,
//    -3.92, 3.00, 4.95,
//    -2.93, 3.00, 5.59,
//    -0.88, 3.00, 5.31,
//    0.38, 3.00, 6.72,
//    0.83, 3.00, 6.87,
//    1.89, 3.00, 7.35,
//    2.36, 3.00, 7.38,
//    2.84, 3.00, 8.54,
//    3.93, 3.00, 8.85,
//    3.51, 3.00, 9.24,
//    3.15, 3.00, 10.07,
//    4.21, 3.00, 11.22,
//    4.64, 3.00, 11.93,
//    5.61, 3.00, 9.55,
//    6.16, 3.00, 7.48,
//    7.45, 3.00, 7.54,
//    8.25, 3.00, 8.79,
//    7.86, 3.00, 9.14,
//    8.15, 3.00, 8.63,
//    9.91, 3.00, 9.55,
//    10.97, 3.00, 9.97,
//    11.96, 3.00, 9.69,
//    12.01, 3.00, 10.67,
//    13.11, 3.00, 11.53,
//    14.20, 3.00, 12.23,
//    14.47, 3.00, 13.98,
//    14.87, 3.00, 14.94,
//    15.45, 3.00, 15.11,
//    16.00, 3.00, 15.96
//};

GLuint path_indexbufferID;
vector<GLuint *> path_indices;
vector<GLuint> length_indices;
//
//GLuint index_list[50] = {
//    0, 1, 2, 3, 4, 5,
//    6, 7, 8, 9, 10, 11,
//    12, 13, 14, 15, 16,
//    17, 18, 19, 20, 21,
//    22, 23, 24, 25, 26,
//    27, 28, 29, 30, 31,
//    32, 33, 34, 35, 36,
//    37, 38, 39, 40, 41,
//    42, 43, 44, 45, 46,
//    47, 48, 49
//};


GLuint *IndxBuffObjs;



//for local copy of framebuffer
typedef struct RGB {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} RGB;

GLubyte myFrame[1024*768*4];

Image *outImage;

void setCamPos6dof(Vector3d pos, Vector3d dir) {
   camPos = pos;
   camDir = dir;
}

void setCamPos3dof(Vector3d pos) {
   camPos = pos;
}

// Initializes arrays to circular path around Nefertiti head
void initCamPath() {
   double radius = 15;
   double dirDelta = (2 * M_PI) / iterations;
   double offset = M_PI;

   for (int i = 0; i < iterations; i++) {
      double theta = i * dirDelta;
      camPosVec.push_back(Vector3d(radius * cos(theta), 3, radius * sin(theta)));
      camDirVec.push_back(Vector3d(cos(offset + theta), 0, -sin(theta)));
   }
   
   setCamPos6dof(camPosVec[0], camDirVec[0]);
}

// Zoe's code to write out texytre to .tga image
void writeOutTex() {
  int idx;

  //dumb conversion right now
  color_t pix;
  for (int i=0; i < actualW; i++) {
      for (int j=0; j < actualH; j++) {
         idx = (j*actualW+i)*4;
         pix.r = myFrame[idx+0];
         pix.g = myFrame[idx+1];
         pix.b = myFrame[idx+2];
         outImage->pixel(i, j, pix);
         // cout << (int)myFrame[idx+0] << " " << endl;
     }
   }
  outImage->WriteTga((char *)"out_texture.tga", true);
}

// Fill the world with underwater objects
static void generate() {
   // Clear out the existing world
   // sharks.clear();
   bubbles.clear();
   rockTransforms.clear();
   seaweedTransforms.clear();
   nautilusTransforms.clear();
   submarineTransforms.clear();

   // Initialize random sharks around Nefertiti
   Vector2f sR(8.0f, 15.0f);
   Vector2f sH(1.0f, 25.0f);
   Vector2f sS(-3.0f, 3.0f);
   float s = randRangef(sS(0), sS(1));
   if (abs(s) < 1.0f) {
      s = s / abs(s);
   }
   float h = randRangef(sH(0), sH(1));
   float r = randRangef(sR(0), sR(1));

   shark = Shark(sharkShapes);
   shark.setHeight(h);
   shark.setRadius(r);
   shark.setSpeed(s);

   // Initialize random bubbles
   int numBubbles = 600;
   bubblesShapes = make_shared<BubblesShapes>(RESOURCE_DIR);
   for (int i = 0; i < numBubbles; i++) {
      Bubbles bubble = Bubbles(bubblesShapes);
      bubbles.push_back(bubble);
   }

   // Initialize random rock transforms
   int numRock0 = 500;
   Vector2f rSC(3.0f, 10.0f);
   Vector2f rR(30.0f, 300.0f);
   for (int i = 0; i < numRock0; i++) {
      auto E = make_shared<MatrixStack>();
      E->loadIdentity();
      float sc = randRangef(rSC(0), rSC(1));
      float r = randRangef(rR(0), rR(1));
      float rot = randRangef(0.0f, 2.0f * (float)M_PI);
      float theta = randRangef(0.0f, 2.0f * (float)M_PI);

      E->translate(Vector3f(r * cos(theta), 0.0f, r * sin(theta)));
      E->rotate(rot, Vector3f(0.0f, 1.0f, 0.0f));
      E->scale(sc);

      rockTransforms.push_back(E->topMatrix());
   }

   // Initialize random seaweed transforms
   int numSeaweed = 100;
   Vector2f swSC(5.0f, 10.0f);
   Vector2f swR(15.0f, 300.0f);
   Vector2f swDR(0.0f, 0.001f);
   Vector2f swDensity(3.0f, 10.0f);
   for (int i = 0; i < numSeaweed; i++) {
      float r = randRangef(swR(0), swR(1));
      float theta = randRangef(0.0f, 2.0f * (float)M_PI);

      int num = (int)randRangef(swDensity(0), swDensity(1));
      for (int j = 0; j < num; j++) {
         float sc = randRangef(swSC(0), swSC(1));
         float rot = randRangef(0.0f, 2.0f * (float)M_PI);
         float dR = randRangef(swDR(0), swDR(1));
         float dTheta = randRangef(0.0f, 2.0f * (float)M_PI);

         auto E = make_shared<MatrixStack>();
         E->loadIdentity();
         E->translate(Vector3f(r * cos(theta), 0.0f, r * sin(theta)));
         E->translate((Vector3f(dR * cos(dTheta), 0.0f, dR * sin(dTheta))));
         E->rotate(rot, Vector3f(0.0f, 1.0f, 0.0f));
         E->scale(sc);
         seaweedTransforms.push_back(E->topMatrix());
      }
   }

   // Initialize random nautilus transforms
   int numNaut = 3;
   Vector2f nR(100.0f, 300.0f);
   Vector2f nXRot(-30.0f * (float)M_PI / 180, 30.0f * (float)M_PI / 180);
   for (int i = 0; i < numNaut; i++) {
      auto E = make_shared<MatrixStack>();
      E->loadIdentity();
      float r = randRangef(nR(0), nR(1));
      float xRot = randRangef(nXRot(0), nXRot(1));
      float yRot = randRangef(0.0f, 2.0f * (float)M_PI);
      float zRot = randRangef(0.0f, 2.0f * (float)M_PI);
      float theta = randRangef(0.0f, 2.0f * (float)M_PI);

      E->translate(Vector3f(r * cos(theta), 0.0f, r * sin(theta)));
      E->rotate(yRot, Vector3f(0.0f, 1.0f, 0.0f));
      E->rotate(xRot, Vector3f(1.0f, 0.0f, 0.0f));
      E->rotate(zRot, Vector3f(0.0f, 0.0f, 1.0f));
      E->scale(50.0f);

      nautilusTransforms.push_back(E->topMatrix());
   }

   // Initialize random submarine transforms
   int numSub = 2;
   Vector2f subR(75.0f, 300.0f);
   Vector2f subZRot(-30.0f * (float)M_PI / 180, 30.0f * (float)M_PI / 180);
   for (int i = 0; i < numSub; i++) {
      auto E = make_shared<MatrixStack>();
      E->loadIdentity();
      float r = randRangef(subR(0), subR(1));
      float xRot = randRangef(0.0f, 2.0f * (float)M_PI);
      float yRot = randRangef(0.0f, 2.0f * (float)M_PI);
      float zRot = randRangef(subZRot(0), subZRot(1));
      float theta = randRangef(0.0f, 2.0f * (float)M_PI);

      E->translate(Vector3f(r * cos(theta), 0.0f, r * sin(theta)));
      E->rotate(yRot, Vector3f(0.0f, 1.0f, 0.0f));
      E->rotate(zRot, Vector3f(0.0f, 0.0f, 1.0f));
      E->rotate(xRot, Vector3f(1.0f, 0.0f, 0.0f));
      E->scale(25.0f);

      submarineTransforms.push_back(E->topMatrix());
   }
}

static void error_callback(int error, const char *description)
{
   cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
   }

   // Move the camera around
   else if (key == GLFW_KEY_W) {
     camPos += speed * camDir;
   }
   else if (key == GLFW_KEY_A) {
     Vector3d camCross = camDir.cross(Vector3d(0.0, 1.0, 0.0)).normalized();
     camPos -= camCross;
   }
   else if (key == GLFW_KEY_S) {
     camPos -= speed * camDir;
   }
   else if (key == GLFW_KEY_D) {
     Vector3d camCross = camDir.cross(Vector3d(0.0, 1.0, 0.0)).normalized();
     camPos += camCross;
   }
   else if (key == GLFW_KEY_SPACE) {
     camPos += speed * Vector3d(0.0, 1.0, 0.0);
   }
   // Re-generate the world
   else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
      generate();
   }
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
   keyToggles[key] = !keyToggles[key];
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
     glfwGetCursorPos(window, &posX, &posY);
     mouse[0] = posX;
     mouse[1] = posY;
   }
}

static void cursor_pos_callback(GLFWwindow * window, double xpos, double ypos) {
   int mouseAction = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (mouseAction == GLFW_PRESS) {
      double posX, posY;
      glfwGetCursorPos(window, &posX, &posY);
      double dX = posX - mouse[0];
      double dY = posY - mouse[1];

      int width, height;
      glfwGetFramebufferSize(window, &width, &height);

      // Calculate the camera angles
      alpha -= 0.5 * M_PI * dY / (double)width;
      beta += M_PI * dX / (double)height;

      // Clamp alpha between +/- 85 degrees.
      if (alpha > alphaClamp) {
        alpha = alphaClamp;
      }
      else if (alpha < -alphaClamp) {
        alpha = -alphaClamp;
      }

      // Calculate the new camera direction
      camDir(0) = cos(alpha) * cos(beta);
      camDir(1) = sin(alpha);
      camDir(2) = cos(alpha) * cos(0.5 * M_PI - beta);

      // Record the x and y positions
      mouse[0] = posX;
      mouse[1] = posY;
   }
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

// Set materials to objs
void setMaterial(int i, shared_ptr<Program> prog) {

  switch (i) {
  case 0: //shiny blue plastic
    glUniform3f(prog->getUniform("matAmb"), 0.002f, 0.004f, 0.02f);
    glUniform3f(prog->getUniform("matDif"), 0.0f, 0.16f, 0.9f);
    glUniform3f(prog->getUniform("matSpec"), 0.14f, 0.2f, 0.8f);
    glUniform1f(prog->getUniform("matShine"), 120.0f);
    break;
  case 1: // flat grey
    glUniform3f(prog->getUniform("matAmb"), 0.013f, 0.013f, 0.014f);
    glUniform3f(prog->getUniform("matDif"), 0.3f, 0.3f, 0.4f);
    glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.4f);
    glUniform1f(prog->getUniform("matShine"), 4.0f);
    break;
  case 2: // brass
      glUniform3f(prog->getUniform("matAmb"), 2.0f * 0.03294f, 2.0f * 0.02235f, 2.0f * 0.002745f);
      glUniform3f(prog->getUniform("matDif"), 0.7804f, 0.5686f, 0.11373f);
      glUniform3f(prog->getUniform("matSpec"), 0.9922f, 0.941176f, 0.80784f);
      glUniform1f(prog->getUniform("matShine"), 27.9f);
      break;
  case 3: // copper
    glUniform3f(prog->getUniform("matAmb"), 0.01913f, 0.00735f, 0.00225f);
    glUniform3f(prog->getUniform("matDif"), 0.7038f, 0.27048f, 0.0828f);
    glUniform3f(prog->getUniform("matSpec"), 0.257f, 0.1376f, 0.08601f);
    glUniform1f(prog->getUniform("matShine"), 12.8f);
    break;
  case 4: // aquamarine
    glUniform3f(prog->getUniform("matAmb"), 0.0f, 0.026f, 0.013f);
    glUniform3f(prog->getUniform("matDif"), 0.0f, 0.8f, 0.7f);
    glUniform3f(prog->getUniform("matSpec"), 0.0f, 0.2f, 0.99f);
    glUniform1f(prog->getUniform("matShine"), 20.0f);
    break;
  case 5: // shark
     glUniform3f(prog->getUniform("matAmb"), 0.25098f / 3, 0.192157f / 3, 0.282353f / 3);
     glUniform3f(prog->getUniform("matDif"), 0.25098f, 0.192157f, 0.282353f);
     glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.3f);
     glUniform1f(prog->getUniform("matShine"), 5.0);
     break;
  case 6: // bubbles
     glUniform3f(prog->getUniform("matAmb"), 0.9f, 0.9f, 0.9f);
     glUniform3f(prog->getUniform("matDif"), 0.0f, 0.0f, 0.0f);
     glUniform3f(prog->getUniform("matSpec"), 0.1f, 0.1f, 0.1f);
     glUniform1f(prog->getUniform("matShine"), 20.0f);
     break;
  case 7: // seaweed
     glUniform3f(prog->getUniform("matAmb"), 0.0f, 0.2f, 0.1f);
     glUniform3f(prog->getUniform("matDif"), 0.0f, 0.4f, 0.0f);
     glUniform3f(prog->getUniform("matSpec"), 0.4f, 0.5f, 0.4f);
     glUniform1f(prog->getUniform("matShine"), 20.0f);
     break;
  }
}

// Set materials to textured objs
void setTextureMaterial(int i, shared_ptr<Program> prog) {

   switch (i) {
   case 0: // sand
      glUniform1f(prog->getUniform("matAmb"), 0.25f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.3f);
      glUniform1f(prog->getUniform("matShine"), 5.0f);
      break;
   case 1: // water surface
      glUniform1f(prog->getUniform("matAmb"), 0.25f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 2.0f, 2.0f, 2.0f);
      glUniform1f(prog->getUniform("matShine"), 10.0f);
      break;
   case 2: // coral
      glUniform1f(prog->getUniform("matAmb"), 0.25f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.3f);
      glUniform1f(prog->getUniform("matShine"), 5.0f);
      break;
   case 3: // nautilus
      glUniform1f(prog->getUniform("matAmb"), 0.25f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.3f);
      glUniform1f(prog->getUniform("matShine"), 45.0f);
      break;
   case 4: // submarine
      glUniform1f(prog->getUniform("matAmb"), 0.25f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 0.0f, 0.45f, 0.45f);
      glUniform1f(prog->getUniform("matShine"), 45.0f);
      break;
   case 5: // ICEX wreck
      glUniform1f(prog->getUniform("matAmb"), 1.0f);
      glUniform1f(prog->getUniform("matDif"), 1.0f);
      glUniform3f(prog->getUniform("matSpec"), 0.0f, 0.0f, 0.0f);
      glUniform1f(prog->getUniform("matShine"), 1.0f);
      break;
   }
}

// Parses through "path.txt" and "roadmap.txt" to get the possible paths' indices and all possible vertices.
void loadRoadMap() {
    // get array of vertices
    FILE *infoFile = fopen("../../resources/info.txt", "r");
    char num[10];
    
    fgets(num, 10, infoFile);
    GLuint numVert = atoi(num);
    printf("numVert is %d\n", numVert);
    fclose(infoFile);
    
    FILE *vertexFile = fopen("../../resources/roadmap.txt", "r");
    path_vertices = (GLfloat *) malloc(sizeof(GLfloat) * numVert * 3);
    
    int vert = 0;
    float x, z;
    float y = 3.0f;
    
    while (fscanf(vertexFile, " (%f, %f)  -  %*d", &x, &z) != EOF) {
        printf("Vertex %d: ", vert / 3);
        path_vertices[vert++] = x;
        printf("x is %.2f, ", x);
        path_vertices[vert++] = y;
        path_vertices[vert++] = z;
        printf("z is %.2f\n", z);
    }
    
    fclose(vertexFile);
    // end of getting vertices
    
    printf("Successfully loaded vertex array\n");
    
    // get array of indices
    FILE *indexFile = fopen("../../resources/paths.txt", "r");
    char cur[10];
    int count = 0;
    
    // TODO: find a way to have IBOs with varying lengths. perhaps have separate array to index
    // into that contains the lengths of each IBO?? Is there a better way? (Ask Zoe).
    while (fscanf(indexFile, " %s", cur) != EOF) {
        GLuint index;
        
        GLuint numIndices = 0;
        fscanf(indexFile, " number of indices: %d", &numIndices);
        printf("num indices is %d\n", numIndices);
        
        GLuint *indexArr = new GLuint[numIndices];
        length_indices.push_back(numIndices);
        
        fscanf(indexFile, " %s", cur);
        
        while (strcmp(cur, "end")) {
            //printf("num is %s\n", cur);
            index = atoi(cur);
            //printf("index is %d\n", count);
            
            assert(indexArr);
            indexArr[count++] = index;
            fscanf(indexFile, " %s", cur);
        }
        path_indices.push_back(indexArr);
        count = 0;
    }
    
    fclose(indexFile);
    // end of getting indices
}

static void init()
{
   GLSL::checkVersion();
    
   // Initialize camera variables
   viewDist = 300.0f;
   speed = 0.75;
   alpha = 0.0;
   alphaClamp = 85.0 * M_PI / 180.0;
   beta = 0.5 * M_PI;
   camPos = Vector3d(0.0, 1.0, -15.0);
   camDir = Vector3d(0.0, 0.0, 1.0);

   // Set background color.
//   glClearColor(.051f, .553f, .875f, 1.0f);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   // Enable z-buffer test.
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

   // Added for ICEX
   // Generate framebuffer, texture, and depth renderbuffer
   glfwGetFramebufferSize(window, &actualW, &actualH);

   glGenFramebuffers(1, &framebuffer);
   glGenTextures(1, &renderTexture);
   glGenRenderbuffers(1, &depthRenderBuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
   glBindTexture(GL_TEXTURE_2D, depthRenderBuffer);
   glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, actualW, actualH, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
   // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
    
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, actualW, actualH);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    
   GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
   glDrawBuffers(1, DrawBuffers);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      cout << "framebuffer error" << endl;
   }

   // Now set up a simple quad for rendering FBO
   glGenVertexArrays(1, &quad_VertexArrayID);
   glBindVertexArray(quad_VertexArrayID);

   static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
   };
    
   glGenBuffers(1, &quad_vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

   loadRoadMap();
    
   // Setup vao for rendering path lines
   glGenVertexArrays(1, &path_VertexArrayID);
   glBindVertexArray(path_VertexArrayID);

//   // First few points of path for testing purposes
//   static const GLfloat g_path_vertex_buffer_data[] = {
//       -16.00, 3.00, -16.00,
//       -14.49, 3.00, -15.08,
//       -15.81, 3.00, -13.96,
//       -14.85, 3.00, -12.92,
//       -15.79, 3.00, -11.88,
//       -14.23, 3.00, -9.62,
//       -13.28, 3.00, -7.36,
//       -12.85, 3.00, -6.94,
//       -12.13, 3.00, -4.77,
//       -11.04, 3.00, -4.74,
//       -9.28, 3.00, -4.28,
//       -8.95, 3.00, -4.05,
//       -8.59, 3.00, -4.60,
//       -6.57, 3.00, -4.35,
//       -4.33, 3.00, -5.41,
//       -3.73, 3.00, -4.70,
//       -1.15, 3.00, -4.67,
//       -0.84, 3.00, -2.06,
//       0.70, 3.00, -2.04,
//       1.55, 3.00, -3.02,
//       0.36, 3.00, -1.90,
//       1.74, 3.00, -0.56,
//       2.81, 3.00, 0.08,
//       2.22, 3.00, 1.07,
//       3.83, 3.00, 2.13,
//       4.93, 3.00, 3.69,
//       5.50, 3.00, 3.34,
//       4.88, 3.00, 3.83,
//       3.94, 3.00, 4.27,
//       2.67, 3.00, 5.91,
//       3.31, 3.00, 6.80,
//       4.64, 3.00, 7.78,
//       2.68, 3.00, 8.45,
//       3.03, 3.00, 9.84,
//       1.48, 3.00, 10.30,
//       1.98, 3.00, 10.93,
//       2.43, 3.00, 11.67,
//       3.74, 3.00, 12.38,
//       4.86, 3.00, 12.93,
//       5.91, 3.00, 13.55,
//       6.30, 3.00, 12.24,
//       7.66, 3.00, 11.72,
//       8.24, 3.00, 11.21,
//       9.17, 3.00, 11.93,
//       10.90, 3.00, 12.60,
//       9.02, 3.00, 13.36,
//       10.76, 3.00, 14.17,
//       11.74, 3.00, 14.41,
//       12.98, 3.00, 15.50,
//       12.21, 3.00, 13.56,
//       13.47, 3.00, 12.54,
//       14.12, 3.00, 11.33,
//       15.13, 3.00, 12.40,
//       15.19, 3.00, 13.37,
//       15.14, 3.00, 14.69,
//       13.46, 3.00, 14.17,
//       14.61, 3.00, 15.77,
//       13.19, 3.00, 14.69,
//       13.55, 3.00, 15.48,
//       14.43, 3.00, 15.23,
//       15.11, 3.00, 15.32,
//       15.99, 3.00, 15.96
//   };
    
    // setup vbo for rendering path lines
    glGenBuffers(1, &path_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, path_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(path_vertices), path_vertices, GL_STATIC_DRAW);
   //glBufferData(GL_ARRAY_BUFFER, sizeof(g_path_vertex_buffer_data), g_path_vertex_buffer_data, GL_STATIC_DRAW);
    
    //path_indices.push_back(index_list);
    //length_indices.push_back(50);
    IndxBuffObjs = new GLuint[path_indices.size()];
    
    glGenBuffers(path_indices.size(), IndxBuffObjs);
    for (int i=0; i < path_indices.size(); i++) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndxBuffObjs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(path_indices[i]), path_indices[i], GL_STATIC_DRAW);
    }

   // End of added for ICEX

   //to write out
   outImage = new Image(actualW, actualH);

   tex_prog = make_shared<Program>();
   tex_prog->setVerbose(true);
   tex_prog->setShaderNames(RESOURCE_DIR + "pass_vert.glsl", RESOURCE_DIR + "tex_frag.glsl");
   tex_prog->init();
   tex_prog->addUniform("texBuf");
   tex_prog->addAttribute("vertPos");

   // Initialize plane for textures.
   plane = make_shared<Shape>();
   plane->loadMesh(RESOURCE_DIR + "plane.obj");
   plane->resize();
   plane->init();

   // Initialize nefertiti.
   nefertiti = make_shared<Shape>();
   nefertiti->loadMesh(RESOURCE_DIR + "Nefertiti-10K.obj");
   nefertiti->resize();
   nefertiti->init();

   // Initialize rock 0.
   rock0 = make_shared<Shape>();
   rock0->loadMesh(RESOURCE_DIR + "Boulder1.obj");
   rock0->resize();
   rock0->init();

   // Initialize rock 1.
   rock1 = make_shared<Shape>();
   rock1->loadMesh(RESOURCE_DIR + "Boulder2.obj");
   rock1->resize();
   rock1->init();

   // Initialize rock 2.
   rock2 = make_shared<Shape>();
   rock2->loadMesh(RESOURCE_DIR + "Boulder3.obj");
   rock2->resize();
   rock2->init();

   // Initialize rock 3.
   rock3 = make_shared<Shape>();
   rock3->loadMesh(RESOURCE_DIR + "rockArc.obj");
   rock3->resize();
   rock3->moveToGround();
   rock3->init();

   // Initialize seaweed.
   seaweed = make_shared<Shape>();
   seaweed->loadMesh(RESOURCE_DIR + "seaweed.obj");
   seaweed->resize();
   seaweed->moveToGround();
   seaweed->init();

   // Initialize nautilus.
   nautilus = make_shared<Shape>();
   nautilus->loadMesh(RESOURCE_DIR + "nautilus.obj");
   nautilus->resize();
   nautilus->init();

   // Initialize submarine.
   submarine = make_shared<Shape>();
   submarine->loadMesh(RESOURCE_DIR + "submarine.obj");
   submarine->resize();
   submarine->init();

   // Initialize ICEX wreck
   wreck = make_shared<Shape>();
   wreck->loadMesh(RESOURCE_DIR + "chimney.obj");
   wreck->resize();
   wreck->init();

   // Initialize sand texture.
   sandTex = make_shared<Texture>();
   sandTex->setFilename(RESOURCE_DIR + "sandLight.png");
   sandTex->init();

   // Initialize ocean surface texture.
   surfaceTex = make_shared<Texture>();
   surfaceTex->setFilename(RESOURCE_DIR + "oceanSurface.png");
   surfaceTex->init();

   // Initialize coral texture 0.
   coral0Tex = make_shared<Texture>();
   coral0Tex->setFilename(RESOURCE_DIR + "coral0.png");
   coral0Tex->init();

   // Initialize coral texture 1.
   coral1Tex = make_shared<Texture>();
   coral1Tex->setFilename(RESOURCE_DIR + "coral1.png");
   coral1Tex->init();

   // Initialize coral texture 2.
   coral2Tex = make_shared<Texture>();
   coral2Tex->setFilename(RESOURCE_DIR + "coral2.png");
   coral2Tex->init();

   // Initialize first rust texture.
   rustedMetalTex0 = make_shared<Texture>();
   rustedMetalTex0->setFilename(RESOURCE_DIR + "rustedMetal0.png");
   rustedMetalTex0->init();

   // Initialize second rust texture.
   rustedMetalTex1 = make_shared<Texture>();
   rustedMetalTex1->setFilename(RESOURCE_DIR + "rustedMetal1.png");
   rustedMetalTex1->init();

   // Initalize the ICEX wreck texture
   wreckTex = make_shared<Texture>();
   wreckTex->setFilename(RESOURCE_DIR + "chimney.jpg");
   wreckTex->init();

   // Initialize shark shapes.
   sharkShapes = make_shared<SharkShapes>(RESOURCE_DIR);

   // Generate the world
   generate();

   // Initialize the fading blinn-phong program
   fadePhongProg = make_shared<Program>();
   fadePhongProg->setVerbose(true);
   fadePhongProg->setShaderNames(RESOURCE_DIR + "fading_phong_vert.glsl", RESOURCE_DIR + "fading_phong_frag.glsl");
   fadePhongProg->init();
   fadePhongProg->addUniform("P");
   fadePhongProg->addUniform("V");
   fadePhongProg->addUniform("M");
   fadePhongProg->addUniform("camPos");
   fadePhongProg->addUniform("lightPos");
   fadePhongProg->addUniform("lightCol");
   fadePhongProg->addUniform("viewDist");
   fadePhongProg->addUniform("matAmb");
   fadePhongProg->addUniform("matDif");
   fadePhongProg->addUniform("matSpec");
   fadePhongProg->addUniform("matShine");
   fadePhongProg->addUniform("baseAlpha");
   fadePhongProg->addAttribute("vertPos");
   fadePhongProg->addAttribute("vertNor");

   // Initialize the fading shark blinn-phong program
   fadeSharkPhongProg = make_shared<Program>();
   fadeSharkPhongProg->setVerbose(true);
   fadeSharkPhongProg->setShaderNames(RESOURCE_DIR + "fading_shark_phong_vert.glsl", RESOURCE_DIR + "fading_shark_phong_frag.glsl");
   fadeSharkPhongProg->init();
   fadeSharkPhongProg->addUniform("P");
   fadeSharkPhongProg->addUniform("V");
   fadeSharkPhongProg->addUniform("M");
   fadeSharkPhongProg->addUniform("camPos");
   fadeSharkPhongProg->addUniform("lightPos");
   fadeSharkPhongProg->addUniform("lightCol");
   fadeSharkPhongProg->addUniform("viewDist");
   fadeSharkPhongProg->addUniform("matAmb");
   fadeSharkPhongProg->addUniform("matDif");
   fadeSharkPhongProg->addUniform("matSpec");
   fadeSharkPhongProg->addUniform("matShine");
   fadeSharkPhongProg->addUniform("baseAlpha");
   fadeSharkPhongProg->addAttribute("vertPos");
   fadeSharkPhongProg->addAttribute("vertNor");

   // Initialize the fading, textured, blinn-phong program
   fadeTexPhongProg = make_shared<Program>();
   fadeTexPhongProg->setVerbose(true);
   fadeTexPhongProg->setShaderNames(RESOURCE_DIR + "fading_tex_phong_vert.glsl", RESOURCE_DIR + "fading_tex_phong_frag.glsl");
   fadeTexPhongProg->init();
   fadeTexPhongProg->addUniform("P");
   fadeTexPhongProg->addUniform("V");
   fadeTexPhongProg->addUniform("M");
   fadeTexPhongProg->addUniform("camPos");
   fadeTexPhongProg->addUniform("lightPos");
   fadeTexPhongProg->addUniform("lightCol");
   fadeTexPhongProg->addUniform("viewDist");
   fadeTexPhongProg->addUniform("matAmb");
   fadeTexPhongProg->addUniform("matDif");
   fadeTexPhongProg->addUniform("matSpec");
   fadeTexPhongProg->addUniform("matShine");
   fadeTexPhongProg->addUniform("texture0");
   fadeTexPhongProg->addAttribute("vertPos");
   fadeTexPhongProg->addAttribute("vertNor");
   fadeTexPhongProg->addAttribute("vertTex");

   // Initialize the fading, waving, blinn-phong program
   fadeWavePhongProg = make_shared<Program>();
   fadeWavePhongProg->setVerbose(true);
   fadeWavePhongProg->setShaderNames(RESOURCE_DIR + "fading_waving_phong_vert.glsl", RESOURCE_DIR + "fading_waving_phong_frag.glsl");
   fadeWavePhongProg->init();
   fadeWavePhongProg->addUniform("P");
   fadeWavePhongProg->addUniform("V");
   fadeWavePhongProg->addUniform("M");
   fadeWavePhongProg->addUniform("camPos");
   fadeWavePhongProg->addUniform("lightPos");
   fadeWavePhongProg->addUniform("lightCol");
   fadeWavePhongProg->addUniform("viewDist");
   fadeWavePhongProg->addUniform("matAmb");
   fadeWavePhongProg->addUniform("matDif");
   fadeWavePhongProg->addUniform("matSpec");
   fadeWavePhongProg->addUniform("matShine");
   fadeWavePhongProg->addUniform("t");
   fadeWavePhongProg->addUniform("wave");
   fadeWavePhongProg->addAttribute("vertPos");
   fadeWavePhongProg->addAttribute("vertNor");
}

static void render()
{
   double t = glfwGetTime();
   // Press p to pause and play path
   if(!keyToggles['p']) {
      iter = ((iter + 1) % iterations); // Make sure iterations is set correctly!
      setCamPos6dof(camPosVec[iter], camDirVec[iter]);
      // std::cout << "pos: " << camPosVec[iter].transpose() << std::endl;
      // std::cout << "dir: " << camDirVec[iter].transpose() << std::endl;
   }
   // Switch to camArr instead of camPosVec to follow PRM path!
   // Make sure to comment in code to initialize camArr

   // Get current frame buffer size.
   // Important for retina displays!
   glfwGetFramebufferSize(window, &actualW, &actualH);
   glViewport(0, 0, actualW, actualH);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // Clear framebuffer.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   float aspect = actualW/(float)actualH;

   // Define the light position in the world.
   Vector3f lightPos(200.0f, 200.0f, -200.0f);
   Vector3f lightCol(1.0f, 1.0f, 1.0f);

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto V = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();

   // Apply initial matrix transforms
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, viewDist);

   // Create the view matrix
   V->pushMatrix();
   Vector3d lookAtPos = camPos + camDir;
   V->lookAt(camPos.cast<float>(), lookAtPos.cast<float>(), Vector3f(0.0f, 1.0f, 0.0f));

   // Push this frame
   M->pushMatrix();

//   /* Draw objects that:
//   1. Are textured objs
//   2. Need blinn-phong lighting
//   3. Fade in the distance
//   */
//   fadeTexPhongProg->bind();
//   glUniformMatrix4fv(fadeTexPhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
//   glUniformMatrix4fv(fadeTexPhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
//   glUniform3f(fadeTexPhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
//   glUniform3f(fadeTexPhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
//   glUniform3f(fadeTexPhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
//   glUniform1f(fadeTexPhongProg->getUniform("viewDist"), viewDist);
//
//
//   // Draw the nautilus's
//   rustedMetalTex0->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(3, fadeTexPhongProg);
//   for (int i = 0; i < (int)nautilusTransforms.size(); i++) {
//      M->pushMatrix();
//      M->multMatrix(nautilusTransforms[i]);
//      glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//      nautilus->draw(fadeTexPhongProg);
//      M->popMatrix();
//   }
//   rustedMetalTex0->unbind(0);
//
//   // Draw the submarine's
//   rustedMetalTex1->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(4, fadeTexPhongProg);
//   for (int i = 0; i < (int)submarineTransforms.size(); i++) {
//      M->pushMatrix();
//      M->multMatrix(submarineTransforms[i]);
//      glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//      submarine->draw(fadeTexPhongProg);
//      M->popMatrix();
//   }
//   rustedMetalTex1->unbind(0);
//
//   // Draw the rocks:
//   coral0Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(2, fadeTexPhongProg);
//   for (int i = 0; i < (int)rockTransforms.size(); i++) {
//      // Switch textures at the 1 / 3 and 2 / 3 marks.
//      if (i == (int)rockTransforms.size() / 3) {
//         coral0Tex->unbind(0);
//         coral1Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//      }
//      else if (i == 2 * (int)rockTransforms.size() / 3) {
//         coral1Tex->unbind(0);
//         coral2Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//      }
//
//      M->pushMatrix();
//      M->multMatrix(rockTransforms[i]);
//      glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//      switch (i % 4) {
//      case 0:
//         rock0->draw(fadeTexPhongProg);
//         break;
//      case 1:
//         rock1->draw(fadeTexPhongProg);
//         break;
//      case 2:
//         rock2->draw(fadeTexPhongProg);
//         break;
//      case 3:
//         rock3->draw(fadeTexPhongProg);
//         break;
//      }
//      M->popMatrix();
//   }
//   coral2Tex->unbind(0);
//
//   // Draw the sand.
//   sandTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(0, fadeTexPhongProg);
//   Vector2i gridLL(-20, -20 );
//   Vector2i gridUR(20, 20);
//   float scale = 10.0f;
//   for (int i = gridLL(0); i < gridUR(0); i++) {
//      for (int j = gridLL(1); j < gridUR(1); j++) {
//         M->pushMatrix();
//         M->translate(Vector3f(2.0f * scale * (float)i, 0.0f, 2.0f * scale * (float)j));
//         M->scale(scale);
//         glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//         plane->draw(fadeTexPhongProg);
//         M->popMatrix();
//      }
//   }
//   sandTex->unbind(0);
//
//   // Draw the surface
//   glDisable(GL_CULL_FACE);
//   surfaceTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(1, fadeTexPhongProg);
//   for (int i = gridLL(0); i < gridUR(0); i++) {
//      for (int j = gridLL(1); j < gridUR(1); j++) {
//         M->pushMatrix();
//         M->translate(Vector3f(2.0f * scale * (float)i, 30.0f, 2.0f * scale * (float)j));
//         M->scale(scale);
//         glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//         plane->draw(fadeTexPhongProg);
//         M->popMatrix();
//      }
//   }
//   glEnable(GL_CULL_FACE);
//   surfaceTex->unbind(0);
//   // fadeTexPhongProg->unbind();
//
//   // Draw the wreck- chimChiminy
//   wreckTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
//   setTextureMaterial(5, fadeTexPhongProg);
//   M->pushMatrix();
//   M->translate(Vector3f(0, 2, 0));
//   M->rotate(M_PI, Vector3f(1, 0, 0));
//   M->scale(8);
//   glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//   wreck->draw(fadeTexPhongProg);
//   M->popMatrix();
//   wreckTex->unbind(0);
//   fadeTexPhongProg->unbind();

   // Draw the wreck- beaufighter
   // wreckTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
   // setTextureMaterial(5, fadeTexPhongProg);
   // M->pushMatrix();
   // M->translate(Vector3f(0, 2, 0));
   // M->rotate(M_PI / 2, Vector3f(1, 0, 0));
   // M->scale(8);
   // glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
   // wreck->draw(fadeTexPhongProg);
   // M->popMatrix();
   // wreckTex->unbind(0);
   // fadeTexPhongProg->unbind();

//   /* Draw objects that:
//   1. Are waving objs
//   2. Need blinn-phong lighting
//   3. Fade in the distance
//   */
//   fadeWavePhongProg->bind();
//   glUniformMatrix4fv(fadeWavePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
//   glUniformMatrix4fv(fadeWavePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
//   glUniform3f(fadeWavePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
//   glUniform3f(fadeWavePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
//   glUniform3f(fadeWavePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
//   glUniform3f(fadeWavePhongProg->getUniform("wave"), 1.0f, 0.0f, 0.0f);
//   glUniform1f(fadeWavePhongProg->getUniform("viewDist"), viewDist);
//   glUniform1f(fadeWavePhongProg->getUniform("t"), (float)t);
//
//   // Draw seaweeds
//   setMaterial(7, fadeWavePhongProg);
//   for (int i = 0; i < (int)seaweedTransforms.size(); i++) {
//      M->pushMatrix();
//      M->multMatrix(seaweedTransforms[i]);
//      glUniformMatrix4fv(fadeWavePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//      seaweed->draw(fadeWavePhongProg);
//      M->popMatrix();
//   }
//   fadeWavePhongProg->unbind();
//
//   /* Draw objects that:
//   1. Are sharks
//   2. Need blinn-phong lighting
//   3. Fade in the distance
//   */
//   // Draw the sharks using a special shader that lightens their underbellies.
//   fadeSharkPhongProg->bind();
//   glUniformMatrix4fv(fadeSharkPhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
//   glUniformMatrix4fv(fadeSharkPhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
//   glUniform3f(fadeSharkPhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
//   glUniform3f(fadeSharkPhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
//   glUniform3f(fadeSharkPhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
//   glUniform1f(fadeSharkPhongProg->getUniform("viewDist"), viewDist);
//   glUniform1f(fadeSharkPhongProg->getUniform("baseAlpha"), 1.0f);
//
//   // Draw sharks.
//   setMaterial(5, fadePhongProg);
//   // for (unsigned int i = 0; i < sharks.size(); i++) {
//     //  sharks[i].draw(fadeSharkPhongProg, t);
//   // }
//   shark.draw(fadeSharkPhongProg, t);
//
//   fadeSharkPhongProg->unbind();

//   /* Draw objects that:
//   1. Are standard objs
//   2. Need blinn-phong lighting
//   3. Fade in the distance
//   */
//   fadePhongProg->bind();
//   glUniformMatrix4fv(fadePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
//   glUniformMatrix4fv(fadePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
//   glUniformMatrix4fv(fadePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//   glUniform3f(fadePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
//   glUniform3f(fadePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
//   glUniform3f(fadePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
//   glUniform1f(fadePhongProg->getUniform("viewDist"), viewDist);
//   glUniform1f(fadePhongProg->getUniform("baseAlpha"), 1.0f);
//
//   // Draw Nefertiti
//   // M->pushMatrix();
//   // M->translate(Vector3f(0.0f, 1.5f, 2.0f));
//   // M->rotate((float)M_PI * 0.33f, Vector3f(-1.0f, 0.0f, 1.0f));
//   // M->rotate((float)-M_PI * 0.5f, Vector3f(1.0f, 0.0f, 0.0f));
//   // M->scale(10.0f);
//   // glUniformMatrix4fv(fadePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
//   // setMaterial(2, fadePhongProg);
//   // nefertiti->draw(fadePhongProg);
//   // M->popMatrix();
//
//   // Draw bubbles.
//   glUniform1f(fadePhongProg->getUniform("baseAlpha"), 0.5f);
//   setMaterial(6, fadePhongProg);
//   for (unsigned int i = 0; i < bubbles.size(); i++) {
//      bubbles[i].draw(fadePhongProg, t);
//   }
//    fadePhongProg->unbind();
//
    
    fadePhongProg->bind();
    glUniformMatrix4fv(fadePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    glUniform3f(fadePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
    glUniform1f(fadePhongProg->getUniform("viewDist"), viewDist);
    
    //    Trying to draw out PRM path
//    GLenum error = glGetError();
//    if (error != GL_NO_ERROR) {
//       cout << "error 1088: " << error << endl;
//    }
    glUniform1f(fadePhongProg->getUniform("baseAlpha"), 1.0f);
    
    glBindVertexArray(path_VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, path_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    
    for (int i = 0; i < path_indices.size(); i++) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndxBuffObjs[i]);
//        printf("length of ibo %d is %d\n", i, length_indices[i]);
        
        glDrawElements(GL_LINE_STRIP, length_indices[i], GL_UNSIGNED_INT, 0);//path_indices[i]);
        
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cout << "error 1097: " << error << endl;
        }
    }
    
//    //second paramater is count
//    glLineWidth(1);
//    glDrawArrays(GL_LINE_STRIP, 0, 61);

    glDisableVertexAttribArray(0);
//    error = glGetError();
//    if (error != GL_NO_ERROR) {
//       cout << "error 1107: " << error << endl;
//    }
    
   fadePhongProg->unbind();
   
   // Pop matrix stacks.
   M->popMatrix();
   V->popMatrix();
   P->popMatrix();


   // Read directly from framebuffer- super slow, don't do
   // vector<unsigned char> pixels(actualW * actualH * 3);
   // glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
   // GLfloat pixels[width * height];
   // glGetBufferSubData(GL_COPY_WRITE_BUFFER, 0, width * height,
   //  &pixels[0]);
   // glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, width * height * sizeof(GL_BYTE),
   //  &pixels[0]);
   // glBufferData(GL_STATIC_READ)
   // glGetNamedBufferSubData(0, 0, width * height, &pixels[0]);
   //   for (int i = 0; i < width; i++) {
   //    for (int j = 0; j < height; j++) {
   //       cout << (int)pixels[i * j + j] << " " 
   //        << (int)pixels[i * j + j + 1] << " " 
   //        << (int)pixels[i * j + j + 2] << ", ";
   //    }
   //    cout << endl << endl;
   //   }
   // cout << endl;


   // Zoe's code to write to texture
   /*
   //regardless unbind the FBO   
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   // Clear framebuffer.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //draw textured quad
   tex_prog->bind();
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, renderTexture);
   glUniform1i(tex_prog->getUniform("texBuf"), 0);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, myFrame);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   glDisableVertexAttribArray(0);
   tex_prog->unbind();
   */
}

int main(int argc, char **argv)
{
   if(argc < 2) {
      cout << "Please specify the resource directory." << endl;
      return 0;
   }
   RESOURCE_DIR = argv[1] + string("/");

   // Set error callback.
   glfwSetErrorCallback(error_callback);
   // Initialize the library.
   if(!glfwInit()) {
      return -1;
   }

   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

   // Create a windowed mode window and its OpenGL context.
   // window = glfwCreateWindow(1200, 800, "Keenan M. Reimer", NULL, NULL);
   window = glfwCreateWindow(512, 384, "ICEX 2016", NULL, NULL);
   if(!window) {
      glfwTerminate();
      return -1;
   }
   // Make the window's context current.
   glfwMakeContextCurrent(window);
   // Initialize GLEW.
   glewExperimental = true;
   if(glewInit() != GLEW_OK) {
      cerr << "Failed to initialize GLEW" << endl;
      return -1;
   }
   //weird bootstrap of glGetError
   glGetError();
   cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

   // Set vsync.
   glfwSwapInterval(1);
   // Set keyboard callback.
   glfwSetKeyCallback(window, key_callback);
   // Set char callback.
   glfwSetCharCallback(window, char_callback);
   //set the mouse call back
   glfwSetMouseButtonCallback(window, mouse_callback);
   // Set cursor position callback.
   glfwSetCursorPosCallback(window, cursor_pos_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

   // Initialize scene. Note geometry initialized in init now
   init();

   initCamPath();

   // Loop until the user closes the window.
   while(!glfwWindowShouldClose(window)) {
      // Render scene.
       render();
      // Swap front and back buffers.
      glfwSwapBuffers(window);
      // Poll for and process events.
      glfwPollEvents();
   }

   writeOutTex();

   // Quit program.
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}

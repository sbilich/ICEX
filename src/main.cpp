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
#include "Bubbles.h"
#include "Utilities.h"
#include "Image.h"

using namespace std;
using namespace Eigen;

bool keyToggles[256] = { false }; // only for English keyboards!

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

// GLSL programs
shared_ptr<Program> phongProg; // simple phong lighting shader
shared_ptr<Program> fadePhongProg;  // Fading objs with phong lighting
shared_ptr<Program> fadeTexPhongProg;  // Fading, textured objs with phong lighting
shared_ptr<Program> fadeWavePhongProg;  // Fading, waving objs with phong lighting
shared_ptr<Program> tex_prog;          // Render the framebuffer texture to the screen

// OBJ shapes
shared_ptr<Shape> plane;
shared_ptr<Shape> rock0;
shared_ptr<Shape> rock1;
shared_ptr<Shape> rock2;
shared_ptr<Shape> rock3;
shared_ptr<Shape> seaweed;
shared_ptr<Shape> wreck;
shared_ptr<Shape> iver_fins;
shared_ptr<Shape> iver_noseAndTail;
shared_ptr<Shape> iver_bodyAndRudder;
shared_ptr<Shape> xlighter;

// Wrapper classes to pass multiple objs to higher order classes
shared_ptr<BubblesShapes> bubblesShapes;

// Textures
shared_ptr<Texture> sandTex;
shared_ptr<Texture> surfaceTex;
shared_ptr<Texture> coral0Tex;
shared_ptr<Texture> coral1Tex;
shared_ptr<Texture> coral2Tex;
shared_ptr<Texture> wreckTex;
shared_ptr<Texture> xlighterTex;
shared_ptr<Texture> water_texture;

// Object transforms handled in this thread
vector<Matrix4f> rockTransforms;
vector<Matrix4f> seaweedTransforms;

// Higher order obj classes that handle some of their own movement
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

// Caustic camera parameters
Vector3d caust_camPos;
Vector3d caust_camDir;

int g_width, g_height;
int actualW, actualH;

// These things added for ICEX

// for caustics
const double elapse = 0.15;
double prevTime;
int curWater;
vector<shared_ptr<Texture> > water;

// Vector of camera positions and directions, initialized at the beginning
vector<Vector3d> camPosVec;
vector<Vector3d> camDirVec;

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

// Initializes arrays to circular path around old position of Nefertiti head
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
    bubbles.clear();
    rockTransforms.clear();
    seaweedTransforms.clear();
    
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
        case 0: // bubbles
            glUniform3f(prog->getUniform("matAmb"), 0.9f, 0.9f, 0.9f);
            glUniform3f(prog->getUniform("matDif"), 0.0f, 0.0f, 0.0f);
            glUniform3f(prog->getUniform("matSpec"), 0.1f, 0.1f, 0.1f);
            glUniform1f(prog->getUniform("matShine"), 20.0f);
            break;
        case 1: // seaweed
            glUniform3f(prog->getUniform("matAmb"), 0.0f, 0.2f, 0.1f);
            glUniform3f(prog->getUniform("matDif"), 0.0f, 0.4f, 0.0f);
            glUniform3f(prog->getUniform("matSpec"), 0.4f, 0.5f, 0.4f);
            glUniform1f(prog->getUniform("matShine"), 20.0f);
            break;
        case 2: //iver bodyAndRudder
            glUniform3f(prog->getUniform("matAmb"), 0.1f, 0.1f, 0.1f);
            glUniform3f(prog->getUniform("matDif"), 0.64f, 0.64f, 0.64f);
            glUniform3f(prog->getUniform("matSpec"), 0.5f, 0.5f, 0.5f);
            glUniform1f(prog->getUniform("matShine"), 5.0f);
            break;
        case 3: //iver noseAndTail
            glUniform3f(prog->getUniform("matAmb"), 0.021768f, 0.021768f, 0.021768f);
            glUniform3f(prog->getUniform("matDif"), 0.021768f, 0.021768f, 0.021768f);
            glUniform3f(prog->getUniform("matSpec"), 0.055556f, 0.055556f, 0.055556f);
            glUniform1f(prog->getUniform("matShine"), 5.0f);
            break;
        case 4: //iver fins
            glUniform3f(prog->getUniform("matAmb"), 0.64f, 0.253291f, 0.0f);
            glUniform3f(prog->getUniform("matDif"), 0.64f, 0.253291f, 0.0f);
            glUniform3f(prog->getUniform("matSpec"), 0.5f, 0.5f, 0.5f);
            glUniform1f(prog->getUniform("matShine"), 5.0f);
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
            glUniform1f(prog->getUniform("matDif"), 0.6f);
            glUniform3f(prog->getUniform("matSpec"), 0.1f, 0.1f, 0.1f);
            glUniform1f(prog->getUniform("matShine"), 5.0f);
            break;
        case 3: // ICEX wreck
            glUniform1f(prog->getUniform("matAmb"), 1.0f);
            glUniform1f(prog->getUniform("matDif"), 1.0f);
            glUniform3f(prog->getUniform("matSpec"), 0.0f, 0.0f, 0.0f);
            glUniform1f(prog->getUniform("matShine"), 1.0f);
            break;
        case 4: // ICEX Xlighter wreck
            glUniform1f(prog->getUniform("matAmb"), 0.25f);
            glUniform1f(prog->getUniform("matDif"), 1.0f);
            glUniform3f(prog->getUniform("matSpec"), 0.3f, 0.3f, 0.3f);
            glUniform1f(prog->getUniform("matShine"), 5.0f);
            break;
    }
}

// MARK: Parses through "path.txt" and "roadmap.txt" to get the possible paths' indices and all possible vertices.
void loadRoadMapFromMotionPlanFiles() {
    // get array of vertices
    const string info_file_name = string(RESOURCE_DIR + "info.txt");
    FILE *infoFile = fopen(info_file_name.c_str(), "r");
    char num[10];
    
    fgets(num, 10, infoFile);
    GLuint numVert = atoi(num);
    //    printf("numVert is %d\n", numVert);
    fclose(infoFile);
    
    const string vertex_file_name = string(RESOURCE_DIR + "roadmap.txt");
    FILE *vertexFile = fopen(vertex_file_name.c_str(), "r");
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
    printf("loaded vertices\n");
    // end of getting vertices
    
    // get array of indices
    const string paths_file_name = string(RESOURCE_DIR + "paths.txt");
    FILE *indexFile = fopen(paths_file_name.c_str(), "r");
    char cur[10];
    int count = 0;
    
    while (fscanf(indexFile, " %s", cur) != EOF) {
        GLuint index;
        
        GLuint numIndices = 0;
        fscanf(indexFile, " number of indices: %d", &numIndices);
        //        printf("num indices is %d\n", numIndices);
        
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
    
    printf("Loaded indices\n");
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
    caust_camPos = Vector3d(0.0, 30.0, 0.0);
    caust_camDir = Vector3d(0.0, -1.0, 0.0);
    
    // Set background color.
    glClearColor(.011f, .129f, .247f, 1.0f);
//    glClearColor(.051f, .553f, .875f, 1.0f); // blue background
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // white background
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    // MARK: Init additions for ICEX
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
    
    //loadRoadMapFromMotionPlanFiles();
    //loadRoadMapFromCSVFiles();
    
    // Setup vao for rendering path lines
    glGenVertexArrays(1, &path_VertexArrayID);
    glBindVertexArray(path_VertexArrayID);
    
    // setup vbo for rendering path lines
    glGenBuffers(1, &path_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, path_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(path_vertices), path_vertices, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_path_vertex_buffer_data), g_path_vertex_buffer_data, GL_STATIC_DRAW);
    
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
    
    // Initialize ICEX wreck
    wreck = make_shared<Shape>();
    wreck->loadMesh(RESOURCE_DIR + "chimney.obj");
    wreck->resize();
    wreck->init();
    
    // Initialize ICEX xlighter
    xlighter = make_shared<Shape>();
    xlighter->loadMesh(RESOURCE_DIR + "ManoelPRMsCombined.obj");
    xlighter->resize();
    xlighter->init();
    
    //initialize ICEX iver parts
    //intialize iver_fins
    iver_fins = make_shared<Shape>();
    iver_fins->loadMesh(RESOURCE_DIR + "iver_fins.obj");
    //iver_fins->resize();
    iver_fins->init();
    
    //intialize iver_bodyAndRudder
    iver_bodyAndRudder = make_shared<Shape>();
    iver_bodyAndRudder->loadMesh(RESOURCE_DIR + "iver_bodyAndRudder.obj");
    //iver_bodyAndRudder->resize();
    iver_bodyAndRudder->init();
    
    //intialize iver_noseAndTail
    iver_noseAndTail = make_shared<Shape>();
    iver_noseAndTail->loadMesh(RESOURCE_DIR + "iver_noseAndTail.obj");
    //iver_noseAndTail->resize();
    iver_noseAndTail->init();
    
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
    
    // Initalize the ICEX wreck texture
    wreckTex = make_shared<Texture>();
    wreckTex->setFilename(RESOURCE_DIR + "chimney.jpg");
    wreckTex->init();
    
    // Initalize the ICEX Xlighter wreck texture
    xlighterTex = make_shared<Texture>();
    xlighterTex->setFilename(RESOURCE_DIR + "ManoelPRMsCombined.jpg");
    xlighterTex->init();
    
    // Generate the world
    generate();
    
    //initialize the blinn-phong program
    phongProg = make_shared<Program>();
    phongProg->setVerbose(true);
    phongProg->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
    phongProg->init();
    phongProg->addUniform("P");
    phongProg->addUniform("V");
    phongProg->addUniform("M");
    phongProg->addUniform("camPos");
    phongProg->addUniform("lightPos");
    phongProg->addUniform("lightCol");
    phongProg->addUniform("matAmb");
    phongProg->addUniform("matDif");
    phongProg->addUniform("matSpec");
    phongProg->addUniform("matShine");
    phongProg->addUniform("baseAlpha");
    phongProg->addAttribute("vertPos");
    phongProg->addAttribute("vertNor");
    phongProg->addUniform("caust_V");
    phongProg->addUniform("water");
    
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
    fadePhongProg->addUniform("caust_V");
    fadePhongProg->addUniform("water");
    
    
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
//    fadeTexPhongProg->addUniform("lightCol");
    fadeTexPhongProg->addUniform("lightPos1");
    fadeTexPhongProg->addUniform("lightPos2");
    fadeTexPhongProg->addUniform("lightCol1");
    fadeTexPhongProg->addUniform("lightCol2");
    //fadeTexPhongProg->addUniform("viewDist");
    fadeTexPhongProg->addUniform("matAmb");
    fadeTexPhongProg->addUniform("matDif");
    fadeTexPhongProg->addUniform("matSpec");
    fadeTexPhongProg->addUniform("matShine");
    fadeTexPhongProg->addUniform("texture0");
    //fadeTexPhongProg->addUniform("brightness");
    fadeTexPhongProg->addAttribute("vertPos");
    fadeTexPhongProg->addAttribute("vertNor");
    fadeTexPhongProg->addAttribute("vertTex");
    fadeTexPhongProg->addUniform("caust_V");
    fadeTexPhongProg->addUniform("water");
    fadeTexPhongProg->addUniform("caust");
    
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
    fadeWavePhongProg->addUniform("caust_V");
    fadeWavePhongProg->addUniform("water");
    
    for (int i = 1; i <= 11; i++) {
        shared_ptr<Texture> water_texture = make_shared<Texture>();
        string num = std::to_string(i);
        water_texture->setFilename(RESOURCE_DIR + "water" + num + ".jpg");
        water_texture->init();
        water_texture->setWrapModes(GL_REPEAT, GL_REPEAT);
        water.push_back(water_texture);
    }
    prevTime = glfwGetTime();
    curWater = 0;
}

void drawRocks(shared_ptr<MatrixStack> &M) {
    coral0Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    
    setTextureMaterial(2, fadeTexPhongProg);
    
    for (int i = 0; i < (int)rockTransforms.size()/3; i++) {
        // Switch textures at the 1 / 3 and 2 / 3 marks.
        if (i == (int)rockTransforms.size() / 3) {
            coral0Tex->unbind(0);
            coral1Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
        }
        else if (i == 2 * (int)rockTransforms.size() / 3) {
            coral1Tex->unbind(0);
            coral2Tex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
        }
        
        M->pushMatrix();
        M->scale(0.2);
        M->multMatrix(rockTransforms[i]);
        glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        switch (i % 4) {
            case 0:
                rock0->draw(fadeTexPhongProg);
                break;
            case 1:
                rock1->draw(fadeTexPhongProg);
                break;
            case 2:
                rock2->draw(fadeTexPhongProg);
                break;
            case 3:
                rock3->draw(fadeTexPhongProg);
                break;
        }
        M->popMatrix();
    }
    coral2Tex->unbind(0);
}

void drawSand(shared_ptr<MatrixStack> &M, Vector2i &gridLL, Vector2i &gridUR, float scale) {
    sandTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    setTextureMaterial(0, fadeTexPhongProg);
    for (int i = gridLL(0); i < gridUR(0); i++) {
        for (int j = gridLL(1); j < gridUR(1); j++) {
            M->pushMatrix();
            M->translate(Vector3f(2.0f * scale * (float)i, 0.0f, 2.0f * scale * (float)j));
            M->scale(scale);
            glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
            plane->draw(fadeTexPhongProg);
            M->popMatrix();
        }
    }
    sandTex->unbind(0);
}

void drawSurface(shared_ptr<MatrixStack> &M, Vector2i &gridLL, Vector2i &gridUR, float scale) {
    glDisable(GL_CULL_FACE);
    surfaceTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    glUniform1f(fadeTexPhongProg->getUniform("caust"), 0);
    setTextureMaterial(1, fadeTexPhongProg);
    for (int i = gridLL(0); i < gridUR(0); i++) {
        for (int j = gridLL(1); j < gridUR(1); j++) {
            M->pushMatrix();
            M->translate(Vector3f(2.0f * scale * (float)i, 30.0f, 2.0f * scale * (float)j));
            M->scale(scale);
            glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
            plane->draw(fadeTexPhongProg);
            M->popMatrix();
        }
    }
    glEnable(GL_CULL_FACE);
    surfaceTex->unbind(0);
    glUniform1f(fadeTexPhongProg->getUniform("caust"), 1.0);
}

void drawScenery(shared_ptr<MatrixStack> &M) {
    drawRocks(M);
    
    Vector2i gridLL(-20, -20 );
    Vector2i gridUR(20, 20);
    float scale = 10.0f;
    drawSand(M, gridLL, gridUR, scale);
    drawSurface(M, gridLL, gridUR, scale);
}

void drawChimChiminy(shared_ptr<MatrixStack> &M) {
    wreckTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    setTextureMaterial(3, fadeTexPhongProg);
    M->pushMatrix();
    M->translate(Vector3f(0, 2, 0));
    M->rotate(M_PI, Vector3f(1, 0, 0));
    M->scale(8);
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    wreck->draw(fadeTexPhongProg);
    M->popMatrix();
    wreckTex->unbind(0);
}

void drawBeaufighter(shared_ptr<MatrixStack> &M) {
    wreckTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    setTextureMaterial(3, fadeTexPhongProg);
    M->pushMatrix();
    M->translate(Vector3f(0, 2, 0));
    M->rotate(M_PI / 2, Vector3f(1, 0, 0));
    M->scale(8);
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    wreck->draw(fadeTexPhongProg);
    M->popMatrix();
    wreckTex->unbind(0);
}

void drawXlighter(shared_ptr<MatrixStack> &M) {
    xlighterTex->bind(fadeTexPhongProg->getUniform("texture0"), 0);
    //wreckTex->bind(fadeTexPhongProg->getUniform("brightness"), 0.3);
    setTextureMaterial(4, fadeTexPhongProg);
    M->pushMatrix();
    M->translate(Vector3f(0, 2, -1));
    M->rotate(90 * M_PI / 180.0f, Vector3f(1, 0, 0));
    M->rotate(-45 * M_PI / 180.0f, Vector3f(0, 1, 0));
    M->rotate(10 * M_PI / 180.0f, Vector3f(0, 0, 1));
    M->rotate(20 * M_PI / 180.0f, Vector3f(0, 1, 0));
    M->scale(8);
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    xlighter->draw(fadeTexPhongProg);
    M->popMatrix();
    xlighterTex->unbind(0);
}

void drawIver(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &M,
              shared_ptr<MatrixStack> &caust_V, Vector3f &lightPos, Vector3f &lightCol, double t) {
    phongProg->bind();
    
    glUniformMatrix4fv(phongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(phongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniform3f(phongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniform3f(phongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(phongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
    glUniform1f(phongProg->getUniform("baseAlpha"), 1.0f);
    water_texture->bind(phongProg->getUniform("water"), curWater + 6);
    glUniformMatrix4fv(phongProg->getUniform("caust_V"), 1, GL_FALSE, caust_V->topMatrix().data());
    
    setMaterial(2, phongProg);
    
    M->pushMatrix();
    M->translate(Vector3f(0, 5, 5));
    M->rotate(-40 * M_PI / 180.0f, Vector3f(1, 0, 0));
    M->rotate(-45 * M_PI / 180.0f, Vector3f(0, 1, 0));
    M->scale(.35);
    glUniformMatrix4fv(phongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    iver_bodyAndRudder->draw(phongProg);
    setMaterial(3, phongProg);
    iver_noseAndTail->draw(phongProg);
    setMaterial(4, phongProg);
    iver_fins->draw(phongProg);
    M->popMatrix();
    
    water_texture->unbind(curWater + 6);
    fadePhongProg->unbind();
}

/* Draw objects that:
 1. Are textured objs
 2. Need blinn-phong lighting
 3. Fade in the distance
 */
void drawTexturedObjects(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &M,
                         shared_ptr<MatrixStack> &caust_V, float lightPos[], float lightCol[]) {
    fadeTexPhongProg->bind();
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniform3f(fadeTexPhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("lightPos"), 1, GL_FALSE, lightPos);
//    glUniform1fv(fadeTexPhongProg->getUniform("lightCol"), 6, lightCol);
    
    glUniform3f(fadeTexPhongProg->getUniform("lightPos1"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(fadeTexPhongProg->getUniform("lightPos2"), lightPos[3], lightPos[4], lightPos[5]);
    glUniform3f(fadeTexPhongProg->getUniform("lightCol1"), lightCol[0], lightCol[1], lightCol[2]);
    glUniform3f(fadeTexPhongProg->getUniform("lightCol2"), lightCol[3], lightCol[4], lightCol[5]);
    
    //glUniform1f(fadeTexPhongProg->getUniform("viewDist"), viewDist);
    //glUniform1f(fadeTexPhongProg->getUniform("brightness"), 0.5);
    water_texture->bind(fadeTexPhongProg->getUniform("water"), curWater + 6);
    glUniformMatrix4fv(fadeTexPhongProg->getUniform("caust_V"), 1, GL_FALSE, caust_V->topMatrix().data());
    glUniform1f(fadeTexPhongProg->getUniform("caust"), 1.0f);
    
    drawScenery(M);
    //    drawChimChiminy(M);
    //    drawBeaufighter(M);
    drawXlighter(M);
    
    water_texture->unbind(curWater + 6);
    fadeTexPhongProg->unbind();
}

/* Draw objects that:
 1. Are waving objs
 2. Need blinn-phong lighting
 3. Fade in the distance
 */
void drawSeaweed(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &M,
                 shared_ptr<MatrixStack> &caust_V, Vector3f &lightPos, Vector3f &lightCol, double t) {
    fadeWavePhongProg->bind();
    glUniformMatrix4fv(fadeWavePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(fadeWavePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniform3f(fadeWavePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniform3f(fadeWavePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(fadeWavePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
    glUniform3f(fadeWavePhongProg->getUniform("wave"), 1.0f, 0.0f, 0.0f);
    glUniform1f(fadeWavePhongProg->getUniform("viewDist"), viewDist);
    glUniform1f(fadeWavePhongProg->getUniform("t"), (float)t);
    water_texture->bind(fadeWavePhongProg->getUniform("water"), curWater + 6);
    glUniformMatrix4fv(fadeWavePhongProg->getUniform("caust_V"), 1, GL_FALSE, caust_V->topMatrix().data());
    
    // Draw seaweeds
    setMaterial(1, fadeWavePhongProg);
    for (int i = 0; i < (int)seaweedTransforms.size()/3; i++) {
        M->pushMatrix();
        M->scale(0.3);
        M->multMatrix(seaweedTransforms[i]);
        glUniformMatrix4fv(fadeWavePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
        seaweed->draw(fadeWavePhongProg);
        M->popMatrix();
    }
    
    water_texture->unbind(curWater + 6);
    fadeWavePhongProg->unbind();
}

/* Draw objects that:
 1. Are standard objs
 2. Need blinn-phong lighting
 3. Fade in the distance
 */
void drawBubbles(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &M,
                 shared_ptr<MatrixStack> &caust_V, Vector3f &lightPos, Vector3f &lightCol, double t) {
    fadePhongProg->bind();
    glUniformMatrix4fv(fadePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    glUniform3f(fadePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
    glUniform1f(fadePhongProg->getUniform("viewDist"), viewDist);
    glUniform1f(fadePhongProg->getUniform("baseAlpha"), 1.0f);
    glUniformMatrix4fv(fadePhongProg->getUniform("caust_V"), 1, GL_FALSE, caust_V->topMatrix().data());
    water_texture->bind(fadePhongProg->getUniform("water"), curWater + 6);
    
    // Draw Bubbles
    glUniform1f(fadePhongProg->getUniform("baseAlpha"), 0.5f);
    setMaterial(0, fadePhongProg);
    for (unsigned int i = 0; i < bubbles.size(); i++) {
        bubbles[i].draw(fadePhongProg, t);
    }
    
    water_texture->unbind(curWater + 6);
    fadePhongProg->unbind();
}

// MARK: Draw Roadmap
void drawPaths(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &M,
                shared_ptr<MatrixStack> &caust_V, Vector3f &lightPos,Vector3f &lightCol) {
    fadePhongProg->bind();
    glUniformMatrix4fv(fadePhongProg->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());
    glUniformMatrix4fv(fadePhongProg->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
    glUniform3f(fadePhongProg->getUniform("camPos"), (float) camPos[0], (float) camPos[1], (float) camPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(fadePhongProg->getUniform("lightCol"), lightCol(0), lightCol(1), lightCol(2));
    glUniform1f(fadePhongProg->getUniform("viewDist"), viewDist);
    glUniform1f(fadePhongProg->getUniform("baseAlpha"), 1.0f);
    water_texture->bind(fadePhongProg->getUniform("water"), 0);
    glUniformMatrix4fv(fadePhongProg->getUniform("caust_V"), 1, GL_FALSE, caust_V->topMatrix().data());
    
    glBindVertexArray(path_VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, path_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    
    for (int i = 0; i < path_indices.size(); i++) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndxBuffObjs[i]);
        
        //        printf("Printing out current ibo\n");
        //        for (int j = 0; j < length_indices[i]; j++) {
        //            printf("%d\n", IndxBuffObjs[i][j]);
        //        }
        //        printf("length of ibo %d is %d\n", i, length_indices[i]);
        
        glDrawElements(GL_LINE_STRIP, length_indices[i], GL_UNSIGNED_INT, 0);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cout << "error 1097: " << error << endl;
        }
    }
    
    glDisableVertexAttribArray(0);
    water_texture->unbind(0);
    fadePhongProg->unbind();
}

//void readFromFrameBuffer(int width, int height) {
//     Read directly from framebuffer- super slow, don't do
//     vector<unsigned char> pixels(actualW * actualH * 3);
//     glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
//     GLfloat pixels[width * height];
//     glGetBufferSubData(GL_COPY_WRITE_BUFFER, 0, width * height,
//      &pixels[0]);
//     glGetBufferSubData(GL_PIXEL_PACK_BUFFER, 0, width * height * sizeof(GL_BYTE),
//      &pixels[0]);
//     glBufferData(GL_STATIC_READ)
//     glGetNamedBufferSubData(0, 0, width * height, &pixels[0]);
//       for (int i = 0; i < width; i++) {
//        for (int j = 0; j < height; j++) {
//           cout << (int)pixels[i * j + j] << " "
//            << (int)pixels[i * j + j + 1] << " "
//            << (int)pixels[i * j + j + 2] << ", ";
//        }
//        cout << endl << endl;
//       }
//     cout << endl;
//}

//void writeToTexture() {
//    // Zoe's code to write to texture
//     //regardless unbind the FBO
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//     // Clear framebuffer.
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//     //draw textured quad
//     tex_prog->bind();
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, renderTexture);
//     glUniform1i(tex_prog->getUniform("texBuf"), 0);
//
//     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//     glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, myFrame);
//
//     glEnableVertexAttribArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
//     glDrawArrays(GL_TRIANGLES, 0, 6);
//     glDisableVertexAttribArray(0);
//     tex_prog->unbind();
//}

// MARK: render objects to scene
static void render()
{
    if (glfwGetTime() - prevTime >= elapse) {
        if (curWater == 10) {
            curWater = 0;
        } else
            curWater++;
        prevTime = glfwGetTime();
    };
    water_texture = water[curWater];

    double t = glfwGetTime();
    // Press p to pause and play path
    if(keyToggles['p']) { //keyToggles['p']) {
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
    
    float lightPos[] = {200.0f, 200.0f, 200.0f,
                        8.0f, 1.0f, -10.0f};
    float lightCol[] = {0.25f, 0.25f, 0.5f,
                        1.0f, 1.0f, 1.0f};
    
    // Vector3f lightPos(200.0f, 200.0f, 200.0f);
    // Vector3f lightCol(0.25f, 0.25f, 0.5f);
    
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
    
    // Caustic matrix stacks
    auto caust_V = make_shared<MatrixStack>();
    
    // Apply scene camera transforms
    caust_V->pushMatrix();
    Vector3d caust_lookAtPos = caust_camPos + caust_camDir;
    caust_V->lookAt(caust_camPos.cast<float>(), caust_lookAtPos.cast<float>(), Vector3f(1.0f, 0.0f, 0.0f));
    
    Vector3f lightPosVec = Vector3f(lightPos[0], lightPos[1], lightPos[2]);
    Vector3f lightColVec = Vector3f(lightCol[0], lightCol[1], lightCol[2]);
    
    drawSeaweed(P, V, M, caust_V, lightPosVec, lightColVec, t);
    drawBubbles(P, V, M, caust_V, lightPosVec, lightColVec, t);
    drawTexturedObjects(P, V, M, caust_V, lightPos, lightCol);
    drawIver(P, V, M, caust_V, lightPosVec, lightColVec, t);
//    drawPaths(P, V, M, lightPos, lightCol);
    
    // Pop matrix stacks.
    M->popMatrix();
    V->popMatrix();
    P->popMatrix();
    caust_V->popMatrix();
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

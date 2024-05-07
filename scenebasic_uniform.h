#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/frustum.h"
#include "helper/teapot.h"
#include "helper/torus.h"


class SceneBasic_Uniform : public Scene
{
private:

    Plane plane;
    SkyBox sky;
    std::unique_ptr<ObjMesh> mesh, cow;
    float tPrev, lightAngle, lightRotationSpeed;
    float angle;
    float time, deltaT;
    float rotSpeed;
    GLSLProgram prog, solidProg, PBRprog;
    GLuint shadowFBO, pass1Index, pass2Index;
    int shadowMapWidth, shadowMapHeight;
    glm::mat4 lightPV, shadowBias;
    Frustum lightFrustum;
    glm::vec4 lightPos;
    glm::vec3 freeCamPos;
    glm::vec3 freeCamFront;
    glm::vec3 freeCamUp;
    float freeCamPitch;
    float freeCamYaw;
    float freeCamSpeed;
    GLFWwindow* window;


    void setMatrices();
    void compile();
    void drawScene();
    void setupFBO();
    void drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color, float scale);
    
public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
    void processInput(GLFWwindow* window);
    void updateCamera(float deltaTime);
    
};

#endif // SCENEBASIC_UNIFORM_H

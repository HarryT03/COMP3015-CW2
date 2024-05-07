#include "scenebasic_uniform.h"
#include <cstdio>
#include <cstdlib>
#include <string>
using std::string;

#include <sstream>
#include <iostream>
#include "GLFW/glfw3.h"
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"
#include "helper/torus.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;



SceneBasic_Uniform::SceneBasic_Uniform() : plane(40.0f, 40.0f, 2, 2), sky(100.0f), tPrev(0), rotSpeed(0.2f), shadowMapWidth(512), shadowMapHeight(512), lightPos(5.0f, 5.0f, 5.0f, 1.0f){

    mesh = ObjMesh::load("media/streetlight.obj", true);
    //cow = ObjMesh::load("../Project_Template/media/spot/spot_triangulated.obj", true);
    freeCamPos = glm::vec3(0.0f, 4.0f, 10.0f);
    freeCamFront = glm::vec3(0.0f, 0.0f, -1.0f);
    freeCamUp = glm::vec3(0.0f, 1.0f, 0.0f);
    freeCamSpeed = 5.0f;


}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    angle = glm::quarter_pi<float>();
    //model = mat4(1.0f);
    setupFBO();
    GLuint programHandle = prog.getHandle();
    pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
    pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");
    shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.5f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 0.5f, 0.0f),
        vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );

    float c = 1.35f;
    vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f);
    lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
    lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();
    //projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);

    view = glm::lookAt(
        glm::vec3(0.0f, 4.0f, 7.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    lightAngle = 0.0f;
    lightRotationSpeed = 0.0f;

    setupFBO();

    prog.use();
    prog.setUniform("Light.Intensity", vec3(1.0f));
    prog.setUniform("Light.La", vec3(1.0f));
    prog.setUniform("Light.L", vec3(1.0f));
    prog.setUniform("ShadowMap", 0);

    PBRprog.use();
    //new pbr stuff
    PBRprog.setUniform("Light[0].L", glm::vec3(45.0f));
    PBRprog.setUniform("Light[0].Position", view * glm::vec4(5.0f, 5.0f, 5.0f, 1.0f));
    PBRprog.setUniform("Light[1].L", glm::vec3(0.3f));
    PBRprog.setUniform("Light[1].Position", glm::vec4(0, 0.15f, -1.0f, 0));
    PBRprog.setUniform("Light[2].L", glm::vec3(45.0f));
    PBRprog.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
    prog.use();
}



void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
        prog.use();

        solidProg.compileShader("shader/solid.vert");
        solidProg.compileShader("shader/solid.frag");
        solidProg.link();

        PBRprog.compileShader("shader/PBR.vert");
        PBRprog.compileShader("shader/PBR.frag");
        PBRprog.link();


	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::processInput(GLFWwindow* window)
{
    const float cameraSpeed = freeCamSpeed * deltaT; // Adjust the speed
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        freeCamPos += cameraSpeed * freeCamFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        freeCamPos -= cameraSpeed * freeCamFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        freeCamPos -= glm::normalize(glm::cross(freeCamFront, freeCamUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        freeCamPos += glm::normalize(glm::cross(freeCamFront, freeCamUp)) * cameraSpeed;
}

void SceneBasic_Uniform::update( float t )
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();

    if (animating())
    {
        lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
        lightPos.x = glm::cos(lightAngle) * 7.0f;
        lightPos.y = 3.0f;
        lightPos.z = glm::sin(lightAngle) * 7.0f;
    }

    /*processInput(window);*/
    updateCamera(deltaT);

}

void SceneBasic_Uniform::render()
{
    //Update a pass
    prog.use();

    //Pass1 shadow map gen
    view = lightFrustum.getViewMatrix();
    projection = lightFrustum.getProjectionMatrix();
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, shadowMapWidth, shadowMapHeight);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    drawScene();
    glCullFace(GL_BACK);
    glFlush();
    //spitOutDepthBuffer();

    //Render pass
    float c = 2.0f;
    vec3 cameraPos(c * 11.5f * cos(angle), c * 7.0f, c * 11.5f * sin(angle));
    view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f));
    projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
    PBRprog.setUniform("Light[0].Position", view * lightPos);
    drawScene();

    solidProg.use();
    solidProg.setUniform("Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));
    mat4 mv = view * lightFrustum.getInverseViewMatrix();
    solidProg.setUniform("MVP", projection * mv);
    lightFrustum.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(mv));
    prog.setUniform("MVP", projection * mv);

    PBRprog.setUniform("ModelViewMatrix", mv);
    PBRprog.setUniform("NormalMatrix", glm::mat3(mv));
    PBRprog.setUniform("MVP", projection * mv);
    //glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))
    
    prog.setUniform("ShadowMatrix", lightPV * model);
}



void SceneBasic_Uniform::drawScene() {
    vec3 color = vec3(0.2f, 0.5f, 0.9f);

    ////MESH
    //prog.setUniform("Material.Ka", color * 0.05f);
    //prog.setUniform("Material.Kd", color);
    //prog.setUniform("Material.Ks", vec3(0.9f));
    //prog.setUniform("Material.Shininess", 150.0f);
    //model = mat4(1.0f);
    //model = glm::scale(model, vec3(0.2f));
    //model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::translate(model, vec3(0.0f, 0.5f, 0.5f));
    //setMatrices();
    //mesh->render();

    //render the plane
    prog.setUniform("Material.Kd", vec3(0.25f, 0.25f, 0.25f));
    prog.setUniform("Material.Ka", vec3(0.05f, 0.05f, 0.05f));
    prog.setUniform("Material.Ks", vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("Material.Shininess", 1.0f);
    model = mat4(1.0f);
    setMatrices();
    plane.render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
    setMatrices();
    plane.render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();

    PBRprog.use();
   
    //Draw metal streetlights
    float metalRough = 0.43f;
    float scale = 0.2f;
    // Gold
    drawSpot(glm::vec3(0.0f, 0.0f, 0.0f), metalRough, 1, glm::vec3(1, 0.71f, 0.29f), scale);
    // Copper
    drawSpot(glm::vec3(-1.5f, 0.0f, 0.0f), metalRough, 1, glm::vec3(0.95f, 0.64f, 0.54f), scale);
	// Aluminum
    drawSpot(glm::vec3(-3.0f, 0.0f, 0.0f), metalRough, 1, glm::vec3(0.91f, 0.92f, 0.92f), scale);
    // Titanium
    drawSpot(glm::vec3(1.5f, 0.0f, 0.0f), metalRough, 1, glm::vec3(0.54f, 0.57f, 0.59f), scale);
    // Silver
    drawSpot(glm::vec3(3.0f, 0.0f, 0.0f), metalRough, 1, glm::vec3(0.95f, 0.93f, 0.88f), scale);
    prog.use();
}

void SceneBasic_Uniform::setupFBO() {

    GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    GLuint depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete" << endl;
    }
    else {
        std::cout << "Framebuffer error" << endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& color, float scale) {
	PBRprog.use();
	PBRprog.setUniform("Material.Rough", rough);
	PBRprog.setUniform("Material.Metal", metal);
	PBRprog.setUniform("Material.Color", color);
	model = mat4(1.0f);
	model = glm::translate(model, pos);
    model = glm::scale(model, vec3(scale));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	setMatrices();
	mesh->render();
}



void SceneBasic_Uniform::updateCamera(float deltaTime)
{
    // Update the view matrix based on camera's new position
    view = glm::lookAt(freeCamPos, freeCamPos + freeCamFront, freeCamUp);
}

// OpenGLProject.cpp : Defines the entry point for the console application.
//

#define GLEW_STATIC

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object3D.h"
#include <vector>

GLFWwindow *window;
float verticalAngle = 0.0f;
float horizontalAngle = 0.0f;
float deltaAngle = 0.1f;
float deltaZ = 0.1f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0, -6.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void init(int w, int h) {
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(w, h, "OpenGL Assignment", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Define the viewport dimensions
    glViewport(0, 0, w, h);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

}

// The view matrix is generated by glm::lookAt call. It constructs an affine transformation matrix from the camera position,
// camera look target position and camera up vector.
glm::mat4 calculateViewTransform() {
    auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    return view;
}


int main() {
    int screenWidth = 800;
    int screenHeight = 600;
    init(screenWidth, screenHeight);
    std::vector<Object3D *> listOfObjects;

    //********This is for example purposes.********
    // You may the chair-like object here.
    Object3D *pRectangle = new Object3D();
    pRectangle->CreateObject();
    listOfObjects.push_back(pRectangle);
    //********This is for example purposes.********

    //Create the shaders.
    Shader shader("res/Basic.shader");

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader
        shader.Use();

        // Transformations
        // Create camera transformation
        glm::mat4 view = calculateViewTransform();
        // Create projection transformation
        glm::mat4 projection = glm::perspective<float>(glm::radians(45.0f), (float) screenWidth / (float) screenHeight,
                                                       0.1f, 100.0f);
        // Get the matrix locations in the shaders
        int modelLoc = glGetUniformLocation(shader.Program, "model");
        int viewLoc = glGetUniformLocation(shader.Program, "view");
        int projLoc = glGetUniformLocation(shader.Program, "projection");
        // Pass the view and projection matrices to the shaders
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glm::vec3 lightPos = glm::vec3(4, 4, 4);

        for (auto pObj : listOfObjects) {
            glBindVertexArray(pObj->VAO);
            // !!!!! You will need to modify the model matrices of each separate object. "modelMatrix"
            // of Object3D is identity if not modified!!!!!
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pObj->modelMatrix));
            // Draw the current object.
            glDrawElements(GL_TRIANGLES, (RESU - 1) * (RESV - 1) * 2 * 3 * pObj->bezierCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        // Swap the buffers

        glfwSwapBuffers(window);

    }
    for (auto pObj : listOfObjects) {
        glDeleteVertexArrays(1, &pObj->VAO);
        glDeleteBuffers(1, &pObj->VBO);
        glDeleteBuffers(1, &pObj->EBO);
        delete pObj;
    }
    shader.Delete();
    glfwTerminate();
    return 0;
}
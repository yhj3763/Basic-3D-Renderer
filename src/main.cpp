#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier);
void ObjectMouseButton(GLFWwindow* window, int button, int action, double x, double y);


const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800/2.0f;
float lastY = 600/2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 objectPos(0.0f, 0.0f, 0.0f);

//glm::vec2 objMoveVec = glm::vec2(0.0f, 0.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (window == NULL){
        SPDLOG_ERROR("Faild to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

 

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("Faild to initialize GLAD");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, OnMouseButton);  
 
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    //Shader ourShader("./shader/simple.vs", "./shader/simple.fs");
    Shader lightShader("./shader/materials.vs", "./shader/materials.fs");
    Shader lightcubeShader("./shader/lightcube.vs", "./shader/lightcube.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    unsigned int VBO;
    unsigned int VAO;

    glGenVertexArrays(1, &VAO);    
    glGenBuffers(1, &VBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightVAO;

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(0);


    std::unique_ptr<Texture> t1(new Texture());
    std::unique_ptr<Texture> t2(new Texture());

    t1->Create();
    t1->load("./image/container.jpg", 1);
    t2->Create();
    t2->load("./image/awesomeface.png", 0);


    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();


    while(!glfwWindowShouldClose(window)){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::Begin("my first ImGui window")){
             ImGui::DragFloat3("Camera Position", glm::value_ptr(camera.Pos), 0.01f);
             ImGui::DragFloat("Camera Yaw", &camera.Yaw, 0.5f);
             ImGui::DragFloat("Camera Pitch", &camera.Pitch, 0.5f, -89.0f, 89.0f);
             ImGui::DragFloat3("Object Position", glm::value_ptr(objectPos), 0.05f);
            if (ImGui::Button("Reset Camera")) {
                camera.Yaw =-90.0f;
                camera.Pitch = 0.0f;
                camera.Pos = glm::vec3(0.0f, 0.0f, 3.0f);
            }
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            if(ImGui::BeginTabBar("tabs")){

                if(ImGui::BeginTabItem("Object")){
                    ImGui::EndTabItem();
                }

                if(ImGui::BeginTabItem("Light"))
                {
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

        }
        ImGui::End();
        camera.updateCameraVector();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
       
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        lightShader.use();
        lightShader.setVec3("light.position", lightPos);
        lightShader.setVec3("viewPos", camera.Pos);

        glm::vec3 lightColor;
        lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
        lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
        lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); 
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        lightShader.setVec3("light.ambient", ambientColor);
        lightShader.setVec3("light.diffuse", diffuseColor);
        lightShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        lightShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        lightShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        lightShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        lightShader.setFloat("material.shininess", 32.0f);
        

       
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f/600.0f, 0.1f, 100.0f);
        lightShader.setMat4("projection", projection);

       
        glm::mat4 view = camera.GetViewMatrix();
        lightShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, objectPos);
        lightShader.setMat4("model", model);
 
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightcubeShader.use();
        lightcubeShader.setMat4("projection", projection);
        lightcubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightcubeShader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (!camera.cameraControl)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.KeyboardProcess(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.KeyboardProcess(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.KeyboardProcess(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.KeyboardProcess(RIGHT, deltaTime);


}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){

    glViewport(0,0, width, height);
}

void  mouse_callback(GLFWwindow* window, double xposin, double yposin){
 
    
    if (!camera.cameraControl)
            return;

    float xpos = static_cast<float>(xposin);
    float ypos = static_cast<float>(yposin);

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.MouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.MouseScroll(static_cast<float>(yoffset));
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
    auto context = glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    camera.MouseButton(button, action, x, y);
    
}

/*
void ObjectMouseButton(GLFWwindow* window, int button, int action, double x, double y){

    double sx, sy;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            objMove = true;
        }
        else if (action == GLFW_RELEASE) {
            glfwGetCursorPos(window, &sx, &sy);

            if(y- sy > 0){
                objMoveVec.y -= 1.0f;
                y = sy;
            }
            else{

                objMoveVec.y += 1.0f;
                y = sy;
            }

            if(x-sx >0){
                objMoveVec.x -=1.0f;
                x = sx;
            }
            else{

                objMoveVec.x += 1.0f;
                x = sx;
            }

            objMove = false;
            
  }

 }
}
*/
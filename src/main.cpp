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
#include <vector>

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "model.h"
#include "ImGuiFileBrowser.h"

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void MouseMove(double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier);
void ObjectMouseButton(GLFWwindow* window, int button, int action, double x, double y);
void OnCursorcPos(GLFWwindow* window, double x, double y);

imgui_addons::ImGuiFileBrowser file_dialog;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = 800/2.0f;
float lastY = 600/2.0f;
bool firstMouse = true;
bool my_tool_active = true;
bool newobj = false;
std::string filepath;
int numofobj = -1;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

//glm::vec2 cutoff { glm::vec2(20.0f, 5.0f) };
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
    glfwSetCursorPosCallback(window, OnCursorcPos);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, OnMouseButton);  

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader lightcubeShader("./shader/lightcube.vs", "./shader/lightcube.fs");
    Shader lightShader("./shader/lightcaster.vs", "./shader/lightcaster.fs");

        float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    std::vector<glm::vec3> objectPositions = {};
    std::vector<glm::vec3> pointLightPositions = {};
    std::vector<glm::vec3> pointLightColors = {};
    std::vector<bool> lights ={};
    std::vector<Model> models ={};

    unsigned int lightVAO;
    unsigned int VBO;
   

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(0);


    std::unique_ptr<cTexture> t1(new cTexture());
    std::unique_ptr<cTexture> t2(new cTexture());


    t1->Create();
    t1->load("./image/container2.png", 0);

    t2->Create();
    t2->load("./image/container2_specular.png", 0);


    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    lightShader.use();
    lightShader.setInt("material.diffuse", 0);
    lightShader.setInt("material.specular", 1);


    while(!glfwWindowShouldClose(window)){
       
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::Begin("my first ImGui window", &my_tool_active, ImGuiWindowFlags_MenuBar)){
          bool open = false, save = false;
            if(ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("Open", NULL))
                        open = true;
                if (ImGui::MenuItem("Save", NULL))
                        save = true;
                    
                ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            
            if(open)
                ImGui::OpenPopup("Open File");
            if(save)
                ImGui::OpenPopup("Save File");

            if(file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".obj,.zip,.7z"))
            {
                std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
                filepath = file_dialog.selected_path;  // The absolute path to the selected file
                Model OurModel(filepath);
                models.push_back(OurModel);
                objectPositions.push_back(glm::vec3( 0.0f,  0.0f,  0.0f));
                numofobj++;
            }
            if(file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
            {
                std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
                std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
                std::cout << file_dialog.ext << std::endl;              // Access ext separately (For SAVE mode)
                //Do writing of files based on extension here
            }

          
             ImGui::DragFloat3("Camera Position", glm::value_ptr(camera.Pos), 0.01f);
             ImGui::DragFloat("Camera Yaw", &camera.Yaw, 0.5f);
             ImGui::DragFloat("Camera Pitch", &camera.Pitch, 0.5f, -89.0f, 89.0f);

            if (ImGui::Button("Reset Camera")) {
                camera.Yaw = -90.0f;
                camera.Pitch = 0.0f;
                camera.Pos = glm::vec3(0.0f, 0.0f, 3.0f);
            }
           
            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            if(ImGui::BeginTabBar("tabs")){
                if(ImGui::BeginTabItem("Objects")){
                    char buff[10] = "Object";
                    char buff2[20] = "Object Position##";
                    char buff3[12] = "Delete##";
                    for(int i = 0 ; i < objectPositions.size();i++){
                        snprintf(buff, 10, "Object%d", i+1);
                        snprintf(buff2, 20,  "Object Position##%d", i);
                        snprintf(buff3, 12,  "Delete##%d", i);
                        if(ImGui::CollapsingHeader(buff, ImGuiTreeNodeFlags_DefaultOpen)){
                        ImGui::DragFloat3(buff2, glm::value_ptr(objectPositions[i]), 0.05f);
                        if(ImGui::Button(buff3)){
                            objectPositions.erase(objectPositions.begin() + i);
                            numofobj--;
                        }
                    }
                }
                    if(ImGui::Button("Add")){
                        objectPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Lights")){   
                    char buff[10] = "Light";
                    char buff2[20] = "Light Position##";
                    char buff3[15] = "Light Color##";
                    char buff4[12] = "Delete##";
                    char buff5[8] ="On##";
                    char buff6[10] ="Off##";
                    for(int i = 0 ; i < pointLightPositions.size();i++){
                        snprintf(buff, 10, "Light%d", i+1);
                        snprintf(buff2, 20,  "Light Position##%d", i);
                        snprintf(buff3, 15,  "Light Color##%d", i);
                        snprintf(buff4, 12,  "Delete##%d", i);
                        snprintf(buff5, 8,  "On##%d", i);
                        snprintf(buff6, 10,  "Off##%d", i);
                        if(ImGui::CollapsingHeader(buff, ImGuiTreeNodeFlags_DefaultOpen)){
                            if(lights[i] == true)
                                ImGui::DragFloat3(buff2, glm::value_ptr(pointLightPositions[i]), 0.05f);
                                ImGui::DragFloat3(buff3, glm::value_ptr(pointLightColors[i]), 0.1f);
                            if(ImGui::Button(buff5)){
                                lights[i] = true;
                            }
                            ImGui::SameLine();
                            if(ImGui::Button(buff6)){
                                lights[i] = false;
                            }

                            if(ImGui::Button(buff4)){
                                pointLightPositions.erase(pointLightPositions.begin() + i);
                                pointLightColors.erase(pointLightColors.begin() + i);
                                lights.erase(lights.begin() +  i);
                            }
                        } 
      
                    }
                    if(ImGui::Button("ADD")){ 
                        if(pointLightPositions.size()==10){
                            return -1;
                        }
                        pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                        pointLightColors.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
                        lights.push_back(true);
                    }  

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
        lightShader.setVec3("viewPos", camera.Pos);
        lightShader.setFloat("material.shininess", 32.0f);
        lightShader.setInt("currentnumber", pointLightPositions.size());

        lightShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        lightShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    
        for(int i = 0; i<pointLightPositions.size(); i++){
            std::string target ="pointLights[" + std::to_string(i) + "].";

             if(lights[i] == true){
                lightShader.setVec3(target + "position", pointLightPositions[i]);
                lightShader.setVec3(target + "ambient", pointLightColors[i]);
                lightShader.setVec3(target + "diffuse", pointLightColors[i]);
                lightShader.setVec3(target + "specular", pointLightColors[i]);
                lightShader.setFloat(target + "constant", 1.0f);
                lightShader.setFloat(target + "linear", 0.09f);
                lightShader.setFloat(target + "quadratic", 0.032f);
             }
        }
       
        lightShader.setVec3("spotLight.position", camera.Pos);
        lightShader.setVec3("spotLight.direction", camera.Front);
        lightShader.setVec3("spotLight.ambient",  glm::vec3(0.0f, 0.0f, 0.0f));
        lightShader.setVec3("spotLight.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
        lightShader.setVec3("spotLight.specular",  glm::vec3(1.0f, 1.0f, 1.0f));
        lightShader.setFloat("spotLight.constant", 1.0f);
        lightShader.setFloat("spotLight.linear", 0.09f);
        lightShader.setFloat("spotLight.quadratic", 0.032f);
        lightShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        lightShader.setMat4("projection", projection);

       
        glm::mat4 view = camera.GetViewMatrix();
        lightShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        for(int i = 0 ; i < objectPositions.size();i++){
            model = glm::translate(model, objectPositions[i]);
            lightShader.setMat4("model", model);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t1->get());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, t2->get());


        if(numofobj >= 0)
            models[0].Draw(lightShader);

        lightcubeShader.use();
        lightcubeShader.setMat4("projection", projection);
        lightcubeShader.setMat4("view", view);

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < pointLightPositions.size(); i++)
        {
            if(lights[i] == true){
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightcubeShader.setMat4("model", model);
                lightcubeShader.setVec3("lightColor", pointLightColors[i]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        

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

void OnCursorcPos(GLFWwindow* window, double x, double y){
    glfwGetWindowUserPointer(window);
    MouseMove(x, y);
}

void MouseMove(double x, double y){
 
    
    if (!camera.cameraControl)
            return;

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - camera.preMousePos;
  
    const float cameraRotSpeed = 0.5f;
    camera.Yaw += deltaPos.x * cameraRotSpeed;
    camera.Pitch -= deltaPos.y * cameraRotSpeed;

    if (camera.Yaw < 0.0f)   camera.Yaw += 360.0f;
    if (camera.Yaw > 360.0f) camera.Yaw -= 360.0f;

    if (camera.Pitch > 89.0f)  camera.Pitch = 89.0f;
    if (camera.Pitch < -89.0f) camera.Pitch = -89.0f;

    camera.preMousePos = pos;

    

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


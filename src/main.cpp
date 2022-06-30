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
unsigned int loadCubemap(vector<std::string> faces);

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
    // Shader simpleShader("./shader/simple.vs", "./shader/simple.fs");
    Shader skyboxShader("./shader/skybox.vs","./shader/skybox.fs");
 
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
    std::vector<glm::vec3> pointLightAmbient = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };
    std::vector<glm::vec3> pointLightDiffuse = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };
    std::vector<glm::vec3> pointLightSpecular = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)
    };
    std::vector<bool> lights ={
        true,
        true
    };
    std::vector<Model> models ={};
    std::vector<glm::vec3> sizes = {};
    glm::vec3 direcLight = glm::vec3(0.0f, 0.0f, 0.0f);
    bool reset = false;

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


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

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    stbi_set_flip_vertically_on_load(false);
    vector<std::string> faces
    {
        "./image/blue/right.png",
        "./image/blue/left.png",
        "./image/blue/top.png",
        "./image/blue/bottom.png",
        "./image/blue/front.png",
        "./image/blue/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);


    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    lightShader.use();
    lightShader.setInt("material.diffuse", 0);
    lightShader.setInt("material.specular", 1);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);



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
                sizes.push_back(glm::vec3( 5.0f,  5.0f,  5.0f));
                
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
                    char buff4[15] = "Object Size##";
                    for(int i = 0 ; i < objectPositions.size();i++){
                        snprintf(buff, 10, "Object%d", i+1);
                        snprintf(buff2, 20,  "Object Position##%d", i);
                        snprintf(buff3, 12,  "Delete##%d", i);
                        snprintf(buff4, 15,  "Object Size##%d", i);
                        if(ImGui::CollapsingHeader(buff, ImGuiTreeNodeFlags_DefaultOpen)){
                        ImGui::DragFloat3(buff2, glm::value_ptr(objectPositions[i]), 0.05f);
                        ImGui::DragFloat3(buff4, glm::value_ptr(sizes[i]), 0.05f);
                            if(ImGui::Button(buff3)){
                                objectPositions.erase(objectPositions.begin() + i);
                                sizes.erase(sizes.begin() + i);  
                                models.erase(models.begin() + i );  
                            }
                        }
                    }

                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Lights")){  
                    if(ImGui::BeginTabBar("tabs2")){
                        if(ImGui::BeginTabItem("Point Light")){
                                char buff[10] = "Light";
                                char buff2[20] = "Light Position##";
                                char buff3[25] = "Ambient Light##";
                                char buff3_1[25] = "Diffuse Light##";
                                char buff3_2[25] = "Specular Light##";
                                char buff4[12] = "Delete##";
                                char buff5[8] ="On##";
                                char buff6[10] ="Off##";
                                for(int i = 0 ; i < pointLightPositions.size();i++){
                                    snprintf(buff, 10, "Light%d", i+1);
                                    snprintf(buff2, 20,  "Light Position##%d", i+2);
                                    snprintf(buff3, 25,  "Ambient Light##%d", i+2);
                                    snprintf(buff3_1, 25,  "Diffuse Light##%d", i+2);
                                    snprintf(buff3_2, 25,  "Specular Light##%d", i+2);
                                    snprintf(buff4, 12,  "Delete##%d", i+2);
                                    snprintf(buff5, 8,  "On##%d", i+2);
                                    snprintf(buff6, 10,  "Off##%d", i+2);
                                    if(ImGui::CollapsingHeader(buff, ImGuiTreeNodeFlags_DefaultOpen)){
                                        if(lights[i+2] == true){
                                            ImGui::DragFloat3(buff2, glm::value_ptr(pointLightPositions[i]), 0.05f);
                                            ImGui::ColorEdit3(buff3, glm::value_ptr(pointLightAmbient[i+2]), 0.1f);
                                            ImGui::ColorEdit3(buff3_1, glm::value_ptr(pointLightDiffuse[i+2]), 0.1f);
                                            ImGui::ColorEdit3(buff3_2, glm::value_ptr(pointLightSpecular[i+2]), 0.1f);
                                        }
                                        if(ImGui::Button(buff5)){
                                            lights[i+2] = true;
                                        }
                                        ImGui::SameLine();
                                        if(ImGui::Button(buff6)){
                                            lights[i+2] = false;
                                        }
                                        ImGui::SameLine();
                                        if(ImGui::Button(buff4)){
                                            pointLightPositions.erase(pointLightPositions.begin() + (i));
                                            pointLightAmbient.erase(pointLightAmbient.begin() + (i+2));
                                            pointLightDiffuse.erase(pointLightDiffuse.begin() + (i+2));
                                            pointLightSpecular.erase(pointLightSpecular.begin() + (i+2));
                                            lights.erase(lights.begin() +  (i+2));
                                        }
                                    } 
                
                                }
                                if(ImGui::Button("ADD")){ 
                                    if(pointLightPositions.size()==10){
                                        return -1;
                                    }
                                    pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                                    pointLightAmbient.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
                                    pointLightDiffuse.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
                                    pointLightSpecular.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
                                    lights.push_back(true);
                                }  
                                ImGui::EndTabItem();
                        }
                        if(ImGui::BeginTabItem("Directional Light")){
                            if(ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)){
                                if(lights[0] == true){
                                    ImGui::DragFloat3("Direction", glm::value_ptr(direcLight), 0.05f);
                                    ImGui::ColorEdit3("Ambient", glm::value_ptr(pointLightAmbient[0]), 0.1f);
                                    ImGui::ColorEdit3("Diffuse", glm::value_ptr(pointLightDiffuse[0]), 0.1f);
                                    ImGui::ColorEdit3("Specular", glm::value_ptr(pointLightSpecular[0]), 0.1f);
                                }
                                if(ImGui::Button("On##0")){
                                    lights[0] = true;
                                }
                                ImGui::SameLine();
                                if(ImGui::Button("Off##0")){
                                    lights[0] = false;
                                    direcLight = glm::vec3(0.0f,0.0f,0.0f);
                                    pointLightAmbient[0] = glm::vec3(0.0f,0.0f,0.0f);
                                    pointLightDiffuse[0] = glm::vec3(0.0f,0.0f,0.0f);
                                    pointLightSpecular[0] = glm::vec3(0.0f,0.0f,0.0f);
                                    reset = true;
                                }
                            }
                                ImGui::EndTabItem();   
                        }
                        if(ImGui::BeginTabItem("Spot Light")){
                                ImGui::EndTabItem();   
                        }
                        ImGui::EndTabBar();
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

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

            lightShader.use();
            lightShader.setVec3("viewPos", camera.Pos);
            lightShader.setFloat("material.shininess", 32.0f);
            lightShader.setInt("currentnumber", pointLightPositions.size());

            if(lights[0] == true || reset == true){
                lightShader.setVec3("dirLight.direction",direcLight);
                lightShader.setVec3("dirLight.ambient", pointLightAmbient[0]);
                lightShader.setVec3("dirLight.diffuse", pointLightDiffuse[0]);
                lightShader.setVec3("dirLight.specular", pointLightSpecular[0]);
                reset = false;
            }
        
            for(int i = 0; i<pointLightPositions.size(); i++){
                std::string target ="pointLights[" + std::to_string(i) + "].";

                if(lights[i+2] == true){
                    lightShader.setVec3(target + "position", pointLightPositions[i]);
                    lightShader.setVec3(target + "ambient", pointLightAmbient[i+2]);
                    lightShader.setVec3(target + "diffuse", pointLightDiffuse[i+2]);
                    lightShader.setVec3(target + "specular", pointLightSpecular[i+2]);
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

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 1000.0f);
            lightShader.setMat4("projection", projection);

            glm::mat4 view = camera.GetViewMatrix();
            lightShader.setMat4("view", view);
    
            // simpleShader.use();
            // simpleShader.setMat4("view", view);
            // simpleShader.setMat4("projection", projection);

           // glm::mat4 model = glm::mat4(1.0f);
            //lightShader.setMat4("model", model);
            // glEnable(GL_STENCIL_TEST);
            // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            // glStencilFunc(GL_ALWAYS, 1, 0xFF);
            // glStencilMask(0xFF);
            for(int i = 0 ; i < objectPositions.size();i++){
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, objectPositions[i]);
                model = glm::scale(model, sizes[i]);
                lightShader.setMat4("model", model);
                models[i].Draw(lightShader);
            }

            // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            // glStencilMask(0x00);
            // glDisable(GL_DEPTH_TEST);
            // simpleShader.use();
            // for(int i = 0 ; i < objectPositions.size();i++){
            //     glm::mat4 model = glm::mat4(1.0f);
            //     model = glm::translate(model, glm::vec3(objectPositions[i].x,objectPositions[i].y,objectPositions[i].z));
            //     //model = glm::scale(model, glm::vec3(1.1f,1.1f,1.1f));
            //     lightShader.setMat4("model", model);
            //     models[i].Draw(lightShader);
            // }
     
            // glEnable(GL_DEPTH_TEST);
            // glDisable(GL_STENCIL_TEST);
            // glStencilFunc(GL_ALWAYS, 1, 0xFF);
            // glStencilMask(0xFF);
       

        lightcubeShader.use();
        lightcubeShader.setMat4("projection", projection);
        lightcubeShader.setMat4("view", view);

        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < pointLightPositions.size(); i++)
        {
            if(lights[i+2] == true){
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightcubeShader.setMat4("model", model);
                lightcubeShader.setVec3("lightColor", pointLightDiffuse[i+2]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        
        
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

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    return textureID;
}



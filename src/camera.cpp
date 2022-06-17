#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up,bool cameracontrol, glm::vec2 premousepos, float yaw, float pitch): Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
{
    cameraControl = cameracontrol;
    preMousePos = premousepos;
    Pos = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVector();
}

void Camera::MouseMovement(float xoffset, float yoffset, GLboolean constrainPitch ){
  
    float sensitivity = 0.1f; 
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    
    Yaw += xoffset;
    Pitch += yoffset;
    
    

    if(constrainPitch){
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
        if (Yaw < 0.0f)
            Yaw += 360.0f;
        if (Yaw > 360.f)
            Yaw -= 360.0f;
    }
    

    updateCameraVector();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Pos, Pos + Front, Up);
}


void Camera::MouseScroll(double yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}


void Camera::KeyboardProcess(CameraMovement direction, float deltaTime){

    float cameraSpeed = 0.02f;
    if(direction == FORWARD)
        Pos += Front * cameraSpeed;
    if(direction == BACKWARD)
        Pos -= Front * cameraSpeed;
    if(direction == LEFT)
        Pos -= Right * cameraSpeed;
    if(direction == RIGHT)
        Pos += Right * cameraSpeed;
}

void Camera::MouseButton(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
        preMousePos = glm::vec2((float)x, (float)y);
        cameraControl = true;
    }
    else if (action == GLFW_RELEASE) {
        cameraControl = false;
    }
  }
}

void Camera::updateCameraVector(){

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));

}


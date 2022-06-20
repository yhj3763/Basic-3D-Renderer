#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up,bool cameracontrol, glm::vec2 premousepos, float yaw, float pitch)
{
    cameraControl = cameracontrol;
    preMousePos = premousepos;
    Pos = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Front = glm::vec3(0.0f, -1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Pos, Pos + Front, Up);
}


void Camera::KeyboardProcess(CameraMovement direction, float deltaTime){

    float cameraSpeed = 0.02f;
    
    if(direction == FORWARD)
        Pos += Front * cameraSpeed;
    if(direction == BACKWARD)
        Pos -= Front * cameraSpeed;

    auto cameraRight = glm::normalize(glm::cross(Up, -Front));
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

void Camera::MouseScroll(float yoffset)
{
     Zoom -= (float)yoffset;
     if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f; 
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


#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         =  -90.0f;
const float PITCH       =  0.0f;


class Camera{

    public:
        bool cameraControl;
        glm::vec2 preMousePos;
        glm::vec3 Pos;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        float Yaw;
        float Pitch;
        float Zoom;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), bool cameracontrol = false, glm::vec2 premousepos = glm::vec2(0.0f, 0.0f),  float yaw = YAW, float pitch = PITCH);
        glm::mat4 GetViewMatrix();
        void MouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
        void MouseScroll(float yoffset);
        void KeyboardProcess(CameraMovement direction, float deltaTime);
        void MouseButton(int button, int action, double x, double y);
        void updateCameraVector();
};



#endif
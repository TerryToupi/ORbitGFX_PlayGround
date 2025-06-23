#ifndef __EDITOR_CAMERA_HPP__
#define __EDITOR_CAMERA_HPP__

#include <math.hpp>

class EditorCamera {
public:
    EditorCamera();

    EditorCamera(Vec3 startPos);

    Mat4 getViewProjMatrix() const;
    Vec3 getCameraPos() const;

    void ProcessMouseMovement(float xoffset, float yoffset, float sensitivity = 0.1f);

    void ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime, float speed = 2.5f);

private:
    Vec3 m_Position;
    float m_Yaw;
    float m_Pitch;
    Vec3 m_Up;

    Mat4 m_Proj;
};

#endif // !__EDITOR_CAMERA_HPP__

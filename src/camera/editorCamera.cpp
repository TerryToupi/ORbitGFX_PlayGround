#include <camera/editorCamera.hpp>

EditorCamera::EditorCamera()
    : m_Position(Vec3(0.0, 0.0, 0.0)), m_Yaw(-90.0f), m_Pitch(0.0f), m_Up(0.0f, 1.0f, 0.0f), m_Proj(glm::identity<Mat4>())
{
}

EditorCamera::EditorCamera(Vec3 startPos)
	: m_Position(startPos), m_Yaw(-90.0f), m_Pitch(0.0f), m_Up(0.0f, 1.0f, 0.0f)
{ 
	m_Proj = glm::perspectiveZO(glm::radians(60.0f), 21.0f / 9.0f, 0.01f, 100.0f);
}

Mat4 EditorCamera::getViewProjMatrix() const
{
        Vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

        Mat4 view = glm::lookAt(m_Position, m_Position + glm::normalize(front), m_Up);
        return m_Proj * view;
}

Vec3 EditorCamera::getCameraPos() const
{
    return m_Position;
}

void EditorCamera::ProcessMouseMovement(float xoffset, float yoffset, float sensitivity)
{
    m_Yaw += xoffset * sensitivity;
    m_Pitch += yoffset * sensitivity;
    // Clamp pitch
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;
}

void EditorCamera::ProcessKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime, float speed)
{
    Vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = 0.0f; // no flying for FPS
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front = glm::normalize(front);
    Vec3 rightVec = glm::normalize(glm::cross(front, m_Up));

    float velocity = speed * deltaTime;
    if (forward) m_Position += front * velocity;
    if (backward) m_Position -= front * velocity;
    if (left) m_Position -= rightVec * velocity;
    if (right) m_Position += rightVec * velocity;
}

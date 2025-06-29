#ifndef __EDITOR_CAMERA_HPP__
#define __EDITOR_CAMERA_HPP__

#include <math.hpp> 

// Default camera values
const float YAW = -90.0f; // Yaw is initially -90.0 degrees to look along the negative Z-axis
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;

class EditorCamera {
public:
    // Default constructor: initializes camera at origin looking forward
    EditorCamera(Vec3 position = Vec3(0.0f, 0.0f, 0.0f), Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // Returns the combined view-projection matrix
    Mat4 getViewProjMatrix() const;
    // Returns the current position of the camera
    Vec3 getCameraPos() const;

    /**
     * @brief Processes mouse movement for looking around (yaw and pitch).
     *
     * @param deltaX Horizontal change in mouse position since last frame.
     * @param deltaY Vertical change in mouse position since last frame.
     * @param sensitivity Multiplier for mouse look speed.
     */
    void processMouseMovement(float x, float y, bool active, float sensitivity = SENSITIVITY);

    /**
     * @brief Processes keyboard input for moving the camera directly.
     *
     * @param forward True if forward key is pressed.
     * @param backward True if backward key is pressed.
     * @param left True if left key is pressed.
     * @param right True if right key is pressed.
     * @param deltaTime Time elapsed since last frame, for frame-rate independent movement.
     * @param speed Movement speed multiplier.
     */
    void processKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime, float speed = SPEED);

    /**
     * @brief Sets the aspect ratio of the projection matrix.
     * Call this when the window/viewport resizes.
     * @param aspect The new aspect ratio (width / height).
     */
    void setAspect(float aspect);

private:
    // Camera attributes
    Vec3 m_Position;    // The actual 3D position of the camera
    Vec3 m_Front;       // The forward direction vector of the camera
    Vec3 m_Up;          // The calculated up direction vector relative to camera's orientation
    Vec3 m_Right;       // The calculated right direction vector relative to camera's orientation
    Vec3 m_WorldUp;     // The fixed world up direction (e.g., (0, 1, 0))

    // Euler Angles
    float m_Yaw;        // Rotation angle around the Y-axis (vertical axis)
    float m_Pitch;      // Rotation angle around the X-axis (horizontal axis)

    // Projection attributes
    float m_Fov;        // Field of View in degrees
    float m_Aspect;     // Aspect ratio of the viewport (width / height)
    float m_Near;       // Near clipping plane distance
    float m_Far;        // Far clipping plane distance

    Mat4 m_Proj;        // Projection matrix

    double m_MouseX;
    double m_MouseY;

    /**
     * @brief Internal helper function to recalculate the camera's front, right, and up vectors.
     * This function updates `m_Front`, `m_Right`, and `m_Up` based on `m_Yaw` and `m_Pitch`.
     * It should be called after any changes to these angles to ensure the camera's orientation
     * is consistent for movement and view matrix generation.
     */
    void updateCameraVectors();
};

#endif // !__EDITOR_CAMERA_HPP__


#ifdef __EDITOR_CAM_IMPLEMENTATION__

/**
 * @brief Default constructor for EditorCamera (FPS style).
 * Initializes the camera at a given position with a default orientation.
 *
 * @param position The initial 3D position of the camera.
 * @param worldUp The world's up direction vector, used to calculate camera's right/up vectors.
 * @param yaw The initial yaw angle (rotation around Y-axis).
 * @param pitch The initial pitch angle (rotation around X-axis).
 */
EditorCamera::EditorCamera(Vec3 position, Vec3 worldUp, float yaw, float pitch)
    : m_Position(position),
    m_WorldUp(worldUp),
    m_Yaw(yaw),
    m_Pitch(pitch),
    m_Fov(FOV), m_Aspect(21.0f / 9.0f), m_Near(0.01f), m_Far(100.0f),
    m_MouseX(0.0f),
    m_MouseY(0.0f)
{
    // Initialize the projection matrix using left-handed, zero-to-one depth range
    m_Proj = glm::perspectiveLH_ZO(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
    // Calculate the initial camera vectors based on yaw and pitch
    updateCameraVectors();
}

/**
 * @brief Recalculates the camera's front, right, and up vectors.
 * These vectors are crucial for constructing the view matrix and handling movement.
 */
void EditorCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    Vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down.
    // This is a normalisation issue.
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

/**
 * @brief Gets the combined view-projection matrix for rendering.
 * The view matrix is calculated using glm::lookAtLH, pointing from m_Position along m_Front.
 *
 * @return A glm::mat4 representing the view-projection matrix.
 */
Mat4 EditorCamera::getViewProjMatrix() const
{
    // Calculate the view matrix: look from m_Position towards m_Position + m_Front
    Mat4 view = glm::lookAtLH(m_Position, m_Position + m_Front, m_Up);
    // Return the combined projection and view matrix
    return m_Proj * view;
}

/**
 * @brief Gets the current 3D position of the camera.
 *
 * @return A glm::vec3 representing the camera's current position.
 */
Vec3 EditorCamera::getCameraPos() const
{
    return m_Position;
}

/**
 * @brief Processes mouse movement, directly updating the camera's yaw and pitch.
 * This simulates "looking around" in a first-person view.
 *
 * @param deltaX The change in mouse X position since the last frame.
 * @param deltaY The change in mouse Y position since the last frame.
 * @param sensitivity Multiplier for the speed of mouse look.
 */
void EditorCamera::processMouseMovement(float x, float y, bool active, float sensitivity)
{
    double deltaX = x - m_MouseX;
    double deltaY = y - m_MouseY;

    m_MouseX = x;
    m_MouseY = y;

    if (active)
    {
		// Adjust yaw and pitch based on mouse deltas and sensitivity
		m_Yaw += deltaX * sensitivity;
		m_Pitch -= deltaY * sensitivity; // Invert Y for typical FPS mouse look (positive deltaY means mouse moves up, camera looks down)

		// Clamp pitch to prevent flipping the camera (looking too far up or down)
		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		// After pitch/yaw change, recalculate the camera's front, right, and up vectors
		updateCameraVectors();
    }
}

/**
 * @brief Processes keyboard input for moving the camera directly in its local space.
 *
 * @param forward True if the "move forward" key is pressed.
 * @param backward True if the "move backward" key is pressed.
 * @param left True if the "move left" (strafe left) key is pressed.
 * @param right True if the "move right" (strafe right) key is pressed.
 * @param deltaTime The time elapsed since the last frame, for frame-rate independent movement.
 * @param speed The base movement speed.
 */
void EditorCamera::processKeyboard(bool forward, bool backward, bool left, bool right, float deltaTime, float speed)
{
    float velocity = speed * deltaTime; // Calculate the distance to move this frame

    // Update camera position based on keyboard input and camera's local vectors
    if (forward) {
        m_Position += m_Front * velocity;
    }
    if (backward) {
        m_Position -= m_Front * velocity;
    }
    if (left) {
        m_Position -= m_Right * velocity; // Move left is subtracting from the right vector
    }
    if (right) {
        m_Position += m_Right * velocity; // Move right is adding to the right vector
    }
}

/**
 * @brief Sets the aspect ratio for the camera's projection matrix.
 * This should be called when the viewport (window) size changes.
 *
 * @param aspect The new aspect ratio (width / height).
 */
void EditorCamera::setAspect(float aspect)
{
    m_Aspect = aspect;
    m_Proj = glm::perspectiveLH_ZO(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
}

#endif

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// ���������� ��������� ��������� ��������� �������� ������. ������������ � �������� ����������, ����� ��������� �������� �� ����������� ��� ������� ������� ������� �����
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    NO
};

// ��������� ������ �� ���������
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
bool OnGround = true;
bool InBounce = false;
bool InFall = false;
float FallSpeed = 10.0f;
float BounceSpeed = 20.0f;
float Pi = 3.14159265f;
int TimeInBounce = 0;
int map[256][256];

// ����������� ����� ������, ������� ������������ ������� ������ � ��������� ��������������� ���� ������, ������� � ������� ��� ������������� � OpenGL
class Camera
{
public:
    // �������� ������
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
	
    // ���� ������
    float Yaw;
    float Pitch;
	
    // ��������� ������
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // �����������, ������������ �������
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
	
    // �����������, ������������ �������
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // ���������� ������� ����, ����������� � �������������� ����� ������ � LookAt-������� 
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    float getMapHeight()
    {
        float y = map[(int)Position.x][(int)Position.z];
        return y;
    }

    void Collision(float deltaTime)
    {
        float DownVelocity = FallSpeed * deltaTime;
        float UpVelocity = BounceSpeed * deltaTime;
        float y = getMapHeight();

        if (y < Position.y - 1.7)
        {
            OnGround = false;
        }
        else
        {
            OnGround = true;
            InFall = false;
        }

        if (OnGround == false && InBounce == false)
            InFall = true;

        if (InBounce == true)
        {
            Position.y += UpVelocity;
            TimeInBounce++;
        }

        if (TimeInBounce > 6)
        {
            InBounce = false;
            TimeInBounce = 0;
        }

        if (OnGround == false)
            Position.y -= DownVelocity;
    }

    bool CheckObstacle(glm::vec3 Move)
    {
        glm::vec3 NewPosition = Move;
        if (OnGround == true)
            if (map[(int)Position.x][(int)Position.z] >= map[(int)NewPosition.x][(int)NewPosition.z])
                return true;
            else
                return false;
        else
            if (Position.y - 1.7f >= map[(int)NewPosition.x][(int)NewPosition.z])
                return true;
            else
                return false;
    }

    // ������������ ������� ������, ���������� �� ������������ ������� �����. ��������� ������� �������� � ���� ������������� ������� ������������ (��� ��������������� ��� �� ������� ������)
    void ProcessKeyboard(Camera_Movement directionX, Camera_Movement directionY, Camera_Movement directionZ, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        float upVelocity = BounceSpeed * deltaTime;
        glm::vec3 front = glm::cross(WorldUp, Right);

        if (directionY == UP && OnGround == true)
        {
            if (directionZ == NO)
            {
                if (directionX == NO)
                {
                    InBounce = true;
                }
                else
                {
                    InBounce = true;
                    if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                        Position += front * velocity * 0.75f;
                    if (directionX == BACKWARD && CheckObstacle(Position -front * velocity))
                        Position -= front * velocity * 0.75f;
                    if (directionX == LEFT && CheckObstacle(Position - Right * velocity))
                        Position -= Right * velocity * 0.75f;
                    if (directionX == RIGHT && CheckObstacle(Position + Right * velocity))
                        Position += Right * velocity * 0.75f;
                }
            }
            else
            {
                InBounce = true;
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                    Position += front * velocity * 0.5f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity * 0.25f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity * 0.25f;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity))
                    Position -= front * velocity * 0.5f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity * 0.25f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity * 0.25f;
            }
        }

        if (directionY == NO && (InBounce == true || InFall == true))
        {
            if (directionZ == NO)
            {
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                    Position += front * velocity;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity))
                    Position -= front * velocity;
                if (directionX == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity;
                if (directionX == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity;
                /*
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                    Position += front * velocity * 0.75f;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity))
                    Position -= front * velocity * 0.75f;
                if (directionX == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity * 0.75f;
                if (directionX == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity * 0.75f;
                */
            }
            else
            {
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity * 0.7f))
                    Position += front * velocity * 0.7f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity * 0.7f))
                    Position -= Right * velocity * 0.35f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity * 0.7f))
                    Position += Right * velocity * 0.35f;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity * 0.7f))
                    Position -= front * velocity * 0.7f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity * 0.7f))
                    Position -= Right * velocity * 0.35f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity * 0.7f))
                    Position += Right * velocity * 0.35f;
                /*
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                    Position += front * velocity * 0.5f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity * 0.25f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity * 0.25f;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity))
                    Position -= front * velocity * 0.5f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity * 0.25f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity * 0.25f;
                */
            }
        }

        if (directionY == NO && OnGround == true)
        {
            if (directionZ == NO)
            {
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity))
                    Position += front * velocity;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity))
                    Position -= front * velocity;
                if (directionX == LEFT && CheckObstacle(Position - Right * velocity))
                    Position -= Right * velocity;
                if (directionX == RIGHT && CheckObstacle(Position + Right * velocity))
                    Position += Right * velocity;
            }
            else
            {
                if (directionX == FORWARD && CheckObstacle(Position + front * velocity * 0.7f))
                    Position += front * velocity * 0.7f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity * 0.7f))
                    Position -= Right * velocity * 0.35f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity * 0.7f))
                    Position += Right * velocity * 0.35f;
                if (directionX == BACKWARD && CheckObstacle(Position - front * velocity * 0.7f))
                    Position -= front * velocity * 0.7f;
                if (directionZ == LEFT && CheckObstacle(Position - Right * velocity * 0.7f))
                    Position -= Right * velocity * 0.35f;
                if (directionZ == RIGHT && CheckObstacle(Position + Right * velocity * 0.7f))
                    Position += Right * velocity * 0.35f;
            }
        }
    }

    // ������������ ������� ������, ���������� �� ������� ����� � ������� ����. ������� � �������� ���������� �������� �������� ��� � ����������� x, ��� � � ����������� y
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // ����������, ��� ����� ������ ������� �� ������� ������, ����� �� ����������������
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // ��������� �������� �������-�����, �������-������ � �������-�����, ��������� ����������� �������� ����� ������
        updateCameraVectors();
    }

    // ������������ ������� ������, ���������� �� ������� ������ ��������� ����. ���������� ������ ������� ������ �� ������������ ��� �������� 
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:
    // ��������� ������-����� �� (�����������) ����� ������ ������
    void updateCameraVectors()
    {
        // ��������� ����� ������-�����
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
		
        // ����� ������������� ������-������ � ������-�����
        Right = glm::normalize(glm::cross(Front, WorldUp)); // ����������� �������, ������ ��� �� ����� ��������� � 0 ��� ������, ��� ������ �� �������� ����� ��� ����, ��� �������� � ����� ���������� ��������
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
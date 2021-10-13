#pragma once

#include "vect.h"


class CCamera {
    CVec position;
    CVec direction;
    CVec forward_velocity;
    float yaw;
    float pitch;
    float roll;

public:


    void init_camera(CVec position, CVec direction);

    CVec get_camera_position(void);
    CVec get_camera_direction(void);
    CVec get_camera_forward_velocity(void);
    float get_camera_yaw(void);
    float get_camera_pitch(void);

    void update_camera_position(CVec position);
    void update_camera_direction(CVec direction);
    void update_camera_forward_velocity(CVec forward_velocity);

    void rotate_camera_yaw(float angle);
    void rotate_camera_pitch(float angle);

    CVec get_camera_lookat_target(void);
};

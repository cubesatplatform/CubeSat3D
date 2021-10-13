#include "camera.h"
#include "matrix.h"

//static camera_t camera;

void CCamera::init_camera(CVec pos, CVec dir) {
    position = pos;
    direction = dir;
    forward_velocity = CVec(0, 0, 0);
    yaw = 0.0;
    pitch = 0.0;
};

CVec CCamera::get_camera_position(void) {
    return position;
}

CVec CCamera::get_camera_direction(void) {
    return direction;
}

CVec CCamera::get_camera_forward_velocity(void) {
    return forward_velocity;
}

float CCamera::get_camera_yaw(void) {
    return yaw;
}

float CCamera::get_camera_pitch(void) {
    return pitch;
}

void CCamera::update_camera_position(CVec pos) {
    position = pos;
}

void CCamera::update_camera_direction(CVec dir) {
    direction = dir;
}

void CCamera::update_camera_forward_velocity(CVec fv) {
    forward_velocity = fv;
}

void CCamera::rotate_camera_yaw(float angle) {
    yaw += angle;
}

void CCamera::rotate_camera_pitch(float angle) {
    pitch += angle;
}

CVec CCamera::get_camera_lookat_target(void) {
    // Initialize the target looking at the positive z-axis
    CVec target = { 0, 0, 1 };

    CMatrix camera_yaw_rotation;
    CMatrix camera_pitch_rotation;
  
    camera_yaw_rotation.rotate_y(yaw);
    camera_pitch_rotation.rotate_x(pitch);

    // Create camera rotation matrix based on yaw and pitch
    CMatrix camera_rotation ;
    
    camera_rotation.identity();
    camera_rotation = camera_pitch_rotation * camera_rotation;
    camera_rotation = camera_yaw_rotation * camera_rotation;
 
    // Update camera direction based on the rotation
    CVec  camera_direction = camera_rotation *target ;
    direction = camera_direction;

    // Offset the camera position in the direction where the camera is pointing at
    target = position+ direction;
    target.w = 1;
    return target;
}

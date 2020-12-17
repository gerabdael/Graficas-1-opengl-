/*
 tdogl::Camera

 Copyright 2012 Thomas Dalling - http://tomdalling.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#define _USE_MATH_DEFINES
#include <cmath>
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace tdogl;

static const float MaxVerticalAngle = 89.9f; //must be less than 90 to avoid gimbal lock

Camera::Camera(float width, float height) :
	_position(0.0f, 0.0f, 0.0f),
	_horizontalAngle(0.0f),
	_verticalAngle(0.0f),
	_fieldOfView(50.0f),
	_nearPlane(0.01f),
	_farPlane(10000.0f),
	_viewportAspectRatio(width / height)
{
	_screenWidth = width;
	_screenHeight = height;
	_cameraType = false;
}

const glm::vec3& Camera::position() const {
	glm::vec3 eye = _position;

	if (_cameraType) {
		glm::vec3 center = glm::vec3(orientation() * glm::vec4(0, 0, -1, 1));
		eye -= center * _ratio;
	}

	return glm::vec3(eye);
}

void Camera::setPosition(const glm::vec3& position) {
	_position = position;
}

void Camera::setHeight(const float& height) {
	_position.y = height;
}

void Camera::setCameraType(const bool& thirdPerson) {
	_cameraType = thirdPerson;
}

void Camera::setRatio(const float& ratio) {
	_ratio = ratio;
}

void Camera::offsetPosition(const glm::vec3& offset) {
	_position += offset;
}

float Camera::fieldOfView() const {
	return _fieldOfView;
}

void Camera::setFieldOfView(float fieldOfView) {
	assert(fieldOfView > 0.0f && fieldOfView < 180.0f);
	_fieldOfView = fieldOfView;
}

float Camera::nearPlane() const {
	return _nearPlane;
}

float Camera::farPlane() const {
	return _farPlane;
}

void Camera::setNearAndFarPlanes(float nearPlane, float farPlane) {
	assert(nearPlane > 0.0f);
	assert(farPlane > nearPlane);
	_nearPlane = nearPlane;
	_farPlane = farPlane;
}

glm::mat4 Camera::orientation() const {
	glm::mat4 orientation;
	orientation = glm::rotate(orientation, glm::radians(_verticalAngle), glm::vec3(1, 0, 0));
	orientation = glm::rotate(orientation, glm::radians(_horizontalAngle), glm::vec3(0, 1, 0));
	return glm::inverse(orientation);
}

void Camera::offsetOrientation(float upAngle, float rightAngle) {
	_horizontalAngle += rightAngle;
	_verticalAngle += upAngle;
	normalizeAngles();
}

float Camera::viewportAspectRatio() const {
	return _viewportAspectRatio;
}

void Camera::setViewportAspectRatio(float viewportAspectRatio) {
	assert(viewportAspectRatio > 0.0);
	_viewportAspectRatio = viewportAspectRatio;
}

glm::vec3 Camera::forward() const {
	glm::vec4 forward = orientation() * glm::vec4(0, 0, -1, 1);
	return glm::vec3(forward);
}

glm::vec3 Camera::right() const {
	glm::vec4 right = orientation() * glm::vec4(1, 0, 0, 1);
	return glm::vec3(right);
}

glm::vec3 Camera::up() const {
	glm::vec4 up = orientation() * glm::vec4(0, 1, 0, 1);
	return glm::vec3(up);
}

glm::mat4 Camera::perspectiveMatrix() const {
	return projection() * view();
}

glm::mat4 Camera::orthographicMatrix() const {
	return orthographic();
}

glm::mat4 Camera::projection() const {
	return glm::perspective(glm::radians(_fieldOfView), _viewportAspectRatio, _nearPlane, _farPlane);
}

glm::mat4 Camera::orthographic() const {
	return glm::ortho(0.0f, _screenWidth, _screenHeight, 0.0f, 0.0f, 100.0f);
}

glm::mat4 Camera::view() const {
	glm::vec4 eye = glm::vec4(0, 0, 0, 1);
	glm::vec4 center = glm::vec4(0, 0, -1, 1);
	glm::vec4 up = glm::vec4(0, 1, 0, 1);

	eye = glm::translate(glm::mat4(), _position) * eye;
	center = orientation() * center;
	up = orientation() * up;

	if (_cameraType) {
		glm::vec4 temp = eye;
		eye = eye - (center * _ratio);
		center = temp;
	}
	else {
		center += eye;
	}

	return glm::lookAt(glm::vec3(eye), glm::vec3(center), glm::vec3(up));
}

void Camera::normalizeAngles() {
	_horizontalAngle = fmodf(_horizontalAngle, 360.0f);
	//fmodf can return negative values, but this will make them all positive
	if (_horizontalAngle < 0.0f)
		_horizontalAngle += 360.0f;

	if (_verticalAngle > MaxVerticalAngle)
		_verticalAngle = MaxVerticalAngle;
	else if (_verticalAngle < -MaxVerticalAngle)
		_verticalAngle = -MaxVerticalAngle;
}
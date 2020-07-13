/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 		Tim Dykes and Ian Cant
 * 		University of Portsmouth
 *
 */

#include "Camera.h"

namespace previewer
{
	vec3f Camera::GetCameraPosition()
	{
		return cameraPosition;
	}
	vec3f Camera::GetLookAt()
	{
		return cameraLookAt;
	}
	vec3f Camera::GetUpVector()
	{
		return cameraUpVector;
	}
	vec3f Camera::GetTarget()
	{
		return cameraTarget;
	}
	void Camera::SetLookAt(vec3f target)
	{
		// Calculate relative direction from the current position
		cameraLookAt =  target - cameraPosition;
		cameraLookAt.Normalize();

		// Normalize the right and up vectors
		cameraRightVector = crossprod(cameraUpVector, cameraLookAt);
		cameraRightVector.Normalize();

		cameraUpVector = crossprod(cameraLookAt, cameraRightVector);
		cameraUpVector.Normalize();

		// Construct a view matrix (special case)
		ConstructViewMatrix(false);
		return;
	}

	void Camera::SetFieldOfView(float fov)
	{
		cameraFieldOfView = fov;
		return;
	}
	
	void Camera::SetMaxAcceleration(vec3f ma)
	{
		maxAcceleration = ma;
	}

	void Camera::SetMaxSpeed(vec3f ms)
	{
		maxSpeed = ms;
	}


	void Camera::Create(BoundingBox box)
	{
		// Look at the bounding box
		LookAtBox(box, FRONT);
		return;
	}

	void Camera::Create(vec3f position, vec3f lookAt)
	{
		// Call the more useful overloaded function
		Create(position, lookAt, vec3f(0, 0, 1));
		return;
	}

	void Camera::Create(vec3f position, vec3f lookAt, vec3f up)
	{
		// Set cameras position, up vector and fov (no modifications needed here)
		cameraPosition = position;
		cameraUpVector = up;
		velocity = vec3f(0,0,0);

		// Set the look at point for the camera (the cameras right vector is calculated here!)
		SetTarget(lookAt);
		return;
	}

	void Camera::LookAtBox(BoundingBox box)
	{
		// Call the more useful overloaded function
		LookAtBox(box, FRONT);
		return;
	}

	void Camera::LookAtBox(BoundingBox box, int face)
	{
		// Find the correct camera position to look at the box given the prescribed
		// field of view
		switch(face)
		{
			case FRONT:
			{
				float distanceWidth = (((box.maxX - box.minX)/2)) / (tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxY;
				float distanceHeight = (((box.maxZ - box.minZ)/2)) / (tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxY;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = (box.maxX + box.minX)/2;
				cameraPosition.y = distanceFromBox;
				cameraPosition.z = (box.maxZ + box.minZ)/2;

				cameraUpVector = vec3f(0, 0, 1);
				break;
			}

			case LEFT:
			{
				float distanceWidth = (((box.maxY - box.minY)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxX;
				float distanceHeight = (((box.maxZ - box.minZ)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxX;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = distanceFromBox;
				cameraPosition.y = (box.maxY + box.minY)/2;
				cameraPosition.z = (box.maxZ + box.minZ)/2;

				cameraUpVector = vec3f(0, 0, 1);
				break;
			}

			case RIGHT:
			{
				float distanceWidth = (((box.maxY - box.minY)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxX;
				float distanceHeight = (((box.maxZ - box.minZ)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxX;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = -distanceFromBox;
				cameraPosition.y = (box.maxY + box.minY)/2;
				cameraPosition.z = (box.maxZ + box.minZ)/2;

				cameraUpVector = vec3f(0, 0, 1);
				break;
			}

			case BACK:
			{
				float distanceWidth = (((box.maxX - box.minX)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxY;
				float distanceHeight = (((box.maxZ - box.minZ)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxY;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = (box.maxX + box.minX)/2;
				cameraPosition.y = -distanceFromBox;
				cameraPosition.z = (box.maxZ + box.minZ)/2;

				cameraUpVector = vec3f(0, 0, 1);
				break;
			}

			case TOP:
			{
				float distanceWidth = (((box.maxX - box.minX)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxZ;
				float distanceHeight = (((box.maxY - box.minY)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxZ;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = (box.maxX + box.minX)/2;
				cameraPosition.y = (box.maxY + box.minY)/2;
				cameraPosition.z = distanceFromBox;

				cameraUpVector = vec3f(0, 1, 0);
				break;
			}

			case BOTTOM:
			{
				float distanceWidth = (((box.maxX - box.minX)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxZ;
				float distanceHeight = (((box.maxY - box.minY)/2) / tan(Math::degreesToRadians(cameraFieldOfView/2))) + box.maxZ;

				float distanceFromBox = (distanceWidth > distanceHeight) ? distanceWidth : distanceHeight;

				cameraPosition.x = (box.maxX + box.minX)/2;
				cameraPosition.y = (box.maxY + box.minY)/2;
				cameraPosition.z = -distanceFromBox;

				cameraUpVector = vec3f(0, 1, 0);
				break;
			}
		}

		// Set the look at point for the camera (the cameras right vector is calculated here)
		SetTarget(box.centerPoint);
		return;
	}

	void Camera::SetTarget(vec3f inTarget)
	{
		cameraTarget = inTarget;
		SetLookAt(cameraTarget);
	}

	void Camera::Rotate(vec3f rotation)
	{
		// Call overloaded member
		Rotate(rotation.x, rotation.y, rotation.z);
		return;
	}
	void Camera::Rotate(float yaw, float pitch, float roll)
	{
		// Create a rotation matrix
		Matrix4 rotationMatrix;

		if(yaw != 0.0f)
		{
			// Create the yaw component in the roation matrix
			rotationMatrix.rotate(cameraUpVector, yaw);

			// Transform the right and look at vector values
			cameraRightVector = cameraRightVector * rotationMatrix;
			cameraLookAt = cameraLookAt * rotationMatrix;
		}

		if(pitch != 0.0f)
		{
			// Create the pitch component in the rotation matrix
			rotationMatrix.rotate(cameraRightVector, pitch);

			// Transform the up and look at vector values
			cameraUpVector = cameraUpVector * rotationMatrix;
			cameraLookAt = cameraLookAt * rotationMatrix;
		}

		if(roll != 0.0f)
		{
			// Create the roll component in the rotation matrix
			rotationMatrix.rotate(cameraLookAt, roll);

			// Transform the right and up vector values
			cameraUpVector = cameraUpVector * rotationMatrix;
			cameraRightVector = cameraRightVector * rotationMatrix;
		}

		return;
	}
	void Camera::RotateYaw(float yaw)
	{
		// Create a rotation matrix
		Matrix4 rotationMatrix;

		if(yaw != 0.0f)
		{
			// Create the yaw component in the roation matrix
			rotationMatrix.rotate(cameraUpVector, yaw);

			// Transform the right and look at vector values
			cameraRightVector = cameraRightVector * rotationMatrix;
			cameraLookAt = cameraLookAt * rotationMatrix;
		}

		return;
	}
	void Camera::RotatePitch(float pitch)
	{
		// Create a rotation matrix
		Matrix4 rotationMatrix;

		if(pitch != 0.0f)
		{
			// Create the pitch component in the rotation matrix
			rotationMatrix.rotate(cameraRightVector, pitch);

			// Transform the up and look at vector values
			cameraUpVector = cameraUpVector * rotationMatrix;
			cameraLookAt = cameraLookAt * rotationMatrix;
		}

		return;
	}
	void Camera::RotateRoll(float roll)
	{
		// Create a rotation matrix
		Matrix4 rotationMatrix;

		if(roll != 0.0f)
		{
			// Create the roll component in the rotation matrix
			rotationMatrix.rotate(cameraLookAt, roll);

			// Transform the right and up vector values
			cameraUpVector = cameraUpVector * rotationMatrix;
			cameraRightVector = cameraRightVector * rotationMatrix;
		}

		return;
	}

	void Camera::RotateAroundTarget(float yaw, float pitch, float roll)
	{
		Matrix4 rotationMtxH;
		Matrix4 rotationMtxP;

		vec3f focusVector = cameraPosition - cameraTarget;

		if(yaw != 0.0f)
		{
			rotationMtxH.rotate(cameraUpVector, yaw);
			focusVector = focusVector * rotationMtxH;
		}

		if(pitch != 0.0f)
		{
			rotationMtxP.rotate(cameraRightVector, pitch);
			focusVector = focusVector * rotationMtxP;
		}

		if(roll != 0.0f)
		{
			// Ignore roll for now
		}

		cameraPosition = focusVector + cameraTarget;

		SetLookAt(cameraTarget);
	}

	void Camera::RotateTargetAround(float yaw, float pitch, float roll)
	{
		Matrix4 rotationMtxH;
		Matrix4 rotationMtxP;

		vec3f focusVector = cameraTarget - cameraPosition;

		if(yaw != 0.0f)
		{
			rotationMtxH.rotate(cameraUpVector, yaw);
			focusVector = focusVector * rotationMtxH;
		}

		if(pitch != 0.0f)
		{
			rotationMtxP.rotate(cameraRightVector, pitch);
			focusVector = focusVector * rotationMtxP;
		}

		if(roll != 0.0f)
		{
			// Ignore roll for now
		}

		cameraTarget = focusVector + cameraPosition;

		SetLookAt(cameraTarget);
	}

	void Camera::Move(vec3f movement)
	{
		// Move the camera relative to world axes
		cameraPosition += movement;
		return;
	}
	void Camera::Move(float x, float y, float z)
	{
		// Move the camera relative to world axes
		cameraPosition.x += x;
		cameraPosition.y += y;
		cameraPosition.z += z;
		return;
	}
	void Camera::MoveForward(float distance)
	{
		// Transform movement relative to local axes 
		cameraPosition += cameraLookAt * distance;
		return;
	}
	void Camera::MoveUpward(float distance)
	{
		// Transform movement relative to local axes 
		cameraPosition += cameraUpVector * distance;
		return;
	}
	void Camera::MoveRight(float distance)
	{
		// Transform movement relative to local axes 
		cameraPosition += cameraRightVector * distance;
		return;
	}

	void Camera::MoveCamAndTargetForward(float distance)
	{
		// Transform movement relative to local axes 
		cameraPosition += cameraLookAt * distance;
		cameraTarget += cameraLookAt * distance;
		return;
	}
	void Camera::MoveCamAndTargetUpward(float distance)
	{
		// Transform movement relative to local axes 
		cameraTarget += cameraUpVector * distance;
		return;
	}
	void Camera::MoveCamAndTargetRight(float distance)
	{
		// Transform movement relative to local axes 
		cameraTarget += cameraRightVector * distance;
		return;
	}

	void Camera::MoveTargetForward(float distance)
	{
		// Transform movement relative to local axes 
		cameraTarget += cameraLookAt * distance;
		return;
	}
	void Camera::MoveTargetUpward(float distance)
	{
		// Transform movement relative to local axes 
		cameraTarget += cameraUpVector * distance;
		return;
	}
	void Camera::MoveTargetRight(float distance)
	{
		// Transform movement relative to local axes 
		cameraTarget += cameraRightVector * distance;
		return;
	}

	void Camera::AccelForward(float amount)
	{
		// Transform movement relative to local axes 
		acceleration.z += amount;
		if(acceleration.z > maxAcceleration.z)
			acceleration.z = maxAcceleration.z;
		return;
	}

	void Camera::AccelUpward(float amount)
	{
		// Transform movement relative to local axes 
		acceleration.y += amount;
		if(acceleration.y > maxAcceleration.y)
			acceleration.y = maxAcceleration.y;
		return;
	}

	void Camera::DecelForward(float amount)
	{
		// z *= 0.x
	}

	void Camera::DecelUpward(float amount)
	{

	}

	void Camera::DecelRight(float amount)
	{

	}

	void Camera::AccelRight(float amount)
	{
		// Transform movement relative to local axes 
		acceleration.x += amount;
		if(acceleration.x > maxAcceleration.x)
			acceleration.x = maxAcceleration.x;
		return;
	}

	void Camera::UpdateSpeed()
	{
		speed += acceleration;
		if(speed.x > maxSpeed.x) speed.x = maxSpeed.x;
		if(speed.y > maxSpeed.y) speed.y = maxSpeed.y;
		if(speed.z > maxSpeed.z) speed.z = maxSpeed.z;
	}

	void Camera::UpdateVelocity()
	{
		velocity = (cameraLookAt * speed.z) + (cameraUpVector * speed.y) + (cameraRightVector * speed.x);
	}

	void Camera::UpdatePosition()
	{
		cameraPosition += velocity;
	}

	void Camera::SetFocusState(bool newFocus)
	{
		hasFocus = newFocus;
	}

	void Camera::SetPerspectiveProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		SetFieldOfView(fov);
		// Create and set camera's projection matrix (perspective) 
		cameraProjectionMatrix = perspective(fov, aspectRatio, nearClip, farClip);
	}

	void Camera::SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
	{
		// Create and set camera's projection matrix (orthgraphic) 
		cameraProjectionMatrix = orthographic(left, right, bottom, top, near, far);
	}

	Matrix4 Camera::GetMVPMatrix()
	{
		
		// Rebuild the view matrix
		ConstructViewMatrix(true);

		// Create and return ModelViewProjection Matrix
		return (cameraViewMatrix*cameraProjectionMatrix);
	}

	Matrix4 Camera::GetViewMatrix()
	{
		// Rebuild and return the view matrix
		ConstructViewMatrix(true);

		return cameraViewMatrix;
	}

	Matrix4 Camera::GetProjectionMatrix()
	{
		return cameraProjectionMatrix;
	}


	void Camera::ConstructViewMatrix(bool orthoganalizeAxes)
	{
		// Orthogonalize axes if necessary (ensure up and right vectors are perpendicular to lookat and each other)
		if(orthoganalizeAxes)
		{
			// Look at
			cameraLookAt.Normalize();

			// Right vector
			cameraRightVector = crossprod(cameraUpVector, cameraLookAt);
			cameraRightVector.Normalize();

			// Up vector
			cameraUpVector = crossprod(cameraLookAt, cameraRightVector);
			cameraUpVector.Normalize();
		}


		// std::cout << "right.x: " << cameraRightVector.x << std::endl;
		// std::cout << "right.y: " << cameraRightVector.y << std::endl;
		// std::cout << "right.z: " << cameraRightVector.z << std::endl;
		// std::cout << "up.x: " << cameraUpVector.x << std::endl;
		// std::cout << "up.y: " << cameraUpVector.y << std::endl;
		// std::cout << "up.z: " << cameraUpVector.z << std::endl;
		// std::cout << "at.x: " << cameraLookAt.x << std::endl;
		// std::cout << "at.y: " << cameraLookAt.y << std::endl;
		// std::cout << "at.z: " << cameraLookAt.z << std::endl;
		// std::cout << "position.x: " << cameraPosition.x << std::endl;
		// std::cout << "position.y: " << cameraPosition.y << std::endl;
		// std::cout << "position.z: " << cameraPosition.z << std::endl;

		// std::cout << "Dotprod right and pos: " << dotprod(cameraRightVector,cameraPosition) << std::endl;
		// std::cout << "Dotprod up and pos: " << dotprod(cameraUpVector,cameraPosition) << std::endl;
		// std::cout << "Dotprod at and pos: " << dotprod(cameraLookAt,cameraPosition) << std::endl;

		// Build the view matrix itself
		cameraViewMatrix[0][0] = cameraRightVector.x;
		cameraViewMatrix[1][0] = cameraRightVector.y;
		cameraViewMatrix[2][0] = cameraRightVector.z;
		cameraViewMatrix[3][0] = -dotprod(cameraRightVector, cameraPosition);

		cameraViewMatrix[0][1] =  cameraUpVector.x;
		cameraViewMatrix[1][1] =  cameraUpVector.y;
		cameraViewMatrix[2][1] =  cameraUpVector.z;
		cameraViewMatrix[3][1] = -dotprod(cameraUpVector, cameraPosition);

		cameraViewMatrix[0][2] = cameraLookAt.x;
		cameraViewMatrix[1][2] = cameraLookAt.y;
		cameraViewMatrix[2][2] = cameraLookAt.z;
		cameraViewMatrix[3][2] = -dotprod(cameraLookAt, cameraPosition);

		cameraViewMatrix[0][3] = 0;
		cameraViewMatrix[1][3] = 0;
		cameraViewMatrix[2][3] = 0;
		cameraViewMatrix[3][3] = 1;

		// std::cout << "trans[0][0]: " << cameraViewMatrix[0][0] << std::endl;
		// std::cout << "trans[0][1]: " << cameraViewMatrix[0][1] << std::endl;
		// std::cout << "trans[0][2]: " << cameraViewMatrix[0][2] << std::endl;
		// std::cout << "trans[0][3]: " << cameraViewMatrix[0][3] << std::endl;
		// std::cout << "trans[1][0]: " << cameraViewMatrix[1][0] << std::endl;
		// std::cout << "trans[1][1]: " << cameraViewMatrix[1][1] << std::endl;
		// std::cout << "trans[1][2]: " << cameraViewMatrix[1][2] << std::endl;
		// std::cout << "trans[1][3]: " << cameraViewMatrix[1][3] << std::endl;
		// std::cout << "trans[2][0]: " << cameraViewMatrix[2][0] << std::endl;
		// std::cout << "trans[2][1]: " << cameraViewMatrix[2][1] << std::endl;
		// std::cout << "trans[2][2]: " << cameraViewMatrix[2][2] << std::endl;
		// std::cout << "trans[2][3]: " << cameraViewMatrix[2][3] << std::endl;
		// std::cout << "trans[3][0]: " << cameraViewMatrix[3][0] << std::endl;
		// std::cout << "trans[3][1]: " << cameraViewMatrix[3][1] << std::endl;
		// std::cout << "trans[3][2]: " << cameraViewMatrix[3][2] << std::endl;
		// std::cout << "trans[3][3]: " << cameraViewMatrix[3][3] << std::endl;

		// exit(0);
		return;
	}
#ifndef CLIENT_SERVER
	void Camera::ActionMoveCameraPosition(vec3f newPosition, int type)
	{
		switch(type)
		{
			case XCOORD:
				cameraPosition.x = newPosition.x;
				break;

			case YCOORD:
				cameraPosition.y = newPosition.y;
				break;

			case ZCOORD:
				cameraPosition.z = newPosition.z;
				break;
		}
	}

	void Camera::ActionMoveCameraLookAt(vec3f newLookAt)
	{
		SetLookAt(cameraPosition - newLookAt);
	}

	void Camera::ActionGetCameraPos(vec3f& position)
	{
		position = cameraPosition;
	}

	void Camera::ActionGetCameraLookAt(vec3f& lookAt)
	{
		lookAt = cameraLookAt;
	}

	void Camera::ActionGetCameraUp(vec3f& upVec)
	{
		upVec = cameraUpVector;
	}


	void Camera::SetMainCameraStatus(bool newStatus)
	{
		if(newStatus)
		{
			CameraActionSubscribe();
		}
		else
		{
			CameraActionUnsubscribe();
		}
	}
#endif

}
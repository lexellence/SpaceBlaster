/**************************************************************************************\
** File: CameraSettings.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for camera settings
**
\**************************************************************************************/
#pragma once
namespace Space
{
	namespace CameraSettings
	{
		const d2d::Range<float> CAMERA_SIZE_RANGE{ 75.0f, 300.0f };
		const float ZOOM_SPEED = 1.2f;
		const float INITIAL_ZOOM_OUT_PERCENT = 0.5f;
	}
}

#pragma once

#include "Utilities.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "IScene.h"

/**
 * @struct	RayTracer
 * @brief	Encapsulates the functionality of a ray tracer.
 */

struct RayTracer {
	color defaultColor;
	int antiAliasing=1;
	RayTracer(const color &defaultColor);
	void raytraceScene(FrameBuffer &frameBuffer, int depth,
						const IScene &theScene) const;
	color getLightColor(const Ray & ray, const IScene & theScene, HitRecord & theHit, color  &result)const;
protected:
	color traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const;
	
};

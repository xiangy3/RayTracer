#include "RayTracer.h"
#include "IShape.h"

/**
 * @fn	RayTracer::RayTracer(const color &defa)
 * @brief	Constructs a raytracers.
 * @param	defa	The clear color.
 */

RayTracer::RayTracer(const color &defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth,
	const IScene &theScene) const {
	const RaytracingCamera &camera = *theScene.camera;
	const std::vector<VisibleIShapePtr> &objs = theScene.visibleObjects;
	const std::vector<PositionalLightPtr> &lights = theScene.lights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			color colorForPixel;
			if (antiAliasing == 1) {

				Ray ray = camera.getRay((float)x, (float)y);
				colorForPixel = traceIndividualRay(ray, theScene, depth);
			}
			else if (antiAliasing == 3) {
				float indexDSX = 1.0f / (float)antiAliasing;
				float indexDSY = 1.0f / (float)antiAliasing;
				
				for (int i = -1; i < 2; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						Ray ray = camera.getRay(x + indexDSX * i, y + indexDSY * j);
						colorForPixel += traceIndividualRay(ray, theScene, depth);
					}
				}
				
				colorForPixel =colorForPixel/ (float)(antiAliasing*antiAliasing);
			}
			frameBuffer.setColor(x, y, colorForPixel);
		}
	}

	frameBuffer.showColorBuffer();
}


/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray &ray, const IScene &theScene, int recursionLevel) const {
	HitRecord theHit = VisibleIShape::findIntersection(ray, theScene.visibleObjects);
	HitRecord transHit = VisibleIShape::findIntersection(ray, theScene.transparentObjects);
	color result = { 0,0,0 };
	
	if (theHit.t < FLT_MAX) {

		if (theHit.texture != nullptr) {  // if object has a texture, use it
			float u = glm::clamp(theHit.u, 0.0f, 1.0f);
			float v = glm::clamp(theHit.v, 0.0f, 1.0f);
			color textureColor = theHit.texture->getPixel(u, v);
			result+= getLightColor(ray,theScene,theHit,result)*0.5f + textureColor*0.5f;
		}
		else {			// otherwise, compute color normally
						// Here, compute color normally
			result += getLightColor(ray,theScene,theHit,result);
		}
		if (transHit.t < FLT_MAX) {
			if (theHit.t > transHit.t) {
				Material material = transHit.material;
				result += result * (1 - material.alpha) + material.alpha*(material.ambient);
			}
		}
		
	}
	else {
		return result;
	}

	


	if (recursionLevel == 0) {
		return result;
	}
	else {
		
		glm::vec3 reflectionDirection = (ray.direction - 2 * glm::dot(ray.direction,theHit.surfaceNormal)*theHit.surfaceNormal);
		Ray reflectionRay(theHit.interceptPoint+EPSILON*theHit.surfaceNormal, reflectionDirection);
		result = result+traceIndividualRay(reflectionRay, theScene, recursionLevel - 1)*0.5f;
		
	}
	return result;



}

color RayTracer::getLightColor(const Ray &ray, const IScene &theScene, HitRecord &theHit,color &result) const{
	color finalResult=result;
	for each (PositionalLightPtr light1 in theScene.lights)
	{
		bool isShadow = false;
		color resultColor;

		glm::vec3 shadowDirection1 = light1->lightPosition - theHit.interceptPoint;
		Ray shadowRay1(theHit.interceptPoint + EPSILON * theHit.surfaceNormal, shadowDirection1);
		HitRecord shadowHit1 = VisibleIShape::findIntersection(shadowRay1, theScene.visibleObjects);
		float disShadowToLight1 = glm::distance(theHit.interceptPoint, light1->lightPosition);
		if (shadowHit1.t < disShadowToLight1) {
			isShadow = true;
			resultColor = light1->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, theScene.camera->cameraFrame, isShadow);
		}
		else {
			isShadow = false;
			resultColor = light1->illuminate(theHit.interceptPoint, theHit.surfaceNormal, theHit.material, theScene.camera->cameraFrame, isShadow);
		}


		finalResult += resultColor;
	}

	return finalResult;
}


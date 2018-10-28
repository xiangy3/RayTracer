#include <ctime>
#include "Defs.h"
#include "IShape.h"
#include "FrameBuffer.h"
#include "Raytracer.h"
#include "IScene.h"
#include "Light.h"
#include "Image.h"
#include "Camera.h"
#include "Rasterization.h"

int currLight = 0;
float angle = 0.5f;
float z = 0.0f;
float inc = 0.2f;
float movTowardFront = true;
bool isAnimated = false;
int numReflections = 0;
int antiAliasing = 1;
bool twoViewOn = false; 

Image im("usflag.ppm");

std::vector<PositionalLightPtr> lights = {
						new PositionalLight(glm::vec3(3, 30, 10), pureWhiteLight),
						new SpotLight(glm::vec3(2, 30, 4), glm::vec3(0,-1,0), glm::radians(45.0f), pureWhiteLight)
};

PositionalLightPtr posLight = lights[0];
SpotLightPtr spotLight = (SpotLightPtr)lights[1];

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(lightGray);
PerspectiveCamera pCamera(glm::vec3(0, 10, 10), ORIGIN3D, Y_AXIS, M_PI_2);
OrthographicCamera oCamera(glm::vec3(0, 10, 10), ORIGIN3D, Y_AXIS, 25.0f);
RaytracingCamera *cameras[] = { &pCamera, &oCamera };
int currCamera = 0;
IScene scene(cameras[currCamera], false);

void render() {
	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	cameras[currCamera]->calculateViewingParameters(frameBuffer.getWindowWidth()/2, frameBuffer.getWindowHeight());
	cameras[currCamera]->changeConfiguration(glm::vec3(12, 20, 18), ORIGIN3D, Y_AXIS);
	//cameras[currCamera]->changeConfiguration(glm::vec3(10.0f, 5.0f, 5.0f), glm::vec3(10.0f, 20.0f, 0.0f), Y_AXIS);
	rayTrace.raytraceScene(frameBuffer, numReflections, scene);

	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	float totalTimeSec = (frameEndTime - frameStartTime) / 1000.0f;
	std::cout << "Render time: " << totalTimeSec << " sec." << std::endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	cameras[currCamera]->calculateViewingParameters(width, height);
	glutPostRedisplay();
} 


//ISphere *sphere = new ISphere(glm::vec3(-4.0f, 0.0f, 0.0f), 2.0f);
IShape *plane = new IPlane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
IPlane *plane2 = new IPlane(glm::vec3(0.0f, -2.0f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
ISphere *sphere = new ISphere(glm::vec3(-6.0f, 3.0f, 0.0f), 6.0f);
IEllipsoid *ellipsoid = new IEllipsoid(glm::vec3(-3.0f, 2.0f, 11.0f), glm::vec3(4.0f, 4.0f, 3.0f));
ICylinderX *cylinderX = new ICylinderX(glm::vec3(16.0f, 2.0f, 8.0f), 2.0f, 8.0f);
ICloseCylinderY *closedCylinderY = new ICloseCylinderY(glm::vec3(10.0f, 6.0f, 0.0f), 4.0f, 12.0f);
IConeY *coneY = new IConeY(glm::vec3(20.0f, 6.0f, 0.0f), 1.0f, 8.0f);

void buildScene() {

	
	scene.addObject(new VisibleIShape(plane, tin));
	//scene.addObject(new VisibleIShape(plane2, silver));

	scene.addObject(new VisibleIShape(sphere, polishedSilver));
	scene.addObject(new VisibleIShape(ellipsoid, redPlastic));

	scene.addObject(new VisibleIShape(cylinderX, cyanRubber));
	//scene.addObject(new VisibleIShape(closedCylinderY, gold));
	scene.addObject(new VisibleIShape(coneY, gold));

	scene.addTransparentObject(new VisibleIShape(plane2, red),0.4f);

	
	
	VisibleIShapePtr p;
	scene.addObject(p = new VisibleIShape(closedCylinderY, gold));
	p->setTexture(&im);

	scene.addObject(lights[0]);
	scene.addObject(lights[1]);
}

void incrementClamp(float &v, float delta, float lo, float hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void incrementClamp(int &v, int delta, int lo, int hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void timer(int id) {
	if (isAnimated) {
		// modify something in your scene
		//sphere->center.x++;	
		float cylinderYZAxis = closedCylinderY->center.z;
		if (plane2->a.z > cylinderYZAxis + closedCylinderY->radius + 8.0f) {
			movTowardFront = false;
		}
		if (plane2->a.z < cylinderYZAxis - closedCylinderY->radius - 2.0f) {
			movTowardFront = true;
		}

		if (movTowardFront) {
			plane2->a.z+=0.3f;
		}
		else {
			plane2->a.z-=0.3f;
		}
		
	}
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	const float INC = 0.5f;
	switch (key) {
	case 'A':
	case 'a':	currLight = 0;
				std::cout << *lights[0] << std::endl;
				break;
	case 'B':	
	case 'b':	currLight = 1;
				std::cout << *lights[1] << std::endl;
				break;
	case 'O':
	case 'o':	lights[currLight]->isOn = !lights[currLight]->isOn;
				std::cout << (lights[currLight]->isOn ? "ON" : "OFF") << std::endl;
				break;
	case 'V':
	case 'v':	lights[currLight]->isTiedToWorld = !lights[currLight]->isTiedToWorld;
				std::cout << (lights[currLight]->isTiedToWorld ? "World" : "Camera") << std::endl;
				break;
	case 'Q':
	case 'q':	lights[currLight]->attenuationIsTurnedOn = !lights[currLight]->attenuationIsTurnedOn;
				std::cout << (lights[currLight]->attenuationIsTurnedOn ? "Atten ON" : "Atten OFF") << std::endl;
				break;
	case 'W':
	case 'w':	incrementClamp(lights[currLight]->attenuationParams.constant, isupper(key) ? INC : -INC, 0.0f, 10.0f);
				std::cout << lights[currLight]->attenuationParams << std::endl;
				break;
	case 'E':
	case 'e':	incrementClamp(lights[currLight]->attenuationParams.linear, isupper(key) ? INC : -INC, 0.0f, 10.0f);
				std::cout << lights[currLight]->attenuationParams << std::endl;
				break;
	case 'R':
	case 'r':	incrementClamp(lights[currLight]->attenuationParams.quadratic, isupper(key) ? INC : -INC, 0.0f, 10.0f);
				std::cout << lights[currLight]->attenuationParams << std::endl;
				break;
	case 'X':
	case 'x':	lights[currLight]->lightPosition.x += (isupper(key) ? INC : -INC);
				std::cout << lights[currLight]->lightPosition << std::endl;
				break;
	case 'Y':
	case 'y':	lights[currLight]->lightPosition.y += (isupper(key) ? INC : -INC);
				std::cout << lights[currLight]->lightPosition << std::endl;
				break;
	case 'Z':
	case 'z':	lights[currLight]->lightPosition.z += (isupper(key) ? INC : -INC);
				std::cout << lights[currLight]->lightPosition << std::endl;
				break;
	case 'J':
	case 'j':	spotLight->spotDirection.x += (isupper(key) ? INC : -INC);
				std::cout << spotLight->spotDirection << std::endl;
				break;
	case 'K':
	case 'k':	spotLight->spotDirection.y += (isupper(key) ? INC : -INC);
				std::cout << spotLight->spotDirection << std::endl;
				break;
	case 'L':
	case 'l':	spotLight->spotDirection.z += (isupper(key) ? INC : -INC);
				std::cout << spotLight->spotDirection << std::endl;
				break;
	case 'F':	
	case 'f':	incrementClamp(spotLight->fov, isupper(key) ? 0.2f : -0.2f, 0.1f, M_PI); 
				std::cout << spotLight->fov << std::endl;
				break;
	case 'P':
	case 'p':	isAnimated = !isAnimated;
				break;
	case 'C':
	case 'c':	
				break;
	case 'U':
	case 'u':	incrementClamp(pCamera.fov, isupper(key) ? 0.2f : -0.2f, glm::radians(10.0f), glm::radians(160.0f)); 
				std::cout << pCamera.fov << std::endl;
				break;
	//case 'h':	pCamera = PerspectiveCamera(glm::vec3(0, -10, -10), ORIGIN3D, Y_AXIS, M_PI_2);
	case 'M':
	case 'm':	break;
	case '+':	antiAliasing = 3;
				rayTrace.antiAliasing = antiAliasing;
				std::cout << "Anti aliasing: " << antiAliasing << std::endl;
				break;
	case '-':	antiAliasing = 1;
				rayTrace.antiAliasing = antiAliasing;
				std::cout << "Anti aliasing: " << antiAliasing << std::endl;
				break;

	case '0':	
	case '1':	
	case '2':	numReflections = key - '0';
				std::cout << "Num reflections: " << numReflections << std::endl;
				break;
	case 'd':	isAnimated = !isAnimated;
				break;
	case '?':	twoViewOn = !twoViewOn;
				break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		std::cout << (int)key << "unmapped key pressed." << std::endl;
	}

	glutPostRedisplay();
}

void special(int key, int x, int y) {
	const float INC = 0.5f;
	switch (key) {
		case GLUT_KEY_PAGE_DOWN: break;
		case GLUT_KEY_PAGE_UP: break;
		case GLUT_KEY_RIGHT: break;
		case GLUT_KEY_LEFT: break;
		default:
			std::cout << key << " special key pressed." << std::endl;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		xDebug = x;
		yDebug = frameBuffer.getWindowHeight() - 1 - y;
		color C = frameBuffer.getColor(xDebug, yDebug);
		std::cout << "(" << xDebug << "," << yDebug << ") = " << C << std::endl;
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	GLuint world_Window = glutCreateWindow(__FILE__);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	buildScene();

	glutMainLoop();

	return 0;
}
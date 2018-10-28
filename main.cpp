#include <ctime>
#include <vector>
#include "defs.h"
#include "Utilities.h"
#include "FrameBuffer.h"
#include "ColorAndMaterials.h"

int main(int argc, char *argv[]) {
	glm::vec2 v1 = glm::vec2(1,2);
	
	std::cout << doubleIt(v1) << std::endl;
	return 0;
}
#include "Shark.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include "Program.h"
#include "Utilities.h"

SharkShapes::SharkShapes() {

}

// Initialize all of the shapes that sharks need to draw
SharkShapes::SharkShapes(string RESOURCE_DIR) {
	bodyBack = make_shared<Shape>();
	bodyBack->loadMesh(RESOURCE_DIR + "sharkobj2/" + "body_back.obj", false);
	bodyBack->resize();
	bodyBack->init();

	bodyFront = make_shared<Shape>();
	bodyFront->loadMesh(RESOURCE_DIR + "sharkobj2/" + "body_front.obj", false);
	bodyFront->resize();
	bodyFront->init();

	finLeft = make_shared<Shape>();
	finLeft->loadMesh(RESOURCE_DIR + "sharkobj2/" + "fin_left.obj", false);
	finLeft->resize();
	finLeft->init();

	finRight = make_shared<Shape>();
	finRight->loadMesh(RESOURCE_DIR + "sharkobj2/" + "fin_right.obj", false);
	finRight->resize();
	finRight->init();

	head = make_shared<Shape>();
	head->loadMesh(RESOURCE_DIR + "sharkobj2/" + "head.obj", false);
	head->resize();
	head->init();

	jawBottom = make_shared<Shape>();
	jawBottom->loadMesh(RESOURCE_DIR + "sharkobj2/" + "jaw_bottom.obj", false);
	jawBottom->resize();
	jawBottom->init();

	tailBack = make_shared<Shape>();
	tailBack->loadMesh(RESOURCE_DIR + "sharkobj2/" + "tail_back.obj", false);
	tailBack->resize();
	tailBack->init();

	tailFin = make_shared<Shape>();
	tailFin->loadMesh(RESOURCE_DIR + "sharkobj2/" + "tail_fin.obj", false);
	tailFin->resize();
	tailFin->init();

	tailFront = make_shared<Shape>();
	tailFront->loadMesh(RESOURCE_DIR + "sharkobj2/" + "tail_front.obj", false);
	tailFront->resize();
	tailFront->init();
}

SharkShapes::~SharkShapes() {

}

Shark::Shark() {
}

Shark::Shark(shared_ptr<SharkShapes> sharkShapes) {
	// Initialize working shark parameters
	height = 1.0f;
	speed = 1.0f;
	radius = 1.0f;
	center << 0.0f, 0.0f, 0.0f;
	angularSpeed = speed / radius;
	// These offsets prevent all sharks from starting the same
	thetaOffset = randRangef(0.0f, 2.0f * float(M_PI));  // Swim circle offset
	phiOffset = randRangef(0.0f, 2.0f * float(M_PI));  // Swim wiggle offset

	// Set shark shapes for drawing
	shapes = sharkShapes;

	// Build up the hierarchical model from shapes.
	// I configured this manually...
	// Yes, it was horrible.
	shared_ptr<Link> bodyFront = make_shared<Link>(shapes->bodyFront);
	bodyFront->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	bodyFront->scaleMesh(1.062f / 2);
	bodyFront->centerMeshGeometry();
	parentLink = bodyFront;

	shared_ptr<Link> finLeft = make_shared<Link>(shapes->finLeft);
	finLeft->translateParent(Vector3f(-0.04557f * 1.1396f, -0.612f, -0.90226f * 1.1396f));
	finLeft->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	finLeft->scaleParent(1.249f / 2);
	finLeft->centerMeshGeometry();
	bodyFront->addChild(finLeft);

	shared_ptr<Link> finRight = make_shared<Link>(shapes->finRight);
	finRight->translateParent(Vector3f(-0.04557f * 1.1396f, -0.612f, 0.90226f * 1.1396f));
	finRight->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	finRight->scaleParent(1.249f / 2);
	finRight->centerMeshGeometry();
	bodyFront->addChild(finRight);

	shared_ptr<Link> head = make_shared<Link>(shapes->head);
	head->translateParent(Vector3f(1.2f, -.07f, 0.0f));
	head->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	head->scaleMesh(1.467f / 2);
	frontLinks.push_back(head);
	bodyFront->addChild(head);

	shared_ptr<Link> jawBottom = make_shared<Link>(shapes->jawBottom);
	jawBottom->translate(Vector3f(0.06f, -0.265f, 0.0f));
	jawBottom->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	jawBottom->scale(0.744f / 2.0f);
	jawBottom->centerMeshGeometry();
	head->addChild(jawBottom);

	shared_ptr<Link> bodyBack = make_shared<Link>(shapes->bodyBack);
	bodyBack->translateParent(Vector3f(-0.55f, 0.34f, 0.0f));
	bodyBack->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	bodyBack->scaleMesh(1.66f / 2.0f);
	bodyBack->centerMeshGeometry();
	backLinks.push_back(bodyBack);
	bodyFront->addChild(bodyBack);

	shared_ptr<Link> tailFront = make_shared<Link>(shapes->tailFront);
	tailFront->translate(Vector3f(-.9f, -.47f, 0.0f));
	tailFront->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	tailFront->scaleMesh(1.244f/2);
	tailFront->centerMeshGeometry();
	backLinks.push_back(tailFront);
	bodyBack->addChild(tailFront);

	shared_ptr<Link> tailBack = make_shared<Link>(shapes->tailBack);
	tailBack->translate(Vector3f(-0.8f, 0.07f, 0.0f));
	tailBack->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	tailBack->scaleMesh(0.742f / 2.0f);
	tailBack->centerMeshGeometry();
	backLinks.push_back(tailBack);
	tailFront->addChild(tailBack);

	shared_ptr<Link> tailFin = make_shared<Link>(shapes->tailFin);
	tailFin->translate(Vector3f(-0.9f, -0.05f, 0.0f));
	tailFin->rotateMesh(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	tailFin->scaleMesh(1.596f / 2);
	tailFin->centerMeshGeometry();
	backLinks.push_back(tailFin);
	tailBack->addChild(tailFin);
}

Shark::~Shark() {
}

void Shark::setHeight(float h) {
	height = h;
}

void Shark::setRadius(float r) {
	radius = r;
	angularSpeed = speed / radius;
}

void Shark::setSpeed(float s) {
	speed = s;
	angularSpeed = speed / radius;
}

void Shark::setCenter(Vector3f cen) {
  center = cen;
}

void Shark::draw(const shared_ptr<Program> prog, double t){
	// Parameters to determine how much the shark 'wiggles' when it swims.
	float dPhi = 5.0f * (float)M_PI / 180;
	float phiExp = 1.1f;

	// Make the front links swim.
	for (unsigned int i = 0; i < frontLinks.size(); i++) {
		shared_ptr<Link> link = frontLinks[i];
		link->pushMatrix();
		link->rotate(dPhi * sin(speed * float(t) + phiOffset) * (float)pow(phiExp, i), Vector3f(0.0f, 1.0f, 0.0f));
	}

	// Make the back links swim.
	for (unsigned int i = 0; i < backLinks.size(); i++) {
		shared_ptr<Link> link = backLinks[i];
		link->pushMatrix();
		link->rotate(dPhi * -sin(speed * float(t) + phiOffset) * (float)pow(phiExp, i), Vector3f(0.0f, 1.0f, 0.0f));
	}

	// Place the Shark in the world.
	float theta = float(t) * angularSpeed + thetaOffset;
	parentLink->pushMatrix();

	// Give the shark a slight sway as it swims.
	 parentLink->translate(Vector3f(center(0), height, center(2)));
	parentLink->rotate(theta, Vector3f(0.0f, 1.0f, 0.0f));
	parentLink->translate(Vector3f(radius, 0.0f, 0.0f));
	parentLink->rotate(3.0f * (float)M_PI / 180 * (float)sin(speed * t + phiOffset), Vector3f(0.0f, 1.0f, 0.0f));
	if (speed >= 0.0f) {
		parentLink->rotate(0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	}
	else {
		parentLink->rotate(-0.5f * (float)M_PI, Vector3f(0.0f, 1.0f, 0.0f));
	}

	shared_ptr<MatrixStack> M = make_shared<MatrixStack>();
	parentLink->draw(prog, M);
	parentLink->popMatrix();

	// Reset the shark's front pose.
	for (unsigned int i = 0; i < frontLinks.size(); i++) {
		shared_ptr<Link> link = frontLinks[i];
		link->popMatrix();
	}

	// Reset the shark's back pose.
	for (unsigned int i = 0; i < backLinks.size(); i++) {
		shared_ptr<Link> link = backLinks[i];
		link->popMatrix();
	}
}

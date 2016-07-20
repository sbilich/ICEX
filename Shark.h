#pragma once
#ifndef _SHARK_H_
#define _SHARK_H_

#include "Link.h"

using namespace std;
using namespace Eigen;

class SharkShapes {
public:
	SharkShapes();
	SharkShapes(string RESOURCE_DIR);
	~SharkShapes();
	shared_ptr<Shape> bodyBack;
	shared_ptr<Shape> bodyFront;
	shared_ptr<Shape> finLeft;
	shared_ptr<Shape> finRight;
	shared_ptr<Shape> head;
	shared_ptr<Shape> jawBottom;
	shared_ptr<Shape> tailBack;
	shared_ptr<Shape> tailFin;
	shared_ptr<Shape> tailFront;
};

class Shark {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Shark();
	Shark(shared_ptr<SharkShapes> sharkShapes);
	~Shark();

	void setHeight(float height);
	void setRadius(float radius);
	void setSpeed(float speed);
	void setCenter(Vector3f center);

	void draw(const shared_ptr<Program> prog, double t);

private:
	Vector3f center;
	float radius;
	float angularSpeed;
	float height;
	float speed;
	float thetaOffset;
	float phiOffset;

	shared_ptr<SharkShapes> shapes;
	shared_ptr<Link> parentLink;
	vector<shared_ptr<Link>> frontLinks;
	vector<shared_ptr<Link>> backLinks;
};

#endif

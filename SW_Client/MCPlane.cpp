#include "MCPlane.h"
#include <glm/glm.hpp>
#include <Eigen\Dense>

using namespace Eigen;


namespace SW_Client
{
	MCPlane::MCPlane(glm::vec3 o, glm::vec3 n)
	{
		MCPlane::o = o;
		MCPlane::n = n;
	}
	MCPlane::~MCPlane()
	{
	}

	glm::vec3 MCPlane::Intersection(MCPlane & a, MCPlane & b, MCPlane & c)
	{
		Matrix3f A;
		Vector3f B;
		A << a.n.x, a.n.y, a.n.z, b.n.x, b.n.y, b.n.z, c.n.x, c.n.y, c.n.z;
		B << glm::dot(a.n, a.o), glm::dot(b.n, b.o), glm::dot(c.n, c.o);
		Vector3f x = A.colPivHouseholderQr().solve(B);
		return glm::vec3(x.x(), x.y(), x.z());
	}
}

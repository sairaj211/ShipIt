#pragma once
#include "Core.h"
class Collider;
class Contact;
enum ContactType;


class GJK
{
public:
	static GEAR_API Contact* CheckContact(Collider* c1, Collider* c2, ContactType a_type);
private:
	static GEAR_API Contact* EPA(Collider* c1, Collider* c2, std::vector<glm::vec3> &simplex, ContactType a_type);
	static GEAR_API bool GJKCheck(Collider* c1, Collider* c2, std::vector<glm::vec3>&simplex);
	static GEAR_API bool DoTetrahedronSimplex(std::vector<glm::vec3> &simplex, glm::vec3 & dir);
	static GEAR_API bool DoLineSimplex(std::vector<glm::vec3> &simplex, glm::vec3 & dir);
	static GEAR_API bool DoTriangleSimplex(std::vector<glm::vec3> &simplex, glm::vec3 & dir);
	static GEAR_API glm::vec3 BaryCentricCoordinates(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 p); //returning u,v,w
	GJK() {};
	~GJK() {};

};


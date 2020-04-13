#include "pch.h"
#include "GJK.h"
#include "CollisionManager.h"
#include "Managers/Log.h"
#define MAX_ITERATION 64//can increase if no convergence

//if there is a contact we will return one, if there is not then we will 
//return a nullptr the collision manager needs to do this extra check
Contact * GJK::CheckContact(Collider * c1, Collider * c2, ContactType a_type)
{
	std::vector<glm::vec3> simplex = std::vector<glm::vec3>();
	if (GJKCheck(c1, c2, simplex))
		return EPA(c1, c2, simplex, a_type);
	//else
		//std::cout << "no contact found between " << c1->GetOwnerID() << " and " << c2->GetOwnerID() << std::endl;
	return nullptr;
}
GEAR_API bool GJK::GJKCheck(Collider * c1, Collider * c2, std::vector<glm::vec3>& simplex)
{

	Shape* s1 = c1->mpShape;
	Shape* s2 = c2->mpShape;
	//	std::vector<glm::vec3> simplex;
	std::vector<glm::vec3> search_history;
	std::vector<std::string> action_history;
	glm::vec3 search_dir = s2->mCenter - s1->mCenter; //initial search direction between colliders
	search_history.push_back(search_dir);
	//This GJK is based on the Casey Muratori implmentation from 2006 

	glm::vec3 B = s2->Support(search_dir) - s1->Support(-1.f*search_dir);
	search_dir = -B; //towards the origin hopefully
	search_history.push_back(search_dir);
	simplex.push_back(B);
	glm::vec3 A;
	//A is always the last point added to the simplex
	//B has always been examined
	//the size of the simplex is max 4 and order of the simplex is 
	//index 0->3, vertices  DCBA for a full simplex
	for (int iteration = 0; iteration < MAX_ITERATION; iteration++)//for loop to ensure exit
	{
		if (search_dir == glm::vec3(0)) {
			std::cout << "zero search dir" << std::endl;
			search_dir = -simplex.back();
		}

		A = s2->Support(search_dir) - s1->Support(-1.f*search_dir);
		
		simplex.push_back(A);
		
		int dim = (int)simplex.size();
		double Adot0 = glm::dot(A, search_dir);
		if (Adot0 < 0)//we didn't reach the origin, won't enclose it
		{
			//std::cout << "didn't reach origin id1: " <<c1->GetOwnerID() <<" id2: " << c2->GetOwnerID()<< std::endl;
			return false;
		}
		switch (dim)
		{
		case 1:
			break;
			LOG_INFO("SIMPLEX 1 called");
		case 2:
			DoLineSimplex(simplex, search_dir);//always returns false
			break;
		case 3:
			DoTriangleSimplex(simplex, search_dir);//always returns false
			break;
		case 4:
			if (DoTetrahedronSimplex(simplex, search_dir))
				return true;
			break;
		default://We have a problem if we reach this...
			LOG_INFO("Dimension is not between 1 or 4");
			std::cout << "Dim " << dim << " simplex size" << simplex.size()<< std::endl;
			break;
		}
		search_history.push_back(search_dir);
		//;

	}
	LOG_WARN("No Convergence after 64 iterations");
	return false;//there is not a collision after 64 iterations
}


struct Triangle {
	Triangle() {
		std::cout << "Empty Triangle() called:" << std::endl;
	}
	Triangle(const glm::vec3 &a, const  glm::vec3 &b, const  glm::vec3 &c) {

		verts[0] = a;
		verts[1] = b;
		verts[2] = c;
		norm = glm::normalize(glm::cross(b - a, c - a));

	}
	glm::vec3 verts[3];
	glm::vec3 norm;
	void Print() {
		std::cout << "point 1: (" << verts[0].x << "," << verts[0].y << "," << verts[0].z << ")" <<std::endl;
		std::cout << "point 2: (" << verts[1].x << "," << verts[1].y << "," << verts[1].z << ")" << std::endl;
		std::cout << "point 3: (" << verts[2].x << "," << verts[2].y << "," << verts[2].z << ")" << std::endl;
		std::cout << "normal: (" << norm.x << "," << norm.y << "," << norm.z << ")" << std::endl;
	}
};
struct Edge {
	Edge(const glm::vec3 &a, const glm::vec3 & b) {
		points[0] = a;
		points[1] = b;
	}
	glm::vec3 points[2];
};
Contact * GJK::EPA(Collider * c1, Collider * c2, std::vector<glm::vec3> & simplex, ContactType a_type)
{
	
	Shape* s1 = c1->mpShape;
	Shape* s2 = c2->mpShape;


	std::vector<Triangle> poly; //Array of faces, each with 3 verts and a normal the polytope
	std::vector<Edge> edges;
	
	//helpful function for tracking the edges in the EPA based  on Jacob Tyndall's Blog http://hacktank.net/blog/?p=119
	auto addEdge = [&](const glm::vec3 &a, const glm::vec3 &b)->void {
		for (auto it = edges.begin(); it != edges.end(); it++) {
			if (it->points[0] == b && it->points[1] == a) {
				//opposite edge found, remove it and do not add new one
				edges.erase(it);
				return;
			}
		}
		edges.emplace_back(Edge(a, b));
	};

	//Initization with the simplex from GJK
	//Face ABC
	poly.push_back(Triangle(simplex[3], simplex[2], simplex[1]));
	//Face ACD
	poly.push_back(Triangle(simplex[3], simplex[1], simplex[0]));
	//Face ADB
	poly.push_back(Triangle(simplex[3], simplex[0], simplex[2]));
	//Face BDC
	poly.push_back(Triangle(simplex[2], simplex[0], simplex[1]));
	float distance;
	float min_dist = FLT_MAX;
	for (int iterations = 0; iterations < MAX_ITERATION; iterations++) {
		
		//Find face that's closest to origin
		min_dist = glm::dot(poly[0].verts[0], poly[0].norm);
		Triangle closest_face = poly[0];
		//find the closest face first
		for (auto it = poly.begin(); it != poly.end(); ++it) {
			float dist = FLT_MAX;
			for (int i = 0; i < 3; ++i) {
				float d = glm::dot(it->verts[i], it->norm);
				if ( d <= dist) 
					dist = d;
			}
			if (dist <= min_dist) {
				min_dist = dist;
				closest_face = *it;
			}
		}

		//search normal to face that's closest to origin
		glm::vec3 search_dir = closest_face.norm;
		glm::vec3 _s2 = s2->Support(search_dir);
		glm::vec3 _s1 = s1->Support(-1.f*search_dir);
		glm::vec3 p = _s2- _s1;
		distance = dot(p, search_dir);
		if (std::fabsf(distance - min_dist) < 0.001f) {
			//dot vertex with normal to resolve collision along normal!
			Contact* contact = new Contact(c1, c2);
			float dist = std::abs(distance);
			glm::vec3 vwu = BaryCentricCoordinates(closest_face.verts[0], closest_face.verts[1], closest_face.verts[2], dist*closest_face.norm);// closest_face.norm*distance);
			float v = vwu.x; float w = vwu.y; float u = vwu.z;
			contact->colPoint = u*(closest_face.verts[0]+s1->mCenter) + 
								v * (closest_face.verts[1] + s1->mCenter) + 
								w * (closest_face.verts[2]+s1->mCenter);

		

			contact->penetrationDepth = dist;
			contact->mNormal = -1.f*closest_face.norm;//check this TODO
			contact->mContactType = a_type;
			//*
			if ((isnan(closest_face.norm.x) || isnan(closest_face.norm.y) || isnan(closest_face.norm.z)) || 
				(isinf(closest_face.norm.x) || isinf(closest_face.norm.y) || isinf(closest_face.norm.z)))
			{
				LOG_ERROR("NAN or inf cuaght in collision normal");
				std::cout << "Colliders " << c1->GetOwnerID() << " - " << c2->GetOwnerID() << std::endl;
				LOG_INFO("Polytope:");
				for (auto t : poly) 
					t.Print();
				
				LOG_INFO("closest face");
				closest_face.Print();
				delete contact;
				return nullptr;
			}

			//*/
			return contact;
		}
		//update the polytope by iterating through the triangles and adding edges 
		for (auto it = poly.begin(); it != poly.end();) {
			// can this face be 'seen' by entry_cur_support?
			if (glm::dot(it->norm, (p - it->verts[0])) > 0) {
				for (int i = 0; i < 3; ++i)
					addEdge(it->verts[i], it->verts[(i + 1) % 3]);
				it = poly.erase(it);//resets iterator
				continue;
			}
			else
				++it;
		}

		// create new triangles for the polytope from the edges in the edge list
		for (auto it = edges.begin(); it != edges.end(); it++)
			poly.push_back(Triangle(p, it->points[0], it->points[1]));

		edges.clear();//clear the new edges to add

	}
	LOG_ERROR("EPA did not converge! possible contact lost after 64 iterations");
	std::cout << " contacts (" << c1->GetOwner() << " and " << c2->GetOwner() << std::endl;
	return nullptr;
}




GEAR_API bool GJK::DoTetrahedronSimplex(std::vector<glm::vec3>& simplex, glm::vec3 & dir)
{
	glm::vec3 origin = glm::vec3(0, 0, 0);
	//A is peak/tip of the D4, BCD is the base (counterclockwise winding order) 
	//and we can ignore it since the triangle test checked if BCD was closest to
	//the origin already

	//Get normals of three new faces
	//A = simplex[3] 
	//B = simplex[2] 
	//C = simplex[1] 
	//D = simplex[0]
	glm::vec3 ABC = glm::cross(simplex[2] - simplex[3], simplex[1] - simplex[3]);
	glm::vec3 ACD = glm::cross(simplex[1] - simplex[3], simplex[0] - simplex[3]);
	glm::vec3 ADB = glm::cross(simplex[0] - simplex[3], simplex[2] - simplex[3]);

	//ABC = glm::normalize(ABC);
	//ADB = glm::normalize(ADB);
	//ACD = glm::normalize(ACD);

	glm::vec3 A0 = - simplex[3]; //dir to origin

	//Plane-test origin with 3 faces
	if (glm::dot(ABC, A0) > 0) {
		//Delete D if origin is in fron of ABC;
		simplex.erase(simplex.begin());
		dir = ABC;
		
		simplex.resize(3);
		//LOG_TRACE("ABC Face check");
		return GJK::DoTriangleSimplex(simplex, dir);
	}
	if (glm::dot(ACD, A0) > 0) { 
		//Delete B if origin is in front of ACD
		simplex.erase(simplex.begin() + 2);
		dir = ACD;
		
		//LOG_TRACE("ACD Face check");
		simplex.resize(3);
		return GJK::DoTriangleSimplex(simplex, dir);
	}
	if (dot(ADB, A0) > 0) { 
		//LOG_TRACE("ADB Face check");
		//Delete C if origin is in front of ADB
		
		simplex.erase(simplex.begin() + 1);
		

		simplex.resize(3);
		return GJK::DoTriangleSimplex(simplex,dir);
	}	
	//if the origin in not in front of any of the faces then it is in the tetrahedron
	//*
	//LOG_INFO("FINAL SIMPLEX: ");
	//for (auto s : simplex)
		//std::cout << "point (" << s.x << "," << s.y << "," << s.z << ")" << std::endl;
	//LOG_INFO("================");
	//*/
	return true;
	
}

GEAR_API bool GJK::DoLineSimplex(std::vector<glm::vec3>& simplex, glm::vec3 & dir)
{
	glm::vec3 AB =  simplex[0] - simplex[1];//check
	glm::vec3 A0 =  -1.f*simplex[1];
	if (glm::dot(AB, A0) > 0) {
		glm::vec3 AB0 = glm::cross(AB, A0);
		dir = glm::cross(AB0,AB);//check
		
	}//simplex is BA
	else {
		simplex.erase(simplex.begin());//check
		dir = A0;
		simplex.resize(1);
		
	}
	if (dir == glm::vec3(0, 0, 0)) {  //origin is on this line segment
		dir = glm::cross(AB, glm::vec3(1, 0, 0)); //normal with x-axis
		
		if (dir == glm::vec3(0, 0, 0)) {
			dir = glm::cross(AB, glm::vec3(0, 0, -1)); //normal with z-axis
			
		}
	}
	return false;
}

GEAR_API bool GJK::DoTriangleSimplex(std::vector<glm::vec3>& simplex, glm::vec3 & dir)
{
	glm::vec3 A0 = -1.f*simplex[2];
	glm::vec3 AB = simplex[1] - simplex[2];//check
	glm::vec3 AC = simplex[0] - simplex[2];
	glm::vec3 ABC = glm::cross(AB, AC);//Triangle face direction

	if (glm::dot(glm::cross(AB, ABC), A0) > 0) {
		//LOG_INFO("AB cross ABC");
		if (glm::dot(AB, A0) > 0) {
			//LOG_INFO("remove C");
			simplex.erase(simplex.begin());//Delete C
			dir = glm::cross(glm::cross(AB, A0),AB);//check
			
			simplex.resize(2);
		}
		else {
			//LOG_INFO("only A ");
			simplex.erase(simplex.begin());//only A
			simplex.erase(simplex.begin());
			dir = A0;//error
			
			simplex.resize(1);
		}
		return false;
	}
	if (glm::dot(glm::cross(ABC, AC), A0) > 0) {
		//LOG_INFO("ABC cross AC");
		if (glm::dot(AC, A0) > 0)
		{
			//LOG_INFO("remove B");
			simplex.erase(simplex.begin() + 1);//Delete B
			dir = glm::cross(glm::cross(AC, A0), AC);
			
			return false;
		}
		else
		{
			if (glm::dot(AB, A0) > 0) {
			//	LOG_INFO("remove C");
				simplex.erase(simplex.begin());//Delete C
				dir = glm::cross(glm::cross(AB, A0),AB);//check
				
			}
			else {
				//LOG_INFO("only A");
				simplex.erase(simplex.begin());//Only A
				simplex.erase(simplex.begin());
				dir = A0;
			}
			return false;
		}
	}

	if (glm::dot(ABC, A0) > 0) {// Face Case
		dir = ABC;//correct directing
		//LOG_INFO("ABC Correct FACING");
	}
	else {

		glm::vec3 tmp = simplex[1];
		simplex[1] = simplex[0];
		simplex[0] = tmp;//change winding direction
		dir = -1.f*ABC;
		//LOG_INFO("ABC REVERSE FACING");
	}
	//std::cout << "Triangle Case (" << dir.x << "," << dir.y << "," << dir.z << ")" << std::endl;
	return false;
}

GEAR_API glm::vec3 GJK::BaryCentricCoordinates(glm::vec3 a, glm::vec3 b, glm::vec3 c,glm::vec3 p)
{
	//based on approach in Christer Ericson
	glm::vec3 v0, v1, v2;
	/*
	std::cout << "a:  " << a.x << "," << a.y << "," << a.z << std::endl;
	std::cout << "b:  " << b.x << "," << b.y << "," << b.z << std::endl;
	std::cout << "c:  " << c.x << "," << c.y << "," << c.z << std::endl;
	std::cout << "p:  " << p.x << "," << p.y << "," << p.z << std::endl;
	*/
	v0 =  a-b; v1 =  a-c; v2 = a-p;
	double d00 = glm::dot(v0, v0);
	double d01 = glm::dot(v0, v1);
	double d11 = glm::dot(v1, v1);
	double d20 = glm::dot(v2, v0);
	double d21 = glm::dot(v2, v1);
	double dem = d00*d11-d01*d01;
	//std::cout << "d00 " << d00 << "d01 " << d01 << "d11 " << d11 << "d20 " << d20 << "d21 " << d21<< "dem " << dem << std::endl;
	double v = (d11*d20 - d01 * d21) / dem;
	double w = (d00*d21 - d01 * d20) / dem;
	glm::vec3 result = glm::vec3(v, w, 1.0 - v - w);

	return result;
}


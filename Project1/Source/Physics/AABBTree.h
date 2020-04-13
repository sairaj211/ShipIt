#pragma once
#include "Shape.h"
#include "Editor/RayCast.h"
#include "Managers/Log.h"
class CollisionManager;
class Node {
public:
	AABB* box;
	Collider* object;
	int objectId;
	int selfIndex;
	int nextIndex;
	int parentIndex;
	int left;
	int right;
	bool isLeaf() { 
		return height == 0 && left == -1 && right == -1; 
	};
	int height;
public:
	void SetParent(int p) {
		parentIndex = p;
		if (p!= -1 &&(p == left ||p ==right)) {
			//LOG_WARN("Setting parent to child");
		}
	}
	void SetHeight(int h) {
		height = h;
	}
	void SetLeft(int _left) {
		if (_left == -1) {
			LOG_WARN("Setting left child to -1");
		}
		if (_left == parentIndex) {
			//LOG_WARN("Setting left child to parent index");
		}
		left = _left;
	}
	void SetRight(int _right) {
		if (_right == -1) {
			LOG_WARN("Setting right child to -1");
		}
		if (_right == parentIndex) {
			//LOG_WARN("Setting right child to parent index");
		}
		right = _right;
	}
	void SetObj(Collider* _objId) {
		if (_objId == nullptr) {
			LOG_WARN("Setting object child to nullptr");
		}
		object = _objId;
	}
	Node();
	~Node();
};

class AABBTree
{
private:
	int PickBestSibling(int leafIndex);
	int AllocateNode();
	void DeallocateNode(int nodeIndex);
	int ReBalance(int index);
	float ComputeCost();
	AABB* GetAABB(Collider* col);
	Node* GetNode(int objectId);
	void QueryTree(CollisionManager* cm, int queryid,AABB* testAABB, std::list < std::pair<Collider*, Collider*>> & contacts);
	bool ValidateTree(int index);
public:
	GEAR_API void Update(std::vector<Component*>objects);
	GEAR_API bool RaycastCheck(Ray* ray);
	GEAR_API std::list<std::pair<Collider*,Collider*>> BroadPhase(CollisionManager* cm);
	GEAR_API void InsertLeaf(Collider* col);
	GEAR_API void DeleteLeaf(int leaf);
	GEAR_API void DeleteNode(int objId);
	GEAR_API void BuildTree(std::vector<Component*> objects);

	GEAR_API const std::vector<Node*> GetNodes();
	GEAR_API const int GetRoot();
	AABBTree(std::vector<Component*> objects );
	AABBTree();
	~AABBTree();
private:
	int mFreeNode;
	std::vector<Node*> nodes;
	std::vector<int> objIds;
	int nodeCount;
	int rootIndex;
};


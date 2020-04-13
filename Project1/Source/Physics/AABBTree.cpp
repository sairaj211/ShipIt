#include "pch.h"
#include "AABBTree.h"
#include "Managers/ComponentManager.h"
#include "CollisionManager.h"
#include "Managers/Log.h"


int AABBTree::AllocateNode()
{
	if (mFreeNode == -1) {
		mFreeNode = nodeCount;
		Node* node = new Node();
		nodes.push_back(node);
	}
	else {
		//LOG_INFO("Reusing node");
	}
	int nodeIndex = mFreeNode;
	mFreeNode = nodes[nodeIndex]->nextIndex;
	nodes[nodeIndex]->objectId = -1;
	nodes[nodeIndex]->selfIndex = nodeIndex;
	nodes[nodeIndex]->parentIndex = -1;
	nodes[nodeIndex]->left = -1;
	nodes[nodeIndex]->right = -1;
	nodes[nodeIndex]->height = 0;
	nodes[nodeIndex]->nextIndex = -1;

	++nodeCount;

	return nodeIndex;
}

void AABBTree::DeallocateNode(int nodeIndex)
{
	
	nodes[nodeIndex]->height = -1;
	nodes[nodeIndex]->objectId = -1;
	if(nullptr != nodes[nodeIndex]->object)
		nodes[nodeIndex]->object->mNodeIndex = -1;
	nodes[nodeIndex]->object = nullptr;
	nodes[nodeIndex]->parentIndex = -1;

	nodes[nodeIndex]->left = -1;
	nodes[nodeIndex]->right = -1;

	nodes[nodeIndex]->nextIndex = mFreeNode;
	mFreeNode = nodeIndex;

	--nodeCount;

}

int AABBTree::ReBalance(int index)
{
	if (index == -1) return index;
	Node* A = nodes[index];
	if (nodes[index]->isLeaf() || nodes[index]->height < 2) 
		return index;
	/*
	*    A
	*   / \
	*  B   C
	*/
	Node* B = nodes[A->left];
	Node* C = nodes[A->right];
	int balance = C->height - B->height;

	// Rotate C up
	if (balance > 1)
	{
		/*
		*    A
		*   / \
		*  B   C
		*     / \
		*    F   G
		*/
		Node* F = nodes[C->left];
		Node* G = nodes[C->right];

		// Swap A and C
		C->SetLeft( index);
		C->SetParent(A->parentIndex);
		A->SetParent( C->selfIndex);

		// A's old parent should point to C
		if (C->parentIndex != -1)
		{
			if (nodes[C->parentIndex]->left == index)
				nodes[C->parentIndex]->SetLeft(C->selfIndex);
			else
				nodes[C->parentIndex]->SetRight(C->selfIndex);
		}
		else
			rootIndex = C->selfIndex;

		// Rotate
		if (F->height > G->height)
		{
			C->SetRight( F->selfIndex);
			A->SetRight( G->selfIndex);
			G->SetParent( index);
			A->box->Union(B->box, G->box);
			C->box->Union(A->box, F->box);

			
			A->SetHeight( 1 + std::max(B->height, G->height));
			C->SetHeight( 1 + std::max(A->height, F->height) );
		}
		else
		{
			C->SetRight(G->selfIndex);
			A->SetRight( F->selfIndex);
			F->SetParent(index);
			A->box->Union(B->box, F->box);
			C->box->Union(A->box, G->box);

			A->SetHeight( 1 + std::max(B->height, F->height));
			C->SetHeight(1 + std::max(A->height, G->height));

		}
		if (!ValidateTree(rootIndex)) {
			LOG_CRITICAL("Tree not valid after rebalance left");
		}
		return C->selfIndex;
	}

	// Rotate B up
	if (balance < -1)
	{
		/*
		*     A
		*    / \
		*   B   C
		*  / \  
		* D  E 
		* 
		*/
		Node* D = nodes[B->left];
		Node* E = nodes[B->right];

		// Swap A and B
		B->SetLeft( index);
		B->SetParent( A->parentIndex);
		A->SetParent( B->selfIndex);

		// A's old parent should point to B
		if (B->parentIndex != -1)
		{
			if (nodes[B->parentIndex]->left == index)
				nodes[B->parentIndex]->SetLeft(B->selfIndex);
			else
				nodes[B->parentIndex]->SetRight( B->selfIndex);
		}
		else
			rootIndex = B->selfIndex;

		// Rotate
		if (D->height > E->height)
		{
			B->SetRight( D->selfIndex);
			A->SetLeft( E->selfIndex);
			E->SetParent( index);
			A->box->Union(C->box, E->box);
			B->box->Union(A->box, D->box);

			A->SetHeight( 1 + std::max(C->height , E->height ));
			B->SetHeight( 1 + std::max(A->height , D->height));
		}
		else
		{
			B->SetRight( E->selfIndex);
			A->SetLeft( D->selfIndex);
			D->SetParent( index);
			A->box->Union(C->box, D->box);
			B->box->Union(A->box, E->box);

			A->SetHeight( 1 + std::max(C->height , D->height));
				
			B->SetHeight( 1 + std::max(A->height , E->height));
		}
		if (!ValidateTree(rootIndex)) {
			LOG_CRITICAL("Tree not valid after rebalance right");
		}
		return B->selfIndex;
	}
	if (!ValidateTree(rootIndex)) {
		LOG_CRITICAL("Tree not valid after rebalance ");
	}
	return index;
}


int AABBTree::PickBestSibling(int leafIndex)
{
	AABB* leafAABB = nodes[leafIndex]->box;
	int index = rootIndex;
	while (!nodes[index]->isLeaf())
	{
		int child1 = nodes[index]->left;
		int child2 = nodes[index]->right;

		float area = nodes[index]->box->GetSurfaceArea();

		AABB* combinedAABB = new AABB(glm::vec3(0));;
		combinedAABB->Union(nodes[index]->box, leafAABB);
		float combinedArea = combinedAABB->GetSurfaceArea();
		delete combinedAABB;
		// Cost of creating a new parent for this node and the new leaf
		float cost = 2.0f * combinedArea;

		// Minimum cost of pushing the leaf further down the tree
		float inheritanceCost = 2.0f * (combinedArea - area);

		// Cost of descending into child1
		float cost1;
		if (nodes[child1]->isLeaf())
		{
			AABB* aabb = new AABB(glm::vec3(0));
			aabb->Union(leafAABB, nodes[child1]->box);
			cost1 = aabb->GetSurfaceArea() + inheritanceCost;
			delete aabb;
		}
		else
		{
			AABB* aabb = new AABB(glm::vec3(0));
			aabb->Union(leafAABB, nodes[child1]->box);
			float oldArea = nodes[child1]->box->GetSurfaceArea();
			float newArea = aabb->GetSurfaceArea();
			cost1 = (newArea - oldArea) + inheritanceCost;
			delete aabb;
		}

		// Cost of descending into child2
		float cost2;
		if (nodes[child2]->isLeaf())
		{
			AABB* aabb = new AABB(glm::vec3(0));
			aabb->Union(leafAABB, nodes[child2]->box);
			cost2 = aabb->GetSurfaceArea() + inheritanceCost;
			delete aabb;
		}
		else
		{
			AABB* aabb = new AABB(glm::vec3(0));;
			aabb->Union(leafAABB, nodes[child2]->box);
			float oldArea = nodes[child2]->box->GetSurfaceArea();
			float newArea = aabb->GetSurfaceArea();
			cost2 = newArea - oldArea + inheritanceCost;
			delete aabb;
		}

		// Descend according to the minimum cost.
		if (cost < cost1 && cost < cost2)
			break;


		// Descend
		if (cost1 < cost2)
			index = child1;
		else
			index = child2;

	}
	if (index == -1) {
		LOG_CRITICAL("Error in the picking the bet sibling, -1");
	}
	if (index == nodes[leafIndex]->parentIndex) {
		LOG_CRITICAL("Circular dependency about to be formed");
	}
	return index;

}

float AABBTree::ComputeCost()
{
	float total = 0.0f; 
	for (Node* n : nodes) 
		total += n->box->GetSurfaceArea();
	return total;
}

AABB * AABBTree::GetAABB(Collider* col)
{
	if (col->mNodeIndex == -1) return nullptr;
	Node* n = nodes[col->mNodeIndex];
	if (n) return n->box;
	return nullptr;
}

Node * AABBTree::GetNode(int objectId)
{
	for (Node* n : nodes) {
		if (n->objectId == objectId)
			return n;
	}
	return nullptr;
}


void AABBTree::QueryTree(CollisionManager* cm, int queryid, AABB * testAABB, std::list<std::pair<Collider*, Collider*>>& contacts)
{
	std::list<int> stack;
	stack.push_front(rootIndex);
	while (!stack.empty()) {
		int index = stack.front();
		stack.pop_front();
		Node* n = nodes[index];
		if (cm->CheckCollision(n->box, testAABB, ContactType::Other)) {
			if (nodes[index]->isLeaf()) {
				if (nodes[index]->object->GetOwner() == queryid)
					continue;
				int id1 = std::min( nodes[index]->object->GetOwner() , queryid);
				int id2 = std::max(nodes[index]->object->GetOwner(), queryid);
				Collider* col1 = COMPONENT_MANAGER->GetComponent<Collider>(id1, "Collider");
				Collider* col2 = COMPONENT_MANAGER->GetComponent<Collider>(id2, "Collider");
				bool insert = true;
				for (auto pair : contacts) {
					if (pair.first == col1 && pair.second == col2) {
						insert = false;
						break;
					}

				}
				if(insert)
					contacts.push_back(std::pair<Collider*, Collider*>(col1, col2));
			}
			else
			{
				stack.push_front(nodes[index]->left);
				stack.push_front(nodes[index]->right);
			}
		}
	}
	return;
}

bool AABBTree::ValidateTree(int index)
{
	if (index == -1) {
		LOG_CRITICAL("Empty Tree");
		return true;
	}

	if (index == rootIndex) {
		if (nodes[index]->parentIndex != -1) {
			LOG_CRITICAL("Bad Root");
			return false;//invalid root
		}
	}
	Node* node = nodes[index];

	int child1 = node->left;
	int child2 = node->right;

	if (node->isLeaf())
	{
		if (child1 != -1 || child2 != -1 || node->height != 0) {
			LOG_CRITICAL("Child is bad",index);
			return false; // child issues
		}
		else {
			return true;//all is good
		}
	}
	else {
		if (node->left == -1 || node->right == -1) {
			LOG_CRITICAL("parent is bad", index);
			return false;//parent issus
		}
	}
	if (nodes[index]->parentIndex == nodes[index]->selfIndex)
		return false;
	if (nodes[child1]->parentIndex != index && nodes[child2]->parentIndex != index)
		return false;//parent issus
	
	bool children_ok = ValidateTree(child1) && ValidateTree(child2);
	return children_ok;
}

GEAR_API void AABBTree::Update(std::vector<Component*>objects)
{
	if (objects.empty()) return;
	Collider* col_test = static_cast<Collider*>(objects[0]);
	if (!col_test) return;
	std::vector<int> objs;
	for (auto it = objects.begin(); it != objects.end(); ++it) {

		objs.push_back((*it)->GetOwnerID());
	}
	std::sort(objs.begin(), objs.end());
	std::sort(objIds.begin(),objIds.end());
	std::vector<int> new_objs;
	std::vector<int> old_objs;
	std::set_difference(objs.begin(), objs.end(), objIds.begin(), objIds.end(), std::inserter(new_objs, new_objs.begin()));
	std::set_difference(objIds.begin(), objIds.end(), objs.begin(), objs.end(), std::inserter(old_objs, old_objs.begin()));
	for (int i : old_objs) //delete old objects
		DeleteLeaf(GetNode(i)->selfIndex);
	for (int i : new_objs)//find new objects to add
		InsertLeaf(COMPONENT_MANAGER->GetComponent<Collider>(i,"Collider"));

	//update the objids list with stl
	for (int i = 0; i < objects.size(); ++i) {
		int id =objects[i]->GetOwnerID();
		//check for resizing and update the tree
		Collider* col = static_cast<Collider*>(objects[i]);
		if (GetAABB(col)->Contains(col->mpShape)) 
			continue;//everthing is alright
		int nodeId = col->mNodeIndex;
		//Delete and readd leaf if it has moved out
		DeleteLeaf(nodeId);
		InsertLeaf(col);
	}	

	if (!ValidateTree(rootIndex)) {
		LOG_CRITICAL("Tree not valid after update");
	}
	objIds = objs;
	//figure out how to ensure this is as few operations as possible TODO
}

GEAR_API bool AABBTree::RaycastCheck(Ray* ray)
{
	std::list<int> stack; 
	stack.push_front(rootIndex); 
	while (!stack.empty()) {
		int index = stack.front(); 
		stack.pop_front();
		Node* n = nodes[index];
		if (!n->box->Intersected(ray).first)
			continue; 
		
		if (nodes[index]->isLeaf()) 
			if (nodes[index]->object->mpShape->Intersected(ray).first)
				 return true; 
		else 
		{ 
			stack.push_front(nodes[index]->left); 
			stack.push_front(nodes[index]->right);
		}
	} 
	return false;

}

GEAR_API std::list<std::pair<Collider*, Collider*>> AABBTree::BroadPhase(CollisionManager* cm)
{
	std::list<std::pair<Collider*, Collider*>> colliders;
	for (int id : objIds) {
		Collider* col = COMPONENT_MANAGER->GetComponent<Collider>(id, "Collider");
		if (nullptr == col) continue;
		AABB* aabb = GetAABB(col);
		QueryTree(cm,id,aabb, colliders);//order colliders so that the min id is first
	}
	for (auto c : colliders) {
		
	}
	return colliders;
}

GEAR_API void AABBTree::InsertLeaf(Collider * col)
{
	if (-1 != col->mNodeIndex) 
		return;
	int id = col->GetOwnerID();
	int index = AllocateNode();
	AABB::MakeBoundingAABB(col->mpShape, nodes[index]->box,glm::vec3(.5f));
	nodes[index]->object = col;
	nodes[index]->objectId = col->GetOwnerID();
	col->mNodeIndex = index;
	int leafIndex = nodes[index]->selfIndex;

	if (nodeCount == 1) { 
		rootIndex = leafIndex; 
		return; 
	}
	// Stage 1: find the best sibling for the new leaf
	int sibling = -1;
	sibling = PickBestSibling(leafIndex);

	// Stage 2: create a new parent
	int oldParent = nodes[sibling]->parentIndex; 
	int newParent;

	newParent = AllocateNode();
	nodes[newParent]->SetParent(oldParent);
	nodes[newParent]->box->Union(nodes[index]->box, nodes[sibling]->box);
	if (oldParent != -1) { // The sibling was not the root 
		if (nodes[oldParent]->left == sibling) 
			nodes[oldParent]->SetLeft( newParent); 
		else 
			nodes[oldParent]->SetRight( newParent); 
		
		nodes[newParent]->SetLeft( sibling);
		nodes[newParent]->SetRight( leafIndex);
		nodes[sibling]->SetParent( newParent); 
		nodes[leafIndex]->SetParent( newParent);
	}
	else 
	{
		// The sibling was the root 
		nodes[newParent]->SetLeft(sibling);
		nodes[newParent]->SetRight(leafIndex);
		nodes[sibling]->SetParent(newParent);
		nodes[leafIndex]->SetParent(newParent);
		rootIndex = newParent;
	}

	// Stage 3: walk back up the tree refitting AABBs
	index = nodes[leafIndex]->parentIndex; 
	while (index != -1) {
		index = ReBalance(index);
		int left = nodes[index]->left; 
		int right = nodes[index]->right;
		//validate that children are not -1
		nodes[index]->SetHeight( 1 + std::max(nodes[left]->height, nodes[right]->height));
		nodes[index]->box->Union(nodes[left]->box, nodes[right]->box); 
		index = nodes[index]->parentIndex;
	}
	if (!ValidateTree(rootIndex))
		LOG_CRITICAL("Tree not valid after inserting leaf %d",leafIndex);
}

GEAR_API void AABBTree::DeleteLeaf(int leaf)
{
	if (leaf == -1) return;
	if (!nodes[leaf]->isLeaf()) 
		return;
	if (leaf == rootIndex)
	{
		DeallocateNode(leaf);
		rootIndex = -1;
		return;
	}

	int parent = nodes[leaf]->parentIndex;
	int grandParent = nodes[parent]->parentIndex;
	int sibling = nodes[parent]->left;
	if (sibling == leaf)
		sibling = nodes[parent]->right;
	else
		sibling = nodes[parent]->left;

	if (grandParent != -1)
	{
		// Destroy parent and connect sibling to grandParent.
		if (nodes[grandParent]->left == parent)
			nodes[grandParent]->SetLeft(sibling);
		else
			nodes[grandParent]->SetRight(sibling);
		nodes[sibling]->SetParent(grandParent);
		DeallocateNode(parent);

		int index = grandParent;
		while (index != -1)//Rebalance the tree
		{
			index = ReBalance(index);

			int left = nodes[index]->left;
			int right = nodes[index]->right;

			nodes[index]->box->Union(nodes[left]->box, nodes[right]->box);
			nodes[index]->SetHeight( 1 + std::max(nodes[left]->height , nodes[right]->height));

			index = nodes[index]->parentIndex;
		}
	}
	else
	{
		rootIndex = sibling;
		nodes[sibling]->parentIndex = -1;
		DeallocateNode(parent);
	}
	DeallocateNode(leaf);
	if(!ValidateTree(rootIndex)){
		LOG_CRITICAL("Tree not valid after deleting leaf %d", leaf);
	}
}

GEAR_API void AABBTree::DeleteNode(int objId)
{
	int index = -1;
	for (int i = 0; nodes.size(); ++i) {
		if (nodes[i]->object->GetOwnerID() == objId) {
			DeallocateNode(i);
			return;
		}
	}
}

GEAR_API void AABBTree::BuildTree(std::vector<Component*> objects)
{
	for (auto obj : objects) {
		Collider* col = static_cast<Collider*>(obj);
		if(col)
			InsertLeaf(col);
	}
}



GEAR_API const std::vector<Node*> AABBTree::GetNodes()
{
	return nodes;
}

GEAR_API const int AABBTree::GetRoot()
{
	return rootIndex;
}

AABBTree::AABBTree(std::vector<Component*>objects)
{
	this->mFreeNode = -1;
	this->nodeCount = 0;

	//build the AABB tree here
	this->BuildTree(objects);
	
	
}

AABBTree::AABBTree()
{
	mFreeNode = -1;
	nodeCount = 0;
}



AABBTree::~AABBTree()
{
	for (Node* node : nodes) 
		delete node;//none of this should be the actual AABB of an object
	nodes.clear();
}

Node::Node()
{
	box = new AABB(glm::vec3(0));
	object = nullptr;
	selfIndex = -1;
	nextIndex = -1;
	parentIndex = -1;
	left = -1;
	right = -1;
	height = -1;
}

Node::~Node()
{
	delete box;
	box = nullptr;
}

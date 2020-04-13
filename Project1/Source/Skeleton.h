#pragma once

/*!
 * A Struct defining the attributes of the
 * Bones that make up the Skeleton hierarchy
 *
 */
struct Bone 
{
	std::string name; /*!< Name of the Bone */
	glm::mat4 offsetMatrix; /*!< Transformation that brings this Bone to the space of its parent */
	glm::mat4 finalTransformation; /*!< Transformation used by the vertices associated with this Bone for the current frame */
	glm::mat4 bindTransformation; /*!< Transformation used by the vertices associated with this Bone in its bind position */
	glm::mat4 worldTransformation; /*!< Transformation used by the vertices associated with this Bone in coordinates relative to its model */

	Bone* pParent; /*!< Pointer to this Bone's parent Bone */
	std::vector<Bone*> children; /*!< Vector of this Bone's child Bones */
};

/*!
 * The Skeleton that maintains a Bone hierarchy
 * so they may be used by Skeletal Animations
 *
 */
class Skeleton 
{
public:
	/*!
	 * Skeleton Default Constructor
	 *
	 */
	GEAR_API Skeleton();
	/*!
	 * Skeleton Default Destructor
	 *
	 */
	GEAR_API ~Skeleton();
	/*!
	 * Builds the Skeleton Bone hierarchy from the given
	 * ASSIMP node hierarchy
	 *
	 * \param a_pNode The current node in the ASSIMP node hierarchy
	 * \param a_pParent A pointer to the parent Bone associated with the current node
	 */
	GEAR_API void BuildFromRoot(aiNode* a_pNode, Bone* a_pParent);
	/*!
	 * Normalizes the Skeleton Bone transformations according
	 * to the given max length
	 *
	 * \param a_maxLength The maximum difference in vertex position range in each of the x, y, and z directions
	 */
	GEAR_API void Normalize(float a_maxLength);
	/*!
	 * Traverses the Bone hierarchy and normalizes the current
	 * Bone transformations according to the given max length
	 *
	 * \param a_pBone The current Bone in the Skeleton hierarchy
	 * \param a_maxLength The maximum difference in vertex position range in each of the x, y, and z directions
	 */
	GEAR_API void NormalizeTraversal(Bone* a_pBone, float a_maxLength);

public:
	std::vector<Bone> mBones; /*!< Vector of Bones in the Skeleton */
	std::unordered_map<std::string, unsigned int> mBoneMap; /*!< Map that maps a Bone's name to its index in the mBones vector */
	Bone* mpSkeletonRoot; /*!< Pointer to the root Bone of the Skeleton hierarchy */
};
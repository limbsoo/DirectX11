#pragma once

#ifndef AV_SCENEANIMATOR_H_INCLUDED
#define AV_SCENEANIMATOR_H_INCLUDED

#include <assimp\scene.h>
#include <map>
#include <vector>
#include "d3d11.h"
#include "AnimEvaluator.h"

class SceneAnimator{
	public:

		SceneAnimator(): Skeleton(0), CurrentAnimIndex(-1) {}
		~SceneAnimator()
		{ 
			Release(); 
		}

		// this must be called to fill the SceneAnimator with valid data
		void Init(const aiScene* pScene);

		// frees all memory and initializes everything to a default state
		void Release();

		bool HasSkeleton() const
		{ 
			return !Bones.empty();
		}

		// the set animation returns whether the animation changed or is still the same. 
		bool SetAnimIndex( int pAnimIndex);// this takes an index to set the current animation to

		// get the transforms needed to pass to the vertex shader. This will wrap the dt value passed, so it is safe to pass 50000000 as a valid number
		std::vector<D3DXMATRIX>& GetTransforms(float dt)
		{ 
			return Animations[CurrentAnimIndex].GetTransforms(dt);
		}

		int GetAnimationIndex() const { 
			return CurrentAnimIndex; 
		}

		std::string GetAnimationName() const { return Animations[CurrentAnimIndex].Name;  }

		//GetBoneIndex will return the index of the bone given its name. The index can be used to index directly into the vector returned from GetTransform
		int GetBoneIndex(const std::string& bname){ std::map<std::string, unsigned int>::iterator found = BonesToIndex.find(bname); if(found!=BonesToIndex.end()) return found->second; else return -1;}

		//GetBoneTransform will return the matrix of the bone given its name and the time. be careful with this to make sure and send the correct dt. If the dt is different from what the model is currently at, the transform will be off
		D3DXMATRIX GetBoneTransform(float dt, const std::string& bname) 
		{ 
			int bindex=GetBoneIndex(bname); 
			if(bindex == -1) 
			{
				D3DXMATRIX mResult;
				D3DXMatrixIdentity(&mResult);
				return mResult;
			}
			
			return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; 
		}

		// same as above, except takes the index
		D3DXMATRIX GetBoneTransform(float dt, unsigned int bindex) 
		{  
			return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; 
		}

		std::vector<AnimEvaluator> Animations;// a std::vector that holds each animation 
		int CurrentAnimIndex;/** Current animation index */


	protected:		
		cBone* Skeleton;/** Root node of the internal scene structure */
		std::map<std::string, cBone*> BonesByName;/** Name to node map to quickly find nodes by their name */
		std::map<std::string, unsigned int> BonesToIndex;/** Name to node map to quickly find nodes by their name */
		std::map<std::string, UINT> AnimationNameToId;// find animations quickly
		std::vector<cBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
		std::vector<D3DXMATRIX> Transforms;// temp array of transfrorms

		void UpdateTransforms(cBone* pNode);
		void CalculateBoneToWorldTransform(cBone* pInternalNode);/** Calculates the global transformation matrix for the given internal node */

		void Calculate(float pTime);
		void CalcBoneMatrices();

		void ExtractAnimations(const aiScene* pScene);
		cBone* CreateBoneTree( aiNode* pNode, cBone* pParent);// Recursively creates an internal node structure matching the current scene and animation. 
};

#endif // AV_SCENEANIMATOR_H_INCLUDED
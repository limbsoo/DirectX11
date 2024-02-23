#pragma once

#ifndef AV_SCENEANIMATOR_H
#define AV_SCENEANIMATOR_H

#include <map>
#include <vector>

#include <fstream>
#include <tuple>
#include <string>
#include "d3d11.h"
#include <types.h>
#include <anim.h>
#include <scene.h>
#include "Types.h"

//class cBone {
//public:
//
//	std::string Name;
//	D3DXMATRIX Offset, LocalTransform, GlobalTransform, OriginalLocalTransform;
//	cBone* Parent;
//	std::vector<cBone*> Children;
//
//	cBone() :Parent(0){}
//	~cBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
//};
//class cAnimationChannel{
//public:
//	std::string Name;
//	std::vector<aiVectorKey> mPositionKeys;
//	std::vector<aiQuatKey> mRotationKeys;
//	std::vector<aiVectorKey> mScalingKeys;
//};

class cAnimEvaluator{
public:

	cAnimEvaluator(): mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f), PlayAnimationForward(true), Animation_Indexer(0) {}
	cAnimEvaluator( const aiAnimation* pAnim);
	void Evaluate( float pTime, std::map<std::string, cBone*>& bones);
	void Save(std::ofstream& file);
	void Load(std::ifstream& file);
	std::vector<XMMATRIX>& GetTransforms(float dt){ return Transforms[GetFrameIndexAt(dt)]; }
	unsigned int GetFrameIndexAt(float time);

	std::string Name;
	UINT Animation_Indexer;// this is only used if an animation has no name. I assigned it Animation + Animation_Indexer
	std::vector<cAnimationChannel> Channels;
	bool PlayAnimationForward;// play forward == true, play backward == false
	float mLastTime, TicksPerSecond, Duration;	
	std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;
	std::vector<std::vector<XMMATRIX>> Transforms;//, QuatTransforms;/** Array to return transformations results inside. */
};

 
class cSceneAnimator{
public:

	cSceneAnimator(): Skeleton(0), CurrentAnimIndex(-1) {}
	~cSceneAnimator(){ Release(); }

	void Init(const aiScene* pScene);// this must be called to fill the SceneAnimator with valid data
	void Release();// frees all memory and initializes everything to a default state
	void Save(std::ofstream& file);
	void Load(std::ifstream& file);
	bool HasSkeleton() const { return !Bones.empty(); }// lets the caller know if there is a skeleton present
	// the set animation returns whether the animation changed or is still the same. 
	bool SetAnimIndex( int pAnimIndex);// this takes an index to set the current animation to
	bool SetAnimation(const std::string& name);// this takes a string to set the animation to, i.e. SetAnimation("Idle");
	// the next two functions are good if you want to change the direction of the current animation. You could use a forward walking animation and reverse it to get a walking backwards
	void PlayAnimationForward() { Animations[CurrentAnimIndex].PlayAnimationForward = true; }
	void PlayAnimationBackward() { Animations[CurrentAnimIndex].PlayAnimationForward = false; }
	//this function will adjust the current animations speed by a percentage. So, passing 100, would do nothing, passing 50, would decrease the speed by half, and 150 increase it by 50%
	void AdjustAnimationSpeedBy(float percent) { Animations[CurrentAnimIndex].TicksPerSecond*=percent/100.0f; }
	//This will set the animation speed
	void AdjustAnimationSpeedTo(float tickspersecond) { Animations[CurrentAnimIndex].TicksPerSecond=tickspersecond; }
	// get the animationspeed... in ticks per second
	float GetAnimationSpeed() const { return Animations[CurrentAnimIndex].TicksPerSecond; }

	// get the transforms needed to pass to the vertex shader. This will wrap the dt value passed, so it is safe to pass 50000000 as a valid number
	std::vector<XMMATRIX>& GetTransforms(float dt)
	{ 
		return Animations[CurrentAnimIndex].GetTransforms(dt);
	}

	int GetAnimationIndex() const { return CurrentAnimIndex; }
	std::string GetAnimationName() const { return Animations[CurrentAnimIndex].Name;  }
	//GetBoneIndex will return the index of the bone given its name. The index can be used to index directly into the vector returned from GetTransform
	int GetBoneIndex(const std::string& bname){ std::map<std::string, unsigned int>::iterator found = BonesToIndex.find(bname); if(found!=BonesToIndex.end()) return found->second; else return -1;}
	//GetBoneTransform will return the matrix of the bone given its name and the time. be careful with this to make sure and send the correct dt. If the dt is different from what the model is currently at, the transform will be off
	XMMATRIX GetBoneTransform(float dt, const std::string& bname) { int bindex=GetBoneIndex(bname); if(bindex == -1) return XMMATRIX(); return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }
	// same as above, except takes the index
	XMMATRIX GetBoneTransform(float dt, unsigned int bindex) {  return Animations[CurrentAnimIndex].GetTransforms(dt)[bindex]; }

	std::vector<cAnimEvaluator> Animations;// a std::vector that holds each animation 
	int CurrentAnimIndex;/** Current animation index */

protected:		

	
	cBone* Skeleton;/** Root node of the internal scene structure */
	std::map<std::string, cBone*> BonesByName;/** Name to node map to quickly find nodes by their name */
	std::map<std::string, unsigned int> BonesToIndex;/** Name to node map to quickly find nodes by their name */
	std::map<std::string, UINT> AnimationNameToId;// find animations quickly
	std::vector<cBone*> Bones;// DO NOT DELETE THESE when the destructor runs... THEY ARE JUST COPIES!!
	std::vector<XMMATRIX> Transforms;// temp array of transfrorms

	void SaveSkeleton(std::ofstream& file, cBone* pNode);
	cBone* LoadSkeleton(std::ifstream& file, cBone* pNode);

	void UpdateTransforms(cBone* pNode);
	void CalculateBoneToWorldTransform(cBone* pInternalNode);/** Calculates the global transformation matrix for the given internal node */

	void Calculate( float pTime);
	void CalcBoneMatrices();

	void ExtractAnimations(const aiScene* pScene);
	cBone* CreateBoneTree( aiNode* pNode, cBone* pParent);// Recursively creates an internal node structure matching the current scene and animation. 
	
};

#endif
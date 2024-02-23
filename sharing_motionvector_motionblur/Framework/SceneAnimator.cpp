#include "DXUT.h"
#include "SceneAnimator.h"
#include "ults.h"

void SceneAnimator::Release(){// this should clean everything up 
	CurrentAnimIndex = -1;
	Animations.clear();// clear all animations
	delete Skeleton;// This node will delete all children recursivly
	Skeleton = NULL;// make sure to zero it out
}

void SceneAnimator::Init(const aiScene* pScene){// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
	if(!pScene->HasAnimations()) 
		return;

	Release();
	
	Skeleton = CreateBoneTree( pScene->mRootNode, NULL);
	ExtractAnimations(pScene);
	
	for (unsigned int i = 0; i < pScene->mNumMeshes;++i){
		const aiMesh* mesh = pScene->mMeshes[i];
		
		for (unsigned int n = 0; n < mesh->mNumBones;++n){
			const aiBone* bone = mesh->mBones[n];
			std::map<std::string, cBone*>::iterator found = BonesByName.find(bone->mName.data);
			if(found != BonesByName.end()){// FOUND IT!!! woohoo, make sure its not already in the bone list
				bool skip = false;
				for(size_t j(0); j< Bones.size(); j++){
					std::string bname = bone->mName.data;
					if(Bones[j]->Name == bname) {
						skip = true;// already inserted, skip this so as not to insert the same bone multiple times
						break;
					}
				}
				if(!skip){// only insert the bone if it has not already been inserted
					std::string tes = found->second->Name;
					TransformMatrix(found->second->Offset, bone->mOffsetMatrix);
					
					// transpoce their matrix to get in the correct format
					D3DXMATRIX mTemp = found->second->Offset;
					D3DXMatrixTranspose(&mTemp, &mTemp);
					found->second->Offset = mTemp;

					Bones.push_back(found->second);
					BonesToIndex[found->first] = (unsigned int)Bones.size()-1;
				}
			} 
		}
	}
	Transforms.resize( Bones.size());
	float timestep = 1.0f/30.0f;// 30 per second
	for(size_t i(0); i< Animations.size(); i++){// pre calculate the animations
		SetAnimIndex((unsigned int)i);
		float dt = 0;
		for(float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond/30.0f){
			dt +=timestep;
			Calculate(dt);
			Animations[i].Transforms.push_back(std::vector<D3DXMATRIX>());
			std::vector<D3DXMATRIX>& trans = Animations[i].Transforms.back();
			for( size_t a = 0; a < Transforms.size(); ++a){
				D3DXMATRIX rotationmat =  Bones[a]->Offset * Bones[a]->GlobalTransform;
				trans.push_back(rotationmat);
			}
		}
	}
}

void SceneAnimator::ExtractAnimations(const aiScene* pScene){
	//OUTPUT_DEBUG_MSG("Extracting Animations . . ");
	for(size_t i(0); i< pScene->mNumAnimations; i++){
		Animations.push_back(AnimEvaluator(pScene->mAnimations[i]) );// add the animations
	}
	for(UINT i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, UINT>::value_type(Animations[i].Name, i));
	}
	CurrentAnimIndex=0;
}

bool SceneAnimator::SetAnimIndex( int  pAnimIndex){
	// no change, or the animations data is out of bounds
	if((size_t)pAnimIndex >= Animations.size()) 
		return false;

	int oldindex = CurrentAnimIndex;

	// only set this after the checks for good data and the object was actually inserted
	CurrentAnimIndex = pAnimIndex;

	return oldindex != CurrentAnimIndex;
}

void SceneAnimator::Calculate(float pTime){
	if( (CurrentAnimIndex < 0) | ((size_t)CurrentAnimIndex >= Animations.size()) )
		return;// invalid animation
	
	Animations[CurrentAnimIndex].Evaluate( pTime, BonesByName);
	UpdateTransforms(Skeleton);
}

// Calculates the bone matrices for the given mesh. 
void SceneAnimator::CalcBoneMatrices(){
	for( size_t a = 0; a < Transforms.size(); ++a){
		Transforms[a] =  Bones[a]->Offset * Bones[a]->GlobalTransform;
	}
}

// Recursively creates an internal node structure matching the current scene and animation.
cBone* SceneAnimator::CreateBoneTree( aiNode* pNode, cBone* pParent){
	cBone* internalNode = new cBone();// create a node
	internalNode->Name = pNode->mName.data;// get the name of the bone
	internalNode->Parent = pParent; //set the parent, in the case this is theroot node, it will be null

	BonesByName[internalNode->Name] = internalNode;// use the name as a key
	TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);
	
	D3DXMATRIX mTemp = internalNode->LocalTransform;
	D3DXMatrixTranspose(&mTemp, &mTemp);
	internalNode->LocalTransform = mTemp;

	// a copy saved
	internalNode->OriginalLocalTransform = internalNode->LocalTransform;
	CalculateBoneToWorldTransform(internalNode);

	// continue for all child nodes and assign the created internal nodes as our children
	// recursivly call this function on all children
	for( unsigned int a = 0; a < pNode->mNumChildren; a++)
	{
		internalNode->Children.push_back(CreateBoneTree( pNode->mChildren[a], internalNode));
	}
	return internalNode;
}

// Recursively updates the internal node transformations from the given matrix array
void SceneAnimator::UpdateTransforms(cBone* pNode) {
	CalculateBoneToWorldTransform( pNode);// update global transform as well
	// continue for all children
	for( std::vector<cBone*>::iterator it = pNode->Children.begin(); 
						it != pNode->Children.end(); ++it)
		UpdateTransforms( *it);
}

// Calculates the global transformation matrix for the given internal node
void SceneAnimator::CalculateBoneToWorldTransform(cBone* child){
	child->GlobalTransform = child->LocalTransform;
	cBone* parent = child->Parent;
	
	// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
	while( parent ){
		child->GlobalTransform *= parent->LocalTransform;
		parent  = parent->Parent;
	}
}
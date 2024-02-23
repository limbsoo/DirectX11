#include "DXUT.h"
#include "cAnimationController.h"
#include "ults.h"

// ------------------------------------------------------------------------------------------------
// Constructor on a given animation. 
cAnimEvaluator::cAnimEvaluator( const aiAnimation* pAnim) {
	mLastTime = 0.0;
	TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
	Duration = static_cast<float>(pAnim->mDuration);

	Name = std::string(pAnim->mName.data, pAnim->mName.length);
	if(Name.size()==0) 
		Name ="Animation" + std::to_string((long double)Animation_Indexer);

	Channels.resize(pAnim->mNumChannels);
	for( unsigned int a = 0; a < pAnim->mNumChannels; a++){		
		Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumPositionKeys; i++) Channels[a].mPositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumRotationKeys; i++) Channels[a].mRotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumScalingKeys; i++) Channels[a].mScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
	}
	mLastPositions.resize( pAnim->mNumChannels, std::make_tuple( 0, 0, 0));
}

unsigned int cAnimEvaluator::GetFrameIndexAt(float ptime){
	// get a [0.f ... 1.f) value by allowing the percent to wrap around 1
	ptime *= TicksPerSecond;
	
	float time = 0.0f;
	if( Duration > 0.0)
		time = fmod( ptime, Duration);

	float percent = time / Duration;
	if(!PlayAnimationForward) percent= (percent-1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
	return static_cast<unsigned int>(( static_cast<float>(Transforms.size()) * percent));
}
void cAnimEvaluator::Save(std::ofstream& file){
	UINT nsize = static_cast<UINT>(Name.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the animation name
	file.write(Name.c_str(), nsize);// the size of the animation name
	file.write(reinterpret_cast<char*>(&Duration), sizeof(Duration));// the duration
	file.write(reinterpret_cast<char*>(&TicksPerSecond), sizeof(TicksPerSecond));// the number of ticks per second
	nsize = static_cast<UINT>(Channels.size());// number of animation channels,
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number animation channels
	for(size_t j(0); j< Channels.size(); j++){// for each channel
		nsize =static_cast<UINT>(Channels[j].Name.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the name
		file.write(Channels[j].Name.c_str(), nsize);// the size of the animation name

		nsize =static_cast<UINT>(Channels[j].mPositionKeys.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		for(size_t i(0); i< nsize; i++){// for each channel
			file.write(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mTime), sizeof(Channels[j].mPositionKeys[i].mTime));// pos key
			file.write(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mValue), sizeof(Channels[j].mPositionKeys[i].mValue));// pos key
		}

		nsize =static_cast<UINT>(Channels[j].mRotationKeys.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		for(size_t i(0); i< nsize; i++){// for each channel
			file.write(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mTime), sizeof(Channels[j].mRotationKeys[i].mTime));// rot key
			file.write(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mValue), sizeof(Channels[j].mRotationKeys[i].mValue));// rot key
		}

		nsize =static_cast<UINT>(Channels[j].mScalingKeys.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		for(size_t i(0); i< nsize; i++){// for each channel
			file.write(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mTime), sizeof(Channels[j].mScalingKeys[i].mTime));// rot key
			file.write(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mValue), sizeof(Channels[j].mScalingKeys[i].mValue));// rot key
		}

	}
}
void cAnimEvaluator::Load(std::ifstream& file){
	UINT nsize = 0;
	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the animation name
	char temp[250];
	file.read(temp, nsize);// the size of the animation name
	temp[nsize]=0;// null char
	Name=temp;
	file.read(reinterpret_cast<char*>(&Duration), sizeof(Duration));// the duration
	file.read(reinterpret_cast<char*>(&TicksPerSecond), sizeof(TicksPerSecond));// the number of ticks per second
	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number animation channels
	Channels.resize(nsize);
	for(size_t j(0); j< Channels.size(); j++){// for each channel
		nsize =0;
		file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the name
		file.read(temp, nsize);// the size of the animation name
		temp[nsize]=0;// null char
		Channels[j].Name=temp;

		nsize =static_cast<UINT>(Channels[j].mPositionKeys.size());
		file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		Channels[j].mPositionKeys.resize(nsize);
		for(size_t i(0); i< nsize; i++){// for each channel
			file.read(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mTime), sizeof(Channels[j].mPositionKeys[i].mTime));// pos key
			file.read(reinterpret_cast<char*>(&Channels[j].mPositionKeys[i].mValue), sizeof(Channels[j].mPositionKeys[i].mValue));// pos key
		}

		nsize =static_cast<UINT>(Channels[j].mRotationKeys.size());
		file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		Channels[j].mRotationKeys.resize(nsize);
		for(size_t i(0); i< nsize; i++){// for each channel
			file.read(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mTime), sizeof(Channels[j].mRotationKeys[i].mTime));// pos key
			file.read(reinterpret_cast<char*>(&Channels[j].mRotationKeys[i].mValue), sizeof(Channels[j].mRotationKeys[i].mValue));// pos key
		}

		nsize =static_cast<UINT>(Channels[j].mScalingKeys.size());
		file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of position keys
		Channels[j].mScalingKeys.resize(nsize);
		for(size_t i(0); i< nsize; i++){// for each channel
			file.read(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mTime), sizeof(Channels[j].mScalingKeys[i].mTime));// pos key
			file.read(reinterpret_cast<char*>(&Channels[j].mScalingKeys[i].mValue), sizeof(Channels[j].mScalingKeys[i].mValue));// pos key
		}
	}
	mLastPositions.resize( Channels.size(), std::make_tuple( 0, 0, 0));
}
// ------------------------------------------------------------------------------------------------
// Evaluates the animation tracks for a given time stamp. 
void cAnimEvaluator::Evaluate( float pTime, std::map<std::string, cBone*>& bones) {

	pTime *= TicksPerSecond;
	
	float time = 0.0f;
	if( Duration > 0.0)
		time = fmod( pTime, Duration);

	// calculate the transformations for each animation channel
	for( unsigned int a = 0; a < Channels.size(); a++){
		const cAnimationChannel* channel = &Channels[a];
		std::map<std::string, cBone*>::iterator bonenode = bones.find(channel->Name);

		if(bonenode == bones.end()) { 
			continue;
		}

		// ******** Position *****
		aiVector3D presentPosition( 0, 0, 0);
		if( channel->mPositionKeys.size() > 0){
			// Look for present frame number. Search from last position if time is after the last time, else from beginning
			// Should be much quicker than always looking from start for the average use case.
			unsigned int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[a]): 0;
			while( frame < channel->mPositionKeys.size() - 1){
				if( time < channel->mPositionKeys[frame+1].mTime){
					break;
				}
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->mPositionKeys.size();
	
			const aiVectorKey& key = channel->mPositionKeys[frame];
			const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0)
				diffTime += Duration;
			if( diffTime > 0) {
				float factor = float( (time - key.mTime) / diffTime);
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
			} else {
				presentPosition = key.mValue;
			}
			std::get<0>(mLastPositions[a]) = frame;
		}
		// ******** Rotation *********
		aiQuaternion presentRotation( 1, 0, 0, 0);
		if( channel->mRotationKeys.size() > 0)
		{
			unsigned int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[a]) : 0;
			while( frame < channel->mRotationKeys.size()  - 1){
				if( time < channel->mRotationKeys[frame+1].mTime)
					break;
				frame++;
			}

			// interpolate between this frame's value and next frame's value
			unsigned int nextFrame = (frame + 1) % channel->mRotationKeys.size() ;

			const aiQuatKey& key = channel->mRotationKeys[frame];
			const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;
			if( diffTime < 0.0) diffTime += Duration;
			if( diffTime > 0) {
				float factor = float( (time - key.mTime) / diffTime);
				aiQuaternion::Interpolate( presentRotation, key.mValue, nextKey.mValue, factor);
			} else presentRotation = key.mValue;
			std::get<1>(mLastPositions[a]) = frame;
		}

		// ******** Scaling **********
		aiVector3D presentScaling( 1, 1, 1);
		if( channel->mScalingKeys.size() > 0) {
			unsigned int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[a]) : 0;
			while( frame < channel->mScalingKeys.size() - 1){
				if( time < channel->mScalingKeys[frame+1].mTime)
					break;
				frame++;
			}

			presentScaling = channel->mScalingKeys[frame].mValue;
			std::get<2>(mLastPositions[a]) = frame;
		}

		aiMatrix4x4 mat = aiMatrix4x4( presentRotation.GetMatrix());

		mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
		mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
		mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
		mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
		mat.Transpose();
		
		TransformMatrix(bonenode->second->LocalTransform, mat);
	}
	mLastTime = time;
}

void cSceneAnimator::Release(){// this should clean everything up 
	CurrentAnimIndex = -1;
	Animations.clear();// clear all animations
	delete Skeleton;// This node will delete all children recursivly
	Skeleton = NULL;// make sure to zero it out
}
void cSceneAnimator::Init(const aiScene* pScene){// this will build the skeleton based on the scene passed to it and CLEAR EVERYTHING
	if(!pScene->HasAnimations()) return;
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

					//found->second->Offset.Transpose();// transpoce their matrix to get in the correct format
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
void cSceneAnimator::Save(std::ofstream& file){
	// first recursivly save the skeleton
	if(Skeleton)
		SaveSkeleton(file, Skeleton);

	UINT nsize = static_cast<UINT>(Animations.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of animations	
	for(UINT i(0); i< nsize; i++){
		Animations[i].Save(file);
	}

	nsize = static_cast<UINT>(Bones.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of bones

	for(UINT i(0); i< Bones.size(); i++){
		nsize = static_cast<UINT>(Bones[i]->Name.size());
		file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the bone name
		file.write(Bones[i]->Name.c_str(), nsize);// the name of the bone
	}

}
void cSceneAnimator::Load(std::ifstream& file){
	Release();// make sure to clear this before writing new data
	Skeleton = LoadSkeleton(file, NULL);
	UINT nsize = 0;
	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of animations
	Animations.resize(nsize);
	for(UINT i(0); i< nsize; i++){
		Animations[i].Load(file);
	}
	for(UINT i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, UINT>::value_type(Animations[i].Name, i));
	}
	if(Animations.size() >0) CurrentAnimIndex =0;// set it to the first animation if there are any
	char bname[250];
	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of bones
	Bones.resize(nsize);

	for(UINT i(0); i< Bones.size(); i++){	
		file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the size of the bone name
		file.read(bname, nsize);// the size of the bone name
		bname[nsize]=0;
		std::map<std::string, cBone*>::iterator found = BonesByName.find(bname);
		BonesToIndex[found->first] = i;
		cBone* tep = found->second;
		Bones[i]=tep;
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

void cSceneAnimator::ExtractAnimations(const aiScene* pScene){
	for(size_t i(0); i< pScene->mNumAnimations; i++){
		Animations.push_back(cAnimEvaluator(pScene->mAnimations[i]) );// add the animations
	}
	for(UINT i(0); i< Animations.size(); i++){// get all the animation names so I can reference them by name and get the correct id
		AnimationNameToId.insert(std::map<std::string, UINT>::value_type(Animations[i].Name, i));
	}
	CurrentAnimIndex=0;
	SetAnimation("Idle");
}
bool cSceneAnimator::SetAnimation(const std::string& name){
	std::map<std::string, UINT>::iterator itr = AnimationNameToId.find(name);
	int oldindex = CurrentAnimIndex;
	if(itr !=AnimationNameToId.end()) CurrentAnimIndex = itr->second;
	return oldindex != CurrentAnimIndex;
}
bool cSceneAnimator::SetAnimIndex( int  pAnimIndex){
	if((size_t)pAnimIndex >= Animations.size()) return false;// no change, or the animations data is out of bounds
	int oldindex = CurrentAnimIndex;
	CurrentAnimIndex = pAnimIndex;// only set this after the checks for good data and the object was actually inserted
	return oldindex != CurrentAnimIndex;
}

// ------------------------------------------------------------------------------------------------
// Calculates the node transformations for the scene. 
void cSceneAnimator::Calculate(float pTime){
	if( (CurrentAnimIndex < 0) | ((size_t)CurrentAnimIndex >= Animations.size()) ) return;// invalid animation
	
	Animations[CurrentAnimIndex].Evaluate( pTime, BonesByName);
	UpdateTransforms(Skeleton);
}

//void UQTtoUDQ(vec4* dual, quat q, vec4& tran ) {
//    dual[0].x = q.x ; 
//    dual[0].y = q.y ; 
//    dual[0].z = q.z ; 
//    dual[0].w = q.w ; 
//    dual[1].x = 0.5f *  ( tran[0] * q.w + tran[1] * q.z - tran[2] * q.y ) ; 
//    dual[1].y = 0.5f *  (-tran[0] * q.z + tran[1] * q.w + tran[2] * q.x ) ; 
//    dual[1].z = 0.5f *  ( tran[0] * q.y - tran[1] * q.x + tran[2] * q.w ) ; 
//    dual[1].w = -0.5f * ( tran[0] * q.x + tran[1] * q.y + tran[2] * q.z ) ; 
//}

// ------------------------------------------------------------------------------------------------
// Calculates the bone matrices for the given mesh. 
void cSceneAnimator::CalcBoneMatrices(){
	for( size_t a = 0; a < Transforms.size(); ++a){
		Transforms[a] =  Bones[a]->Offset * Bones[a]->GlobalTransform;
	}
}

// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
cBone* cSceneAnimator::CreateBoneTree( aiNode* pNode, cBone* pParent){
	cBone* internalNode = new cBone();// create a node
	internalNode->Name = pNode->mName.data;// get the name of the bone
	internalNode->Parent = pParent; //set the parent, in the case this is theroot node, it will be null

	BonesByName[internalNode->Name] = internalNode;// use the name as a key
	TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);

	//internalNode->LocalTransform.Transpose();
	D3DXMATRIX mTemp = internalNode->LocalTransform;
	D3DXMatrixTranspose(&mTemp, &mTemp);
	internalNode->LocalTransform = mTemp;


	internalNode->OriginalLocalTransform = internalNode->LocalTransform;// a copy saved
	CalculateBoneToWorldTransform(internalNode);

	// continue for all child nodes and assign the created internal nodes as our children
	for( unsigned int a = 0; a < pNode->mNumChildren; a++){// recursivly call this function on all children
		internalNode->Children.push_back(CreateBoneTree( pNode->mChildren[a], internalNode));
	}
	return internalNode;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void cSceneAnimator::UpdateTransforms(cBone* pNode) {
	CalculateBoneToWorldTransform( pNode);// update global transform as well
	for( std::vector<cBone*>::iterator it = pNode->Children.begin(); it != pNode->Children.end(); ++it)// continue for all children
		UpdateTransforms( *it);
}

// ------------------------------------------------------------------------------------------------
// Calculates the global transformation matrix for the given internal node
void cSceneAnimator::CalculateBoneToWorldTransform(cBone* child){
	child->GlobalTransform = child->LocalTransform;
	cBone* parent = child->Parent;
	while( parent ){// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
		child->GlobalTransform *= parent->LocalTransform;
		parent  = parent->Parent;// get the parent of the bone we are working on 
	}
}


void cSceneAnimator::SaveSkeleton(std::ofstream& file, cBone* parent){
	UINT nsize = static_cast<UINT>(parent->Name.size());
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of chars
	file.write(parent->Name.c_str(), nsize);// the name of the bone
	file.write(reinterpret_cast<char*>(&parent->Offset), sizeof(parent->Offset));// the bone offsets
	file.write(reinterpret_cast<char*>(&parent->OriginalLocalTransform), sizeof(parent->OriginalLocalTransform));// original bind pose
	nsize = static_cast<UINT>(parent->Children.size());// number of children
	file.write(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of children
	for( std::vector<cBone*>::iterator it = parent->Children.begin(); it != parent->Children.end(); ++it)// continue for all children
		SaveSkeleton(file, *it); 
}

cBone* cSceneAnimator::LoadSkeleton(std::ifstream& file, cBone* parent){
	cBone* internalNode = new cBone();// create a node
	internalNode->Parent = parent; //set the parent, in the case this is theroot node, it will be null
	UINT nsize=0;
	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of chars
	char temp[250];
	file.read(temp, nsize);// the name of the bone
	temp[nsize]=0;
	internalNode->Name = temp;
	BonesByName[internalNode->Name] = internalNode;// use the name as a key
	file.read(reinterpret_cast<char*>(&internalNode->Offset), sizeof(internalNode->Offset));// the bone offsets
	file.read(reinterpret_cast<char*>(&internalNode->OriginalLocalTransform), sizeof(internalNode->OriginalLocalTransform));// original bind pose
	
	internalNode->LocalTransform = internalNode->OriginalLocalTransform;// a copy saved
	CalculateBoneToWorldTransform(internalNode);

	file.read(reinterpret_cast<char*>(&nsize), sizeof(UINT));// the number of children

	// continue for all child nodes and assign the created internal nodes as our children
	for( unsigned int a = 0; a < nsize && file; a++){// recursivly call this function on all children
		internalNode->Children.push_back(LoadSkeleton(file, internalNode));
	}
	return internalNode;

}
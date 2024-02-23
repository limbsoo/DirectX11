#include "DXUT.h"
#include "AnimEvaluator.h"
#include <string>
#include "ults.h"

AnimEvaluator::AnimEvaluator( const aiAnimation* pAnim) {
	mLastTime = 0.0;
	TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
	Duration = static_cast<float>(pAnim->mDuration);
	Name = std::string(pAnim->mName.data, pAnim->mName.length);
	
	string sTemp = to_string((long double)Animation_Indexer);

	if(Name.size()==0) 
		Name ="Animation" + sTemp;

	Channels.resize(pAnim->mNumChannels);
	for( unsigned int a = 0; a < pAnim->mNumChannels; a++)
	{		
		Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumPositionKeys; i++) 
			Channels[a].mPositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumRotationKeys; i++) 
			Channels[a].mRotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);
		for(unsigned int i(0); i< pAnim->mChannels[a]->mNumScalingKeys; i++) 
			Channels[a].mScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
	}

	mLastPositions.resize( pAnim->mNumChannels, std::make_tuple( 0, 0, 0));
}

unsigned int AnimEvaluator::GetFrameIndexAt(float ptime)
{
	// get a [0.f ... 1.f) value by allowing the percent to wrap around 1
	ptime *= TicksPerSecond;
	
	float time = 0.0f;
	if( Duration > 0.0)
		time = fmod( ptime, Duration);

	float percent = time / Duration;
	//if(!PlayAnimationForward) percent= (percent-1.0f)*-1.0f;// this will invert the percent so the animation plays backwards
	return static_cast<unsigned int>(( static_cast<float>(Transforms.size()) * percent));
}

void AnimEvaluator::Evaluate( float pTime, std::map<std::string, cBone*>& bones) {

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


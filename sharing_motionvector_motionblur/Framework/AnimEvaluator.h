/** Calculates a pose for a given time of an animation */
#pragma once

#ifndef AV_ANIMEVALUATOR_H_INCLUDED
#define AV_ANIMEVALUATOR_H_INCLUDED

#include <assimp\types.h>
#include <assimp\anim.h>
#include <vector>
#include <tuple>
#include <map>
#include "Types.h"
#include <d3dx9math.h>

using namespace std;

/** Calculates transformations for a given timestamp from a set of animation tracks. Not directly useful,
 * better use the AnimPlayer class.
 */
class AnimEvaluator
{
	public:
		AnimEvaluator(): mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f), 
							PlayAnimationForward(true), Animation_Indexer(0) {}
		AnimEvaluator( const aiAnimation* pAnim);

		void Evaluate( float pTime, std::map<std::string, cBone*>& bones);

		std::vector<D3DXMATRIX>& GetTransforms(float dt){ 
			return Transforms[GetFrameIndexAt(dt)]; 
		}
		unsigned int GetFrameIndexAt(float time);


		//-----------------------------Members--------------------------------
		string Name;

		// this is only used if an animation has no name. I assigned it Animation + Animation_Indexer
		//uint32_t Animation_Indexer;
		UINT Animation_Indexer;

		vector<cAnimationChannel> Channels;
		bool PlayAnimationForward;// play forward == true, play backward == false

		float mLastTime;
		float TicksPerSecond;
		float Duration;	
	
		vector<tuple<unsigned int, unsigned int, unsigned int>> mLastPositions;

		//, QuatTransforms;/** Array to return transformations results inside. */
		vector<vector<D3DXMATRIX>> Transforms;
};

#endif // AV_ANIMEVALUATOR_H_INCLUDED

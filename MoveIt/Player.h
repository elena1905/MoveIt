/*
 * Player.h
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#pragma once


#include <array>
#include <queue>

#include "MathHelper.h"

#include "stdafx.h"
#include "NuiApi.h"
#include "Ogre3DBase.h"


class Player
{
	static const int		ARR_SIZE = 8;

public:
    Player(Ogre::SceneManager* sceneMgr);
    ~Player();

	void InitEntity(const Ogre::String& entityName, const Ogre::String& meshName);
	void InitSceneNode(const Ogre::String& nodeName, const Ogre::Vector3& translate);
	void InitSkeleton(void);

	void CreateAnimation(const Ogre::String& animationName, const Ogre::Real& timeLength);
	void CreateNodeTrack(const unsigned short& jointHandle, Ogre::Node* jointNode, const Ogre::Real& timeLength);
	void CreateKeyFrame(const unsigned short& index, const Ogre::Real& timeLength);

	void ExportAnimation(const Ogre::String& fileName);
	void ImportAnimation(const Ogre::String& fileName);

	void PlayMotion(void);
	void PlayAnimation(const Ogre::String& animName, const Ogre::Real& timeElapsed);

	void SetQuaternionQueue(std::queue<Ogre::Quaternion> quaternionQueue[ARR_SIZE]);
	void SetCentralPosQueue(std::queue<Ogre::Vector3> centralPosQueue);
	

private:
	void EnableBoneControl(void);
	void DisableBoneControl(void);

	// 
	std::queue<Ogre::Quaternion> QuaternionQueue[ARR_SIZE];
	std::queue<Ogre::Vector3> CentralPosQueue;

	// 
	Ogre::String FileName;
	Ogre::String JointName[ARR_SIZE];

	// Global Scene Manager, passed from main program
	Ogre::SceneManager* SceneManager;
	
	/* Skeleton Entities and skeleton SceneNode */
	Ogre::SceneNode* SceneNode;
	Ogre::Entity* Entity;

	// Ogre Skeleton and Bones
	std::array<Ogre::Bone*, ARR_SIZE> BoneArray;
	Ogre::SkeletonInstance* SkeletonInstance;

	// Write animations to *.skeleton file
	Ogre::Bone* Bone;
	Ogre::Animation* Animation;
	Ogre::NodeAnimationTrack* NodeTrack;
	Ogre::TransformKeyFrame* KeyFrame;
	Ogre::AnimationState* AnimationState;
	Ogre::SkeletonSerializer* SkeletonSerializer;
};

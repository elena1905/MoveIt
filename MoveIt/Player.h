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

#include "MathHelper.h"

#include "stdafx.h"
#include "Ogre3DBase.h"


class Player
{
	static const int		ARR_SIZE = 8;

public:
    Player();
    ~Player();

	void Init();
	
	MathHelper* m_pMath;
	
	/* Skeleton Entities and skeleton SceneNode */
	Ogre::SceneNode* SceneNode;
	Ogre::Entity* Entity;

	// Ogre Skeleton and Bones
	std::array<Ogre::Bone*, ARR_SIZE> BoneArray;
	Ogre::SkeletonInstance* SkeletonInstance;

	// Test manually created animations
	Ogre::AnimationState* AnimationState;
};

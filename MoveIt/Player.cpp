/*
 * Player.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "Player.h"


Player::Player()
{
	m_pMath = new MathHelper();
}


Player::~Player()
{
    delete m_pMath;
}


void Player::Init()
{
	//Entity->setCastShadows(true);
	SceneNode->attachObject(Entity);
	
	// Rotate the player model to face the camera
	SceneNode->rotate(m_pMath->PiQuaternion);
	
	
	/* =========== Get Ogre Bones by Joints ========== */
	SkeletonInstance = Entity->getSkeleton();

	/*
	Ogre::Bone* myBones[ARR_SIZE];
	for (int i = 0; i < ARR_SIZE; i++)
	{
		myBones[i] = m_pSkeleton->getBone("Joint10");
	}
	*/
	
	// Get Ogre Bones to match Kinect Joint
	BoneArray[0] = SkeletonInstance->getBone("Joint10");	// ShoulderLeft
	BoneArray[1] = SkeletonInstance->getBone("Joint11");	// ElbowLeft
	BoneArray[2] = SkeletonInstance->getBone("Joint15");	// ShoulderRight
	BoneArray[3] = SkeletonInstance->getBone("Joint16");	// ElbowRight
	BoneArray[4] = SkeletonInstance->getBone("Joint18");	// HipLeft
	BoneArray[5] = SkeletonInstance->getBone("Joint19");	// KneeLeft
	BoneArray[6] = SkeletonInstance->getBone("Joint23");	// HipRight
	BoneArray[7] = SkeletonInstance->getBone("Joint24");	// KneeRight

	// Enable bones to be manually controlled
	for (int i = 0; i < ARR_SIZE; i++)
	{
		BoneArray[i]->setManuallyControlled(true);
	}
}

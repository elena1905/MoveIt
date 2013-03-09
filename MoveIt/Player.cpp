/*
 * Player.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "Player.h"


Player::Player(Ogre::SceneManager* sceneMgr)
{
	SceneManager = sceneMgr;
}


Player::~Player()
{
    
}


// ------------------------------------------------------------
void Player::InitEntity(const Ogre::String& entityName, const Ogre::String& meshName)
{
	Entity = SceneManager->createEntity(entityName, meshName);
}


// ------------------------------------------------------------
void Player::InitSceneNode(const Ogre::String& nodeName, const Ogre::Vector3& translate)
{
	SceneNode = SceneManager->getRootSceneNode()->createChildSceneNode(nodeName, translate);
	SceneNode->attachObject(Entity);
	SceneNode->rotate(MathHelper::PiQuaternion);
}


// ------------------------------------------------------------
void Player::InitSkeleton()
{
	/* =========== Get Ogre Bones by Joints ========== */
	SkeletonInstance = Entity->getSkeleton();

	// Define joint names
	JointName[0] = "Joint10";
	JointName[1] = "Joint11";
	JointName[2] = "Joint15";
	JointName[3] = "Joint16";
	JointName[4] = "Joint18";
	JointName[5] = "Joint19";
	JointName[6] = "Joint23";
	JointName[7] = "Joint24";

	// Attach joints to bones
	for (int i = 0; i < ARR_SIZE; i++)
	{
		BoneArray[i] = SkeletonInstance->getBone(JointName[i]);
		BoneArray[i]->setInheritOrientation(false);
	}
}


// ------------------------------------------------------------
void Player::CreateAnimation(const Ogre::String& animationName, const Ogre::Real& timeLength)
{

}


// ------------------------------------------------------------
void Player::CreateNodeTrack(const unsigned short& jointHandle, Ogre::Node* jointNode, const Ogre::Real& timeLength)
{

}


// ------------------------------------------------------------
void Player::CreateKeyFrame(const unsigned short& index, const Ogre::Real& timeLength)
{

}


// ------------------------------------------------------------
void Player::ExportAnimation(const Ogre::String& fileName)
{

}


// ------------------------------------------------------------
void Player::ImportAnimation(const Ogre::String& fileName)
{

}


// ------------------------------------------------------------
void Player::PlayMotion(void)
{
	for (int j = 0; j < NUI_SKELETON_COUNT; j++)
	{
		for (int i = 0; i < ARR_SIZE; i++)
		{
			EnableBoneControl(BoneArray[i]);

			if (!QuaternionQueue[i].empty() && !CentralPosQueue.empty())
			{
				SceneNode->setPosition(CentralPosQueue.front());
				BoneArray[i]->setOrientation(QuaternionQueue[i].front());

				CentralPosQueue.pop();
				QuaternionQueue[i].pop();
			}
		}
	}
}


// ------------------------------------------------------------
void Player::PlayAnimation(const Ogre::String& animName, const Ogre::Real& timeElapsed)
{
	// Disable bones to be manually controlled
	for (int i = 0; i < ARR_SIZE; i++)
	{
		DisableBoneControl(BoneArray[i]);
	}

	AnimationState = Entity->getAnimationState(animName);
	AnimationState->setEnabled(true);
	AnimationState->setLoop(true);
	AnimationState->addTime(timeElapsed);
}


// ------------------------------------------------------------
void Player::EnableBoneControl(Ogre::Bone* pBone)
{
	// Enable bone to be manually controlled
	pBone->setManuallyControlled(true);
}


// ------------------------------------------------------------
void Player::DisableBoneControl(Ogre::Bone* pBone)
{
	// Disable bone to be manually controlled
	pBone->setManuallyControlled(false);
}


// ------------------------------------------------------------
void Player::SetQuaternionQueue(std::queue<Ogre::Quaternion> quaternionQueue[ARR_SIZE])
{
	for (int i = 0; i < ARR_SIZE; i++)
	{
		QuaternionQueue[i].swap(quaternionQueue[i]);
	}
}


// ------------------------------------------------------------
void Player::SetCentralPosQueue(std::queue<Ogre::Vector3> centralPosQueue)
{
	CentralPosQueue.swap(centralPosQueue);
}
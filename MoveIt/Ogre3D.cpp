/*
 * Ogre3D.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "Ogre3D.h"


//-------------------------------------------------------------------------------------
Ogre3D::Ogre3D(void)
{
	m_pKinectHelper = new KinectHelper();

	mTimeElapsed = 0.0f;
}

//-------------------------------------------------------------------------------------
Ogre3D::~Ogre3D(void)
{
	delete m_pPlayer;
	delete m_pKinectHelper;
}

//-------------------------------------------------------------------------------------
void Ogre3D::createScene(void)
{
	/* ========= Create KinectSensor instance for the first connected sensor ========== */
	m_pKinectHelper->CreateFirstConnected();


	/* ========= Set Light and Shadow Type ========== */
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);


	/* =========== Create skeleton entities and attach them to skeletonNode ========== */
	m_pPlayer = new Player(mSceneMgr);
	m_pPlayer->InitEntity("Player", "ninja.mesh");
	m_pPlayer->InitSceneNode("PlayerNode", Ogre::Vector3(160.0f, 0.0f, 0.0f));
	m_pPlayer->InitSkeleton();


	/* ================ Set Ground and Shadow Effect ================ */
	// Create a plane
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	
	// Register the plane
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	// Create an Entity from the plane mesh; Display it on the screen
	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);

	// Set the texture of entGround; Do not cast shadows on it
	entGround->setMaterialName("Examples/Rockwall");
	entGround->setCastShadows(false);
}

//-------------------------------------------------------------------------------------
void Ogre3D::createCamera(void)
{
	// Create the Camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Set the Camera's position and direction
	mCamera->setPosition(Ogre::Vector3(180, 50, 300));
	mCamera->lookAt(Ogre::Vector3(180, 120, 0));

	// Set the near clip distance
	mCamera->setNearClipDistance(5);

	// Test: set the far clip distance
	mSceneMgr->setShadowUseInfiniteFarPlane(false);
	mCamera->setFarClipDistance(1000);

	// create a default camera controller
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

//-------------------------------------------------------------------------------------
void Ogre3D::createViewports(void)
{
	// Create one Viewport, entire window
	m_pViewport = mWindow->addViewport(mCamera);

	// Set background colour
	m_pViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(m_pViewport->getActualWidth()) / Ogre::Real(m_pViewport->getActualHeight()));
}

//-------------------------------------------------------------------------------------
bool Ogre3D::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	/* === Kinect: process skeleton streams and draw the cube according to player's position === */
	m_pKinectHelper->ProcessSkeleton();

	m_pPlayer->SetCentralPosQueue(m_pKinectHelper->m_CentralPosQueue);
	m_pPlayer->SetQuaternionQueue(m_pKinectHelper->m_QuaternionQueue);

	m_pPlayer->PlayMotion();


	// Test manually created animations or test animations from rewritten *.skeleton file
	//m_pPlayer->AnimationState->addTime(evt.timeSinceLastFrame);

	return Ogre3DBase::frameRenderingQueued(evt);
}

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

}
//-------------------------------------------------------------------------------------
Ogre3D::~Ogre3D(void)
{

}

//-------------------------------------------------------------------------------------
void Ogre3D::createScene(void)
{
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create an Entity; Cast shadows to it; Attach it to a SceneNode
	mEntity = mSceneMgr->createEntity("Cube", "cube.mesh");
	mEntity->setCastShadows(true);
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CubeNode", Ogre::Vector3(0.0f, 10.0f, 25.0f));
	mNode->attachObject(mEntity);
	mNode->setScale(0.05f, 0.05f, 0.05f);
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entCube);

	
	// Create the walking list
    mWalkList.push_back(Ogre::Vector3(550.0f, 10.0f, 50.0f ));
    mWalkList.push_back(Ogre::Vector3(-100.0f, 10.0f, -200.0f));
    // Create objects so we can see movement
    Ogre::Entity *ent;
    Ogre::SceneNode *node;
 
    ent = mSceneMgr->createEntity("Knot1", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot1Node", Ogre::Vector3(0.0f, 10.0f, 25.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);
 
    ent = mSceneMgr->createEntity("Knot2", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot2Node", Ogre::Vector3(550.0f, 10.0f, 50.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);
 
    ent = mSceneMgr->createEntity("Knot3", "knot.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot3Node", Ogre::Vector3(-100.0f, 10.0f, -200.0f));
    node->attachObject(ent);
    node->setScale(0.1f, 0.1f, 0.1f);
	

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

	// Create the light, set its type point and position, set diffuse and specular colour to red
	Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setPosition(Ogre::Vector3(0, 150, 250));
	pointLight->setDiffuseColour(1.0, 0.0, 0.0);
	pointLight->setSpecularColour(1.0, 0.0, 0.0);

	// Create the light, set its type directional and direction, set diffuse and specular colour to yellow
	Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
	directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
	directionalLight->setDirection(Ogre::Vector3(0, -1, 1));
	directionalLight->setDiffuseColour(Ogre::ColourValue(0.25, 0.25, 0.0));
	directionalLight->setSpecularColour(Ogre::ColourValue(0.25, 0.25, 0.0));

	// Create the light, set its type spotlight, position, direction and light range,
	// set diffuse and specular colour blue
	Ogre::Light* spotLight = mSceneMgr->createLight("spotLight");
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setDirection(Ogre::Vector3(-1, -1, 0));
	spotLight->setPosition(Ogre::Vector3(300, 300, 0));
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));
	spotLight->setDiffuseColour(0, 0, 1.0);
	spotLight->setSpecularColour(0, 0, 1.0);
}

//-------------------------------------------------------------------------------------
void Ogre3D::createCamera(void)
{
	// Create the Camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Set the Camera's position and direction
	mCamera->setPosition(Ogre::Vector3(0, 10, 500));
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));

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
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);

	// Set background colour
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}


//-------------------------------------------------------------------------------------
void Ogre3D::createFrameListener(void)
{
    Ogre3DBase::createFrameListener();
    
	/*
	// Set idle animation
    mAnimationState = mEntity->getAnimationState("Idle");
    mAnimationState->setLoop(true);
    mAnimationState->setEnabled(true);
	*/

    // Set default values for variables
    mWalkSpeed = 35.0f;
    mDirection = Ogre::Vector3::ZERO;
}

//-------------------------------------------------------------------------------------
bool Ogre3D::nextLocation(void)
{
    if (mWalkList.empty()) return false;
    mDestination = mWalkList.front();  // this gets the front of the deque
    mWalkList.pop_front();             // this removes the front of the deque
    mDirection = mDestination - mNode->getPosition();
    mDistance = mDirection.normalise();
    return true;
}

//-------------------------------------------------------------------------------------
bool Ogre3D::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if (mDirection == Ogre::Vector3::ZERO)
	{
		if (nextLocation())
		{
			/*
			// Set walking animation
			mAnimationState = mEntity->getAnimationState("Walk");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
			*/
		}//if
	}
	else
	{
		Ogre::Real move = mWalkSpeed * evt.timeSinceLastFrame;
		mDistance -= move;
		if (mDistance <= 0.0f)
		{                 
			mNode->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;				
			// Set animation based on if the robot has another point to walk to. 
			if (!nextLocation())
			{
				/*
				// Set Idle animation                     
				mAnimationState = mEntity->getAnimationState("Idle");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
				*/
			}
			else
			{
				// Rotation Code will go here later
				Ogre::Vector3 src = mNode->getOrientation() * Ogre::Vector3::UNIT_X;
				if ((1.0f + src.dotProduct(mDirection)) < 0.0001f)
				{
					mNode->yaw(Ogre::Degree(180));						
				}
				else
				{
					Ogre::Quaternion quat = src.getRotationTo(mDirection);
					mNode->rotate(quat);
				} // else
			}//else
		}
		else
		{
			mNode->translate(mDirection * move);
		} // else
	} // if
	
	/*
	mAnimationState->addTime(evt.timeSinceLastFrame);
	*/

	return Ogre3DBase::frameRenderingQueued(evt);
}

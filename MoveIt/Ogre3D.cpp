/*
 * Ogre3D.cpp
 *
 * Copyright@Wenchong Chen
 *
 * Email: wenchong.chen@outlook.com
 *
 */

#include "Ogre3D.h"
#include "stdafx.h"
#include <strsafe.h>

//-------------------------------------------------------------------------------------
Ogre3D::Ogre3D(void):
	m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
    m_pNuiSensor(NULL)
{

}

//-------------------------------------------------------------------------------------
Ogre3D::~Ogre3D(void)
{
	if (m_pNuiSensor)
    {
        m_pNuiSensor->NuiShutdown();
    }

    if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    {
        CloseHandle(m_hNextSkeletonEvent);
    }
}

//-------------------------------------------------------------------------------------
void Ogre3D::createScene(void)
{
	/* ========= Kinect: Create KinectSensor instance for the first connected sensor ========== */
	CreateFirstConnected();
	/* ========= End of Kinect: Create KinectSensor instance for the first connected sensor === */

	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		m_Points[i] = Ogre::Vector3(0, 0, 0);
	}

	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);


	/* =========== Kinect: create skeleton entities and attach them to skeletonNode ========== */
	m_pPlayer = mSceneMgr->createEntity("Player", "robot.mesh");
	m_pPlayer->setCastShadows(true);
	m_pPlayerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode", Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pPlayerNode->attachObject(m_pPlayer);
//>>>	m_pPlayerNode->setScale(0.08f, 0.08f, 0.08f);

	// Rotate the player model to face the camera
	Ogre::Quaternion q;
	Ogre::Math pi;
	q.FromAngleAxis(Ogre::Radian(-pi.PI / 2), Ogre::Vector3(0, 1, 0));
	m_pPlayerNode->rotate(q);

	m_pSkeleton = m_pPlayer->getSkeleton();
//>>>	m_pBones = m_pSkeleton->getRootBone();
	m_pBones = m_pSkeleton->getBone("Joint17");
	m_pBones->setManuallyControlled(true);
//>>>	m_pBones2 = m_pSkeleton->getBone("Joint4");
//>>>	m_pBones2->setManuallyControlled(true);
	
	

	// Test: moving a cube along with the movement of player's head
	m_pHead = mSceneMgr->createEntity("Head", "cube.mesh");
	m_pSkeletonNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	//m_pSkeletonNode->attachObject(m_pHead);
	m_pSkeletonNode->setScale(0.1f, 0.1f, 0.1f);
	/* =========== End of Kinect: create skeleton entities and attach them to skeletonNode ========== */


	/* =========== Test: move a cube between two knots ========== */
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
	/* =========== End of Test: move a cube between two knots ========== */


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
	mCamera->setPosition(Ogre::Vector3(0, 50, 200));
	mCamera->lookAt(Ogre::Vector3(0, 20, 0));

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
	/*
	// Make the character look at the camera
	Ogre::Vector3 posPlayer = m_pPlayerNode->getPosition();
	Ogre::Vector3 posCamera = mCamera->getPosition();

	Ogre::Vector3 toCamera = posPlayer - posCamera;
	toCamera.normalise();

	Ogre::Vector3 basis = Ogre::Vector3(0.0f, 0.0f, -1.0f);

	// Rotation angle
	Ogre::Radian rotAngle = Ogre::Radian();
	
	if (m_pPlayerNode->getOrientation().x < 0)
	{
		rotAngle = Ogre::Math().ACos(basis.dotProduct(toCamera));
	}
	else
	{
		rotAngle = -Ogre::Math().ACos(basis.dotProduct(toCamera));
	}

	Ogre::Quaternion quaPlayer;
	quaPlayer.FromAngleAxis(rotAngle, Ogre::Vector3::UNIT_Y);
	
	Ogre::Vector3 finalDirec;
	finalDirec.x = m_pPlayerNode->getOrientation().x;
	finalDirec.y = m_pPlayerNode->getOrientation().y;
	finalDirec.z = m_pPlayerNode->getOrientation().z;
	m_pPlayerNode->setDirection(quaPlayer * finalDirec);
	*/
	//Ogre::Vector3* targetPos = new Ogre::Vector3(0.0f, 30.0f, 300.0f);
	//m_pPlayerNode->lookAt(Ogre::Vector3(0.0f, 30.0f, 300.0f), Ogre::Node::TransformSpace::TS_WORLD);
	

	/* === Kinect: process skeleton streams and draw the cube according to player's position === */
	ProcessSkeleton();
	/* === End of Kinect: process skeleton streams and draw the cube according to player's position === */

//>>>	m_pBones->yaw(Ogre::Degree(evt.timeSinceLastFrame*100));

	// Move a cube between two knots
	if (mDirection == Ogre::Vector3::ZERO)
	{
		if (nextLocation())
		{
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

	return Ogre3DBase::frameRenderingQueued(evt);
}



HRESULT Ogre3D::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
//>>>        SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}


void Ogre3D::ProcessSkeleton()
{
    NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

    for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

        if (NUI_SKELETON_TRACKED == trackingState)
        {
            // We're tracking the skeleton, draw it
            DrawSkeleton(skeletonFrame.SkeletonData[i]);
        }
        else if (NUI_SKELETON_POSITION_ONLY == trackingState)
        {
			// Draw it anyway
			DrawSkeleton(skeletonFrame.SkeletonData[i]);
        }
    }
}


void Ogre3D::DrawSkeleton(const NUI_SKELETON_DATA & skel)
{      
    int i;

    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
		m_PointsOld[i] = m_Points[i];
        m_Points[i] = SkeletonToVector(skel.SkeletonPositions[i]);
    }

    // Draw the joints in a different color
	if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_INFERRED )
    {
		Ogre::Vector3 oldArm = m_PointsOld[NUI_SKELETON_POSITION_WRIST_LEFT] - m_PointsOld[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		Ogre::Vector3 newArm = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		
		oldArm.normalise();
		newArm.normalise();

		Ogre::Vector3 axis = oldArm.crossProduct(newArm);
		Ogre::Math math;
		Ogre::Radian angle = math.ACos(oldArm.dotProduct(newArm));

		m_pBones->rotate(axis, angle);
    }
	else if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_TRACKED )
    {
//>>>		m_pSkeletonNode->setPosition(m_Points[0]);
//>>>		m_pBones->setPosition(m_Points[0]);
		Ogre::Vector3 oldArm = m_PointsOld[NUI_SKELETON_POSITION_WRIST_LEFT] - m_PointsOld[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		Ogre::Vector3 newArm = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];

		oldArm.normalise();
		newArm.normalise();

		Ogre::Vector3 axis = oldArm.crossProduct(newArm);
		Ogre::Math math;
		Ogre::Radian angle = math.ACos(oldArm.dotProduct(newArm));

		m_pBones->rotate(axis, angle);
    }
}


Ogre::Vector3 Ogre3D::SkeletonToVector(Vector4 skeletonPoint)
{
    FLOAT x, y;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y);

	Ogre::Viewport* viewPort = mWindow->getViewport(0);
	x = (x * viewPort->getActualWidth()) / 320.0f;
	y = (y * viewPort->getActualHeight()) / 240.0f;

    Ogre::Vector3 position = Ogre::Vector3(x, y, 50.0f);

    return position;
}

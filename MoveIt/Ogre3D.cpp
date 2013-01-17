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
	m_Basis = Ogre::Vector3(0.0f, 0.0f, 1.0f);
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
	/* ========= Create KinectSensor instance for the first connected sensor ========== */
	CreateFirstConnected();


	/* ========= Set Light and Shadow Type ========== */
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);


	/* ========== Initialize skeleton points ========== */
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		m_Points[i] = Ogre::Vector3(0, 0, 0);
	}


	/* =========== Create skeleton entities and attach them to skeletonNode ========== */
	m_pPlayer = mSceneMgr->createEntity("Player", "ninja.mesh");
	m_pPlayer->setCastShadows(true);
	m_pPlayerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode", Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pPlayerNode->attachObject(m_pPlayer);
	m_pPlayerNode->setScale(0.5f, 0.5f, 0.5f);

	// Rotate the player model to face the camera
	Ogre::Quaternion q;
	Ogre::Math math;
	q.FromAngleAxis(Ogre::Radian(math.PI), Ogre::Vector3(0, 1, 0));
//>>>	m_pPlayerNode->rotate(q);


	/* =========== Get Ogre Bones by Joints ========== */
	m_pSkeleton = m_pPlayer->getSkeleton();
//>>>	m_pBones = m_pSkeleton->getRootBone();

	// Get Ogre Bones to match Kinect Joint
	m_pBones = m_pSkeleton->getBone("Joint10");	// ShoulderLeft
	m_BoneArray[0] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint11");	// ElbowLeft
	m_BoneArray[1] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint15");	// ShoulderRight
	m_BoneArray[2] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint16");	// ElbowRight
	m_BoneArray[3] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint18");	// HipLeft
	m_BoneArray[4] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint19");	// KneeLeft
	m_BoneArray[5] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint23");	// HipRight
	m_BoneArray[6] = m_pBones;
	m_pBones = m_pSkeleton->getBone("Joint24");	// KneeRight
	m_BoneArray[7] = m_pBones;

	for (int i = 0; i < ARR_SIZE; i++)
	{
		m_BoneArray[i]->setManuallyControlled(true);
	}


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
bool Ogre3D::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	/* === Kinect: process skeleton streams and draw the cube according to player's position === */
	//ProcessSkeleton();

	// Test Ogre Bone hierarchy
//>>>	m_pBones->yaw(Ogre::Degree(evt.timeSinceLastFrame*100));

	// Test Ogre mesh basis vector and Bone rotation with quaternion
	// Test results: m_Basis(0, 0, 1), ninja mesh looking into the screen
	Ogre::Math math;
	Ogre::Vector3 vec = Ogre::Vector3(0.0f, 1.0f, 0.0f);
	Ogre::Radian angle = math.ACos(vec.dotProduct(m_Basis));
	Ogre::Vector3 axis = vec.crossProduct(m_Basis);

	Ogre::Quaternion q;
	q.FromAngleAxis(angle, axis);
	m_BoneArray[0]->setOrientation(q);

	return Ogre3DBase::frameRenderingQueued(evt);
}

//-------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------
void Ogre3D::DrawSkeleton(const NUI_SKELETON_DATA & skel)
{      
    int i;

    for (i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
		m_PointsOld[i] = m_Points[i];
        m_Points[i] = SkeletonToVector(skel.SkeletonPositions[i]);
    }

	Ogre::Vector3* oldBones = new Ogre::Vector3[ARR_SIZE];
	Ogre::Vector3* newBones = new Ogre::Vector3[ARR_SIZE];

	Ogre::Quaternion q;

	Ogre::Vector3 axis;
	Ogre::Radian angle;
	Ogre::Math math;

    // Draw the joints in a different color
	if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_INFERRED )
    {
		// newBones[8]
		newBones[0] = m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		newBones[1] = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT];
		newBones[2] = m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
		newBones[3] = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT] - m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT];
		newBones[4] = m_Points[NUI_SKELETON_POSITION_KNEE_LEFT] - m_Points[NUI_SKELETON_POSITION_HIP_LEFT];
		newBones[5] = m_Points[NUI_SKELETON_POSITION_ANKLE_LEFT] - m_Points[NUI_SKELETON_POSITION_KNEE_LEFT];
		newBones[6] = m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT] - m_Points[NUI_SKELETON_POSITION_HIP_RIGHT];
		newBones[7] = m_Points[NUI_SKELETON_POSITION_ANKLE_RIGHT] - m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT];

		// 
		for(int i = 0; i < ARR_SIZE; i++)
		{
			newBones[i].normalise();

			axis = newBones[i].crossProduct(m_Basis);
			angle = math.ACos(newBones[i].dotProduct(m_Basis));
//			axis = m_Basis.crossProduct(newBones[i]);
//			angle = math.ACos(m_Basis.dotProduct(newBones[i]));

			q.FromAngleAxis(angle, axis);

			m_BoneArray[i]->setInheritOrientation(false);
			m_BoneArray[i]->setOrientation(q);
		}
    }
	else if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_TRACKED )
    {
//>>>		m_pPlayerNode->setPosition(Ogre::Vector3(0.0f, 0.0f, m_Points[0].z));

		// newBones[8]
		newBones[0] = m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];
		newBones[1] = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT];
		newBones[2] = m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
		newBones[3] = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT] - m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT];
		newBones[4] = m_Points[NUI_SKELETON_POSITION_KNEE_LEFT] - m_Points[NUI_SKELETON_POSITION_HIP_LEFT];
		newBones[5] = m_Points[NUI_SKELETON_POSITION_ANKLE_LEFT] - m_Points[NUI_SKELETON_POSITION_KNEE_LEFT];
		newBones[6] = m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT] - m_Points[NUI_SKELETON_POSITION_HIP_RIGHT];
		newBones[7] = m_Points[NUI_SKELETON_POSITION_ANKLE_RIGHT] - m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT];

		// 
		for(int i = 0; i < ARR_SIZE; i++)
		{
			newBones[i].normalise();

			axis = newBones[i].crossProduct(m_Basis);
			angle = math.ACos(newBones[i].dotProduct(m_Basis));
//			axis = m_Basis.crossProduct(newBones[i]);
//			angle = math.ACos(m_Basis.dotProduct(newBones[i]));

			q.FromAngleAxis(angle, axis);

			m_BoneArray[i]->setInheritOrientation(false);
			m_BoneArray[i]->setOrientation(q);
		}
    }

	delete [] oldBones;
	delete [] newBones;
}

//-------------------------------------------------------------------------------------
Ogre::Vector3 Ogre3D::SkeletonToVector(Vector4 skeletonPoint)
{
    FLOAT x, y, z;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y);

	Ogre::Viewport* viewPort = mWindow->getViewport(0);
	x = (x * viewPort->getActualWidth()) / 320.0f;
	y = (y * viewPort->getActualHeight()) / 240.0f;
	z = skeletonPoint.z;

    Ogre::Vector3 position = Ogre::Vector3(x, y, z);

    return position;
}

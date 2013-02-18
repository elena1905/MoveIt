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
Ogre3D::Ogre3D(void):
	m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
    m_pNuiSensor(NULL)
{
	TWOPI = Ogre::Radian(m_Math.TWO_PI);
	PI = Ogre::Radian(m_Math.PI);
	PIOVER2 = Ogre::Radian(m_Math.PI) / 2;

	m_Basis = Ogre::Vector3(0.0f, -1.0f, 0.0f);
	m_QuatPI.FromAngleAxis(PI, Ogre::Vector3(0, 1, 0));
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


	/* ========= Test xerces XML parser ========== */
	try
	{
		XMLPlatformUtils::Initialize();

		XMLCh str[100];
		XMLString::transcode("Core", str, 99);
		DOMImplementation* imp = DOMImplementationRegistry::getDOMImplementation(str);
		XMLString::transcode("root", str, 99);
		xercesc::DOMDocument *doc = imp->createDocument(0, str, 0);
		XMLString::transcode("node", str, 99);
		DOMElement *node = doc->createElement(str);
		node->setAttribute(XMLString::transcode("id"), XMLString::transcode("1"));
		node->setTextContent(XMLString::transcode("node1"));
		DOMElement *root = doc->getDocumentElement();
		root->appendChild(node);
		DOMLSOutput *output = ((DOMImplementationLS*)imp)->createLSOutput();
		DOMLSSerializer *serial = ((DOMImplementationLS*)imp)->createLSSerializer();
		XMLFormatTarget *target = new LocalFileFormatTarget("D:\\ProgramData\\VisualStudio2010\\FYP\\Tutorial\\Tutorial\\2.xml");
		output->setByteStream(target);
		serial->write(doc, output);

		doc->release();
		serial->release();
		delete target;

		XMLPlatformUtils::Terminate();
	}  
	catch (const XMLException& toCatch)
	{
		return ;
	}


	/* ========= Set Light and Shadow Type ========== */
	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// Set shadow type
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);


	/* =========== Create skeleton entities and attach them to skeletonNode ========== */
	m_pPlayer = mSceneMgr->createEntity("Player", "ninja1.mesh");
	m_pPlayer->setCastShadows(true);
	m_pPlayerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode", Ogre::Vector3(160.0f, 0.0f, -250.0f));
	m_pPlayerNode->attachObject(m_pPlayer);

	m_pPlayer2 = mSceneMgr->createEntity("Player2", "ninja.mesh");
	m_pPlayer2->setCastShadows(true);
	m_pPlayerNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode2", Ogre::Vector3(360.0f, 0.0f, -250.0f));
	m_pPlayerNode2->attachObject(m_pPlayer2);
	
	// Rotate the player model to face the camera
	m_pPlayerNode->rotate(m_QuatPI);
	m_pPlayerNode2->rotate(m_QuatPI);

	// Create manual object for drawing curved lines to show motions of both arm
	m_pLineL = mSceneMgr->createManualObject("manualLeft");
	m_pLineR = mSceneMgr->createManualObject("manualRight");

	m_pLineNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("LineNode", Ogre::Vector3(0.0f, 0.0f, 0.0f));
	m_pLineNode->attachObject(m_pLineL);
	m_pLineNode->attachObject(m_pLineR);
	

	/* =========== Get Ogre Bones by Joints ========== */
	m_pSkeleton = m_pPlayer->getSkeleton();
	m_pSkeleton2 = m_pPlayer2->getSkeleton();
	
	// Get Ogre Bones to match Kinect Joint
	m_BoneArray[0] = m_pSkeleton->getBone("Joint10");	// ShoulderLeft
	m_BoneArray[1] = m_pSkeleton->getBone("Joint11");	// ElbowLeft
	m_BoneArray[2] = m_pSkeleton->getBone("Joint15");	// ShoulderRight
	m_BoneArray[3] = m_pSkeleton->getBone("Joint16");	// ElbowRight
	m_BoneArray[4] = m_pSkeleton->getBone("Joint18");	// HipLeft
	m_BoneArray[5] = m_pSkeleton->getBone("Joint19");	// KneeLeft
	m_BoneArray[6] = m_pSkeleton->getBone("Joint23");	// HipRight
	m_BoneArray[7] = m_pSkeleton->getBone("Joint24");	// KneeRight

	m_BoneArray2[0] = m_pSkeleton2->getBone("Joint10");	// ShoulderLeft
	m_BoneArray2[1] = m_pSkeleton2->getBone("Joint11");	// ElbowLeft
	m_BoneArray2[2] = m_pSkeleton2->getBone("Joint15");	// ShoulderRight
	m_BoneArray2[3] = m_pSkeleton2->getBone("Joint16");	// ElbowRight
	m_BoneArray2[4] = m_pSkeleton2->getBone("Joint18");	// HipLeft
	m_BoneArray2[5] = m_pSkeleton2->getBone("Joint19");	// KneeLeft
	m_BoneArray2[6] = m_pSkeleton2->getBone("Joint23");	// HipRight
	m_BoneArray2[7] = m_pSkeleton2->getBone("Joint24");	// KneeRight

	// Enable bones to be manually controlled
	for (int i = 0; i < ARR_SIZE; i++)
	{
		m_BoneArray[i]->setManuallyControlled(true);
		m_BoneArray2[i]->setManuallyControlled(true);
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
	ProcessSkeleton();

	// Test Ogre Bone hierarchy
	/*
	m_BoneArray[1]->yaw(Ogre::Degree(evt.timeSinceLastFrame*100));
	*/

	// Test Ogre mesh basis vector and Bone rotation with quaternion
	// Test results: m_Basis(0, 0, -1), ninja mesh looking into the camera
	/*
	Ogre::Vector3 vec = Ogre::Vector3(-1.0f, 0.0f, 0.0f);
	Ogre::Radian angle = m_Math.ACos(vec.dotProduct(m_Basis));
	Ogre::Vector3 axis = vec.crossProduct(m_Basis);

	Ogre::Quaternion q;
	q.FromAngleAxis(angle, axis);
	q = m_QuatPI * q;
	m_BoneArray[0]->setOrientation(q);
	*/

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
            // We're tracking the skeleton, rotate the bones
            RotateBones(skeletonFrame.SkeletonData[i]);
        }
        else if (NUI_SKELETON_POSITION_ONLY == trackingState)
        {
			// Rotate the bones
			RotateBones(skeletonFrame.SkeletonData[i]);
        }
    }
}

//-------------------------------------------------------------------------------------
void Ogre3D::RotateBones(const NUI_SKELETON_DATA & skel)
{
	// Convert skeleton positions to vector3 format
    for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
        m_Points[i] = SkeletonToVector3(skel.SkeletonPositions[i]);
    }

	// Get the central point: HIP_CENTER
	Ogre::Real centerX, centerY, centerZ, lineXL, lineXR;
	centerX = m_Points[NUI_SKELETON_POSITION_HIP_CENTER].x;
//	centerY = m_Points[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
	lineXL = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT].x;
	lineXR = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT].x;

	// Map Kinect coordinate to Ogre coordinate
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
	{
		m_Points[i].x = 2 * centerX - m_Points[i].x;
		m_Points[i].y = 220.0f - m_Points[i].y;
//		m_Points[i].y = 2 * centerY - m_Points[i].y;
		m_Points[i].z = m_Points[i].z / 100;
	}

	// Set the model's position
	centerZ = -m_Points[NUI_SKELETON_POSITION_HIP_CENTER].z;
	m_pPlayerNode->setPosition(Ogre::Vector3(centerX, 0.0f, centerZ));
	m_pPlayerNode2->setPosition(Ogre::Vector3(centerX + 200.0f, 0.0f, centerZ));


	// Draw curved line for both arms
	Ogre::Vector3 linePosL = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT];
	Ogre::Vector3 linePosR = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT];
	linePosL.x = lineXL;
	linePosR.x = lineXR;
	linePosL.z = -linePosL.z;
	linePosR.z = -linePosR.z;

	m_PointQueL.push(linePosL);
	m_PointQueR.push(linePosR);
	if (m_PointQueL.size() > QUEUE_SIZE)
	{
		m_PointQueL.pop();
		m_PointQueR.pop();
	}

	m_pLineL->clear();
	m_pLineR->clear();

	m_pLineL->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
	m_pLineR->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
	std::queue<Ogre::Vector3> tempL, tempR;
	while (m_PointQueL.size())
	{
		m_pLineL->position(m_PointQueL.front());
		m_pLineL->colour(0.8f, 0.2f, 0.2f);
		tempL.push(m_PointQueL.front());
		m_PointQueL.pop();

		m_pLineR->position(m_PointQueR.front());
		tempR.push(m_PointQueR.front());
		m_PointQueR.pop();
	}
	m_pLineL->end();
	m_pLineR->end();

	while (tempL.size())
	{
		m_PointQueL.push(tempL.front());
		tempL.pop();

		m_PointQueR.push(tempR.front());
		tempR.pop();
	}



	// Calculate bone rotation vectors
	Ogre::Vector3* bonesRot = new Ogre::Vector3[ARR_SIZE];
	bonesRot[0] = m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	bonesRot[1] = m_Points[NUI_SKELETON_POSITION_WRIST_LEFT] - m_Points[NUI_SKELETON_POSITION_ELBOW_LEFT];
	bonesRot[2] = m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT] - m_Points[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
	bonesRot[3] = m_Points[NUI_SKELETON_POSITION_WRIST_RIGHT] - m_Points[NUI_SKELETON_POSITION_ELBOW_RIGHT];
	bonesRot[4] = m_Points[NUI_SKELETON_POSITION_KNEE_LEFT] - m_Points[NUI_SKELETON_POSITION_HIP_LEFT];
	bonesRot[5] = m_Points[NUI_SKELETON_POSITION_ANKLE_LEFT] - m_Points[NUI_SKELETON_POSITION_KNEE_LEFT];
	bonesRot[6] = m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT] - m_Points[NUI_SKELETON_POSITION_HIP_RIGHT];
	bonesRot[7] = m_Points[NUI_SKELETON_POSITION_ANKLE_RIGHT] - m_Points[NUI_SKELETON_POSITION_KNEE_RIGHT];

	Ogre::Quaternion q;

	Ogre::Vector3 axis;
	Ogre::Radian angle;

    // Rotate bones
	if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_INFERRED )
    {
		
    }
	else if ( skel.eSkeletonPositionTrackingState[0] == NUI_SKELETON_POSITION_TRACKED )
    {
		// 
		for(int i = 0; i < ARR_SIZE; i++)
		{
			bonesRot[i].normalise();

			axis = bonesRot[i].crossProduct(m_Basis);
			angle = -m_Math.ACos(bonesRot[i].dotProduct(m_Basis));
			
			q.FromAngleAxis(angle, axis);
			
			m_BoneArray[i]->setInheritOrientation(false);
			m_BoneArray[i]->setOrientation(q);

			m_BoneArray2[i]->setInheritOrientation(false);
			m_BoneArray2[i]->setOrientation(q);
		}
    }

	delete [] bonesRot;
}

//-------------------------------------------------------------------------------------
Ogre::Vector3 Ogre3D::SkeletonToVector3(Vector4 skeletonPoint)
{
    LONG x, y;
	USHORT depth;

    // Calculate the skeleton's position on the screen
    // NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
    NuiTransformSkeletonToDepthImage(skeletonPoint, &x, &y, &depth);

	float posX = static_cast<float>(x);
	float posY = static_cast<float>(y);
	float posZ = static_cast<float>(depth); // Unit of depth value is millimeter

	Ogre::Vector3 position = Ogre::Vector3(posX, posY, posZ);

    return position;
}

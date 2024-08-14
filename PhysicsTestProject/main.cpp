#include "pch.h"
#include "PxPhysicsAPI.h"
#include "pvd/PxPvd.h"
//#include "extensions/PxDefaultErrorCallback.h"
//#include "extensions/PxDefaultAllocator.h"
//#include "pvd/PxPvd.h"
//#include "pvd/PxPvdSceneClient.h"

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

static void createStack(physx::PxPhysics* mPhysics, physx::PxScene* mScene, physx::PxMaterial* mMaterial, const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent)
{
    physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
    for (physx::PxU32 i = 0; i < size; i++)
    {
        for (physx::PxU32 j = 0; j < size - i; j++)
        {
            physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
            physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
            body->attachShape(*shape);
            physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            mScene->addActor(*body);
        }
    }
    shape->release();
}

CLIENT_API void Setup()
{
    physx::PxDefaultAllocator mDefaultAllocatorCallback;
    physx::PxDefaultErrorCallback mDefaultErrorCallback;
    physx::PxTolerancesScale mToleranceScale;
    physx::PxFoundation* mFoundation = NULL;
    physx::PxPhysics* mPhysics = NULL;
    physx::PxScene* mScene = NULL;
    physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
    physx::PxMaterial* mMaterial = NULL;
    physx::PxPvd* mPvd = NULL;

    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (!mFoundation) Blaze::Debug::Logger::LogError("Client", "PxCreateFoundation failed!");

    mPvd = PxCreatePvd(*mFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    mToleranceScale.length = 100;
    mToleranceScale.speed = 981;
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);

    physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    mScene = mPhysics->createScene(sceneDesc);
    
    physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }



    mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 0), *mMaterial);
    mScene->addActor(*groundPlane);
    
    createStack(mPhysics, mScene, mMaterial, physx::PxTransform(physx::PxIdentity), 20, .5f);

    while (1)
    {        
        mScene->simulate(1.0f / 60.0f);
        mScene->fetchResults(true);
    }
    
    mScene->release();
    mDispatcher->release();
    mPhysics->release();
    mFoundation->release();
}
#include "module.h"

#include <string>
#include <GLM/glm.hpp>
#include <BULLET/btBulletDynamicsCommon.h>
using namespace std;
using namespace glm;

#include "transform.h"
#include "camera.h"
#include "person.h"


unsigned int ModuleCamera::Create(vec3 Position, float Height)
{
	unsigned int camera = Entity->New(), person = Entity->New();
	auto tsfcam = Entity->Add<StorageTransform>(camera);
	auto tsfpsn = Entity->Add<StorageTransform>(person);
	auto psn = Entity->Add<StoragePerson>(person);
	Entity->Add<StorageCamera>(camera)->Person = person;
	
	tsfcam->Body->setActivationState(DISABLE_DEACTIVATION);
	tsfcam->Position(Position);

	psn->Calculate(Height);

	btCollisionShape *shape = new btCapsuleShape(psn->Radius, psn->Radius /* plus twice radius tall */);
	btVector3 inertia;
	shape->calculateLocalInertia(psn->Mass, inertia);
	
	tsfpsn->Body = new btRigidBody(psn->Mass, new btDefaultMotionState(), shape, inertia);
	tsfpsn->Body->setAngularFactor(btVector3(0, 1, 0)); /* lock rotation around X and Z to prevent falling over */
	tsfpsn->Body->setActivationState(DISABLE_DEACTIVATION);
	tsfpsn->Position(Position);

	return camera;
}
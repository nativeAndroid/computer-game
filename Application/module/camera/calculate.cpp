#include "module.h"

#include <glm/glm.hpp>
#include <sfml/Graphics/RenderWindow.hpp>
using namespace glm;
using namespace sf;

#include "settings.h"
#include "camera.h"
#include "form.h"


void ModuleCamera::Projection()
{
	Projection(Global->Get<RenderWindow>("window")->getSize());
}

void ModuleCamera::Projection(Vector2u Size)
{
	auto stg = Global->Get<Settings>("settings");
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));

	cam->Projection = perspective(*stg->Get<float>("Fieldofview"), (float)Size.x / (float)Size.y, 0.1f, *stg->Get<float>("Viewdistance"));
}

void ModuleCamera::Calculate()
{
	uint64_t id = *Global->Get<uint64_t>("camera");
	auto frm = Entity->Get<Form>(id);
	auto cam = Entity->Get<Camera>(id);

	cam->View = lookAt(frm->Position(), frm->Position() + frm->Direction(), vec3(0, 1, 0));
}

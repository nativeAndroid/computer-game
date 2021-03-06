#include "module.h"

#include <string>
#include <filesystem>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <zip/zipint.h>

#include "type/settings/type.h"

using namespace std;
using namespace std::tr2::sys;
using namespace glm;

bool ModuleTerrain::Load(Terrain *Terrain)
{
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	return Archive->Read(path, file, Terrain->Blocks);
}

void ModuleTerrain::Save(Terrain *Terrain)
{
	auto stg = Global->Get<Settings>("settings");
	string path = "save/" + *stg->Get<string>("Savegame") + "/" + Name() + ".zip";
	string file = to_string(Terrain->Key.x) + "," + to_string(Terrain->Key.y) + "," + to_string(Terrain->Key.z) + ".txt";

	Archive->WriteAsync(path, file, Terrain->Blocks, sizeof Terrain->Blocks, [=](bool result){
		if(!result) Log->Fail("save chunk failed");
	});
}

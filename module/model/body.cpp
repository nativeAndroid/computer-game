#include "module.h"

#include <dependency/glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
using namespace glm;

#include "type/form/type.h"


btRigidBody *ModuleModel::CreateBody(string Path, vec3 Scale, float Mass)
{
	btCollisionShape *shape = GetShape(Path, Scale, (Mass == 0));
	btRigidBody *body = new btRigidBody(Mass, new btDefaultMotionState(), shape);

	if(Mass)
	{
		btVector3 inertia;
		shape->calculateLocalInertia(Mass, inertia);
		body->setMassProps(Mass, inertia);
	}
	else
	{
		body->setMassProps(0, btVector3(0, 0, 0));
	}

	return body;
}

btCollisionShape *ModuleModel::GetShape(string Path, vec3 Scale, bool Static)
{
	shape_key key = make_tuple(Path, Scale, Static);
	auto i = shapes.find(key);
	if(i != shapes.end()) return i->second.first;

	btCollisionShape *shape = NULL;
	LoadShape(shape, Path, Scale, Static);

	shapes.insert(make_pair(key, make_pair(shape, File->Hash("module/" + Name() + "/mesh/" + Path))));

	return shape;
}

void ModuleModel::ReloadShapes()
{
	auto fms = Entity->Get<Form>();

	for(auto i = shapes.begin(); i != shapes.end(); ++i)
	{
		int hash = File->Hash("module/" + Name() + "/mesh/" + get<0>(i->first));
		if(i->second.second != hash)
		{
			i->second.second = hash;
			btCollisionShape *previous = i->second.first;
			LoadShape(i->second.first, get<0>(i->first), get<1>(i->first), get<2>(i->first));
			Log->Pass("shape (" + get<0>(i->first) + ") reloaded");

			btCollisionShape *shape = GetShape(get<0>(i->first), get<1>(i->first), get<2>(i->first));
			for(auto j = fms.begin(); j != fms.end(); ++j)
				if(j->second->Body->getCollisionShape() == previous)
					j->second->Body->setCollisionShape(shape);
		}
	}
}

void ModuleModel::LoadShape(btCollisionShape *&Shape, string Path, vec3 Scale, bool Static)
{
	if(Path == "qube.prim")  return LoadShapeCube (Shape, Scale);
	if(Path == "plane.prim") return LoadShapePlane(Shape);

	if(Static)
	{
		btTriangleMesh *triangles = new btTriangleMesh();

		const aiScene *scene = aiImportFile(("module/" + Name() + "/mesh/" + Path).c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if(!scene)
		{
			Log->Fail("mesh (" + Path + ") cannot be loaded for collision");
			return;
		}

		for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh *mesh = scene->mMeshes[i];

			vector<vector<int>> faces;
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				vector<int> face;
				for(int j = 0; j < 3; ++j)
				{
					face.push_back(mesh->mFaces[i].mIndices[j]);
				}
				faces.push_back(face);
			}

			if(mesh->HasPositions())
			{
				for(auto face : faces)
				{
					vector<btVector3> triangle;
					for(auto index : face)
					{
						aiVector3D vertex = mesh->mVertices[index];
						triangle.push_back(btVector3(vertex.x, vertex.y, vertex.z));
					}
					triangles->addTriangle(triangle[0], triangle[1], triangle[2]);
				}
			}
		}

		Log->Pass("collision shape with " + to_string(triangles->getNumTriangles()) + " triangles");

		auto oldshape = Shape;
		Shape = new btBvhTriangleMeshShape(triangles, true, true);
		delete oldshape;
	}
	else
	{

	}

	Shape->setLocalScaling(btVector3(Scale.x, Scale.y, Scale.z));
}

void ModuleModel::LoadShapeCube(btCollisionShape *&Shape, vec3 Scale)
{
	auto oldshape = Shape;
	Shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	Shape->setLocalScaling(btVector3(Scale.x, Scale.y, Scale.z));
	delete oldshape;
}

void ModuleModel::LoadShapePlane(btCollisionShape *&Shape)
{
	auto oldshape = Shape;
	Shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	delete oldshape;
}

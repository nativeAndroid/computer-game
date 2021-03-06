#include "module.h"

#include <vector>
#include <unordered_map>
#include <glew/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;


ModuleModel::Mesh ModuleModel::GetMesh(string Path)
{
	auto i = meshes.find(Path);
	if(i != meshes.end()) return i->second.first;

	Mesh mesh;
	glGenBuffers(1, &mesh.Positions);
	glGenBuffers(1, &mesh.Normals);
	glGenBuffers(1, &mesh.Texcoords);
	glGenBuffers(1, &mesh.Elements);
	LoadMesh(mesh, Path);

	meshes.insert(make_pair(Path, make_pair(mesh, File->Hash("asset/" + Name() + "/mesh/" + Path))));
	return mesh;
}

void ModuleModel::ReloadMeshes()
{
	for (auto i = meshes.begin(); i != meshes.end(); ++i) {
		int hash = File->Hash("asset/" + Name() + "/mesh/" + i->first);
		if (i->second.second != hash) {
			i->second.second = hash;
			LoadMesh(i->second.first, i->first);
			Log->Pass("mesh (" + i->first + ") reloaded");
		}
	}
}

void ModuleModel::LoadMesh(Mesh &Mesh, string Path)
{
	if (Path == "cube.prim")  return LoadMeshCube(Mesh);
	if (Path == "plane.prim") return LoadMeshPlane(Mesh);

	const aiScene *scene = aiImportFile(("asset/" + Name() + "/mesh/" + Path).c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene) {
		Log->Fail("mesh (" + Path + ") cannot be loaded");
		return;
	}

	for (size_t i = 0; i < scene->mNumMeshes; ++i) {
		const aiMesh *mesh = scene->mMeshes[i];

		vector<GLuint> faces;
		for (size_t t = 0; t < mesh->mNumFaces; ++t) {
			faces.push_back(mesh->mFaces[t].mIndices[0]);
			faces.push_back(mesh->mFaces[t].mIndices[1]);
			faces.push_back(mesh->mFaces[t].mIndices[2]);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.Elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof GLuint, &faces[0], GL_STATIC_DRAW);

		if (mesh->HasPositions()) {
			glBindBuffer(GL_ARRAY_BUFFER, Mesh.Positions);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);

			// bounding box
			Mesh.Box = Model::Bounds();
			for (size_t i = 0; i < mesh->mNumVertices; ++i) {
				Mesh.Box.X.Min = min(Mesh.Box.X.Min, mesh->mVertices[i].x);
				Mesh.Box.X.Max = max(Mesh.Box.X.Max, mesh->mVertices[i].x);
				Mesh.Box.Y.Min = min(Mesh.Box.Y.Min, mesh->mVertices[i].y);
				Mesh.Box.Y.Max = max(Mesh.Box.Y.Max, mesh->mVertices[i].y);
				Mesh.Box.Z.Min = min(Mesh.Box.Z.Min, mesh->mVertices[i].z);
				Mesh.Box.Z.Max = max(Mesh.Box.Z.Max, mesh->mVertices[i].z);
			}
		}

		if (mesh->HasNormals()) {
			glBindBuffer(GL_ARRAY_BUFFER, Mesh.Normals);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
		}

		if (mesh->HasTextureCoords(0)) {
			vector<GLfloat> texcoords;
			for (size_t i = 0; i < mesh->mNumVertices; ++i) {
				texcoords.push_back(mesh->mTextureCoords[0][i].x);
				texcoords.push_back(mesh->mTextureCoords[0][i].y);
			}
			glBindBuffer(GL_ARRAY_BUFFER, Mesh.Texcoords);
			glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof GLfloat, &texcoords[0], GL_STATIC_DRAW);
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleModel::LoadMeshCube(Mesh &Mesh)
{
	const GLfloat positions[] = {-1.,-1.,1.,1.,-1.,1.,1.,1.,1.,-1.,1.,1.,-1.,1.,1.,1.,1.,1.,1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,-1.,-1.,-1.,1.,-1.,-1.,1.,-1.,1.,-1.,-1.,1.,-1.,-1.,-1.,-1.,-1.,1.,-1.,1.,1.,-1.,1.,-1.,1.,-1.,1.,1.,-1.,-1.,1.,1.,-1.,1.,1.,1.};
	const GLfloat normals[]   = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,-1,0,0,-1,0,0,-1,0,0,-1,0,0,1,0,0,1,0,0,1,0,0,1,0,0};
	const GLfloat texcoords[] = {0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.,0.,0.,1.,0.,1.,1.,0.,1.};
	const GLuint  elements[]  = {0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8,12,13,14,14,15,12,16,17,18,18,19,16,20,21,22,22,23,20};
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof positions, &positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof normals, &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof texcoords, &texcoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof elements, &elements[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	Mesh.Box.X.Min = -1;
	Mesh.Box.X.Max =  1;
	Mesh.Box.Y.Min = -1;
	Mesh.Box.Y.Max =  1;
	Mesh.Box.Z.Min = -1;
	Mesh.Box.Z.Max =  1;
}

void ModuleModel::LoadMeshPlane(Mesh &Mesh, float Size)
{
	float l = Size;
	const GLfloat positions[] = { -l,0,-l, -l,0,l, l,0,l, l,0,-l };
	const GLfloat normals[]   = { 0,1,0, 0,1,0., 0,1,0, 0,1,0 };
	const GLfloat texcoords[] = { 0,0, l/2,0, l/2,l/2, 0,l/2 };
	const GLuint  elements[]  = { 0,1,2, 2,3,0 };
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof positions, &positions[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof normals, &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.Texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof texcoords, &texcoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof elements, &elements[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	Mesh.Box.X.Min = -l;
	Mesh.Box.X.Max =  l;
	Mesh.Box.Y.Min =  0;
	Mesh.Box.Y.Max =  0;
	Mesh.Box.Z.Min = -l;
	Mesh.Box.Z.Max =  l;
}

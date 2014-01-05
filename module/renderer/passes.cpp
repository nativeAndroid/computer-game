#include "module.h"

#include <dependency/sfml/OpenGL.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
#include <dependency/glm/gtc/type_ptr.hpp>
using namespace std;
using namespace sf;
using namespace glm;

#include "type/camera/type.h"


void ModuleRenderer::PassCreate(string Name, string Vertex, string Fragment, unordered_map<GLenum, string> Targets, unordered_map<string, string> Samplers, unordered_map<string, string> Fallbacks, Function Function, float Size, GLenum StencilFunction, GLint StencilReference, GLenum StencilOperation)
{
	if(PassGet(Name, false))
	{
		Log->Fail("pass (" + Name + ") already exists");
		return;
	}

	Pass pass;

	for(auto i : Targets)
	{
		auto texture = TextureGet(i.second);
		if(get<2>(texture) != Size)
		{
			Log->Fail("size of pass (" + Name + ") and target (" + i.second + ") must match");
			continue;
		}
		GLuint id = get<0>(texture);
		if(id) pass.Targets.insert(make_pair(i.first, id));
	}

	for(auto i : Samplers)
	{
		GLuint id = get<0>(TextureGet(i.second));
		if(id) pass.Samplers.insert(make_pair(i.first, id));
	}

	for(auto i : Fallbacks)
	{
		GLuint from = get<0>(TextureGet(i.second)),
			   to   = get<0>(TextureGet(i.first));
		if(from && to) pass.Fallbacks.insert(make_pair(from, to));
	}

	switch(Function)
	{
	case FORMS:
		pass.Function = bind(&ModuleRenderer::DrawForms,  this, std::placeholders::_1);
		break;
	case SKY:
		pass.Function = bind(&ModuleRenderer::DrawSky,    this, std::placeholders::_1);
		break;
	case LIGHTS:
		pass.Function = bind(&ModuleRenderer::DrawLights, this, std::placeholders::_1);
		break;
	case QUAD:
		pass.Function = bind(&ModuleRenderer::DrawQuad,   this, std::placeholders::_1);
		break;
	case SCREEN:
		pass.Function = bind(&ModuleRenderer::DrawScreen, this, std::placeholders::_1);
		break;
	default:
		Log->Fail("pass (" + Name + ") invalid drawing function");
	}

	pass.Vertex           = Vertex;
	pass.Fragment         = Fragment;
	pass.Size             = Size;
	pass.Program          = CreateProgram(Vertex, Fragment);
	pass.Framebuffer      = Targets.size() ? CreateFramebuffer(pass.Targets) : 0;
	pass.StencilFunction  = StencilFunction;
	pass.StencilReference = StencilReference;
	pass.StencilOperation = StencilOperation;

	passes.push_back(make_pair(Name, pass));
}

ModuleRenderer::Pass *ModuleRenderer::PassGet(string Name, bool Output)
{
	for(auto i = passes.begin(); i != passes.end(); ++i)
		if(i->first == Name)
			return &i->second;

	if(Output) Log->Fail("cannot get pass because (" + Name + ") doesn't exist");
	return NULL;
}

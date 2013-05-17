#include "module.h"

#include <GLEW/glew.h>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;


GLuint ModuleRenderer::CreateFramebuffer(unordered_map<GLenum, pair<GLuint, GLenum>> Targets, unordered_map<string, GLuint> Samplers, float Size)
{
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	vector<GLenum> buffers;
	for(auto i : Targets)
	{
		TextureResize(i.second.first, i.second.second, Vector2u(Vector2f(size) * Size));
		glFramebufferTexture2D(GL_FRAMEBUFFER, i.first, GL_TEXTURE_2D, i.second.first, 0);
		if(i.first != GL_DEPTH_ATTACHMENT) buffers.push_back(i.first); // check for color attachment instead
	}
	glDrawBuffers(buffers.size(), &buffers[0]);

	Debug->PassFail("framebuffer setup", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return framebuffer;
}
	
void ModuleRenderer::TextureResize(GLuint Id, GLenum Type)
{
	TextureResize(Id, Type, Global->Get<RenderWindow>("window")->getSize());
}

void ModuleRenderer::TextureResize(GLuint Id, GLenum Type, Vector2u Size)
{
	auto format = TextureFormat(Type);
	glBindTexture(GL_TEXTURE_2D, Id);
	glTexImage2D(GL_TEXTURE_2D, 0, Type, Size.x, Size.y, 0, format.first, format.second, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

pair<GLenum, GLenum> ModuleRenderer::TextureFormat(GLenum InternalType)
{
	GLenum type;
	switch (InternalType)
	{
	case GL_RGB16:
	case GL_RGB16F:
	case GL_RGB32F:
		type = GL_RGB;
		break;
	case GL_RGBA16:
	case GL_RGBA16F:
	case GL_RGBA32F:
		type = GL_RGBA;
		break;
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
	case GL_DEPTH_COMPONENT32F:
		type = GL_DEPTH_COMPONENT;
	}

	GLenum format;
	switch (InternalType)
	{
	case GL_RGB16:
	case GL_RGBA16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
		format = GL_UNSIGNED_BYTE;
		break;
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_DEPTH_COMPONENT32F:
		format = GL_FLOAT;
		break;
	}

	return make_pair(type, format);
}

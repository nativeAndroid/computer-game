#pragma once

#include "system.h"

#include <unordered_map>
#include <GLEW/glew.h>


struct StorageFramebuffer
{
	StorageFramebuffer() : Id(0) {}
	GLuint Id;
	std::unordered_map<GLenum, std::pair<GLuint, GLenum>> Targets; // attachment point, texture id, internal type

	GLuint AddTarget(GLenum Attachment, GLenum Type)
	{
		GLuint id;
		glGenTextures(1, &id);
		Targets.insert(std::make_pair(Attachment, std::make_pair(id, Type)));
		return id;
	}
};
#include "Mesh.h"
#include "../../../Common/Define.h"

Mesh::Mesh(const char* fileName)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");

	mObjName = fileName;
}

Mesh::~Mesh()
{
}
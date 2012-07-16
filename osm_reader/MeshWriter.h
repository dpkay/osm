#pragma once

#define _USE_MATH_DEFINES
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

class MeshWriter
{
public:
	MeshWriter(void);
	~MeshWriter(void);

	int write();

private:
	typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;

};


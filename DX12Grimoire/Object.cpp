#include "Object.h"
#include "ObjectManager.h"

CObject::CObject()
{
	ObjectID = OBJ_ID_DEFAULT;
	Vertices = nullptr;
	Indices = nullptr;
}

void CObject::SetVertices(vector<Vertex>* vert)
{
	Vertices = vert;
}

void CObject::SetIndices(vector<Index>* ind)
{
	Indices = ind;
}

void CObject::Destroy()
{
	SObjectManager::ObjectManager->DeleteObject(this);
}

void CObject::Destroy(CObject** p)
{
	Destroy();
	*p = nullptr;
}

void CObject::Tick(float DeltaTime)
{
}

void CObject::Draw()
{
}

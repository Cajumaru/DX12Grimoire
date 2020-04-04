#pragma once
#include "CoreMinimal.h"

#define OBJ_ID_DEFAULT 18446744073709551615

class CObject
{
public:
	CObject();

	void SetVertices(vector<Vertex>* vert);
	void SetIndices(vector<Index>* ind);

	vector<Vertex>* const GetVertices() { return Vertices; }
	vector<Index>* const GetIndices() { return Indices; }

	void Destroy();
	void Destroy(CObject** p);

	void SetObjectID(size_t ID) { ObjectID = ID; }
	size_t const GetObjectID() { return ObjectID; }

public:
	virtual void Tick(float DeltaTime);

	virtual void Draw();

protected:
	vector<Vertex>* Vertices;
	vector<Index>* Indices;

	size_t ObjectID;
};
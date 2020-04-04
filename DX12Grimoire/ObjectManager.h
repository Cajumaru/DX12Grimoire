#pragma once

#include "Object.h"

using namespace std;

class SObjectManager
{
public:
	SObjectManager(size_t InitSize);

	static SObjectManager* ObjectManager;

	template<class T, typename... Args>
	T* CreateObject(Args... args);

	void DeleteObject(CObject* Obj);

	void ClearAll();

private:

	vector<CObject*> ObjectList;
	vector<size_t> ListQueue;

	size_t MaxObjectNum;
};

template<class T, typename... Args>
T* SObjectManager::CreateObject(Args... args)
{
	size_t ID;
	if (ListQueue.size() != 0)
	{
		ID = ListQueue.back();
		ListQueue.pop_back();
	}
	else
	{
		ListQueue.push_back(MaxObjectNum);
		ID = MaxObjectNum;
		++MaxObjectNum;
	}

	T* Obj = new T(args...);
	Obj->SetObjectID(ID);
	ObjectList.push_back(Obj);

	return Obj;
}

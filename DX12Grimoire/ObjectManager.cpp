#include "ObjectManager.h"

SObjectManager* SObjectManager::ObjectManager;

SObjectManager::SObjectManager(size_t InitSize)
{
	ObjectManager = this;
	ObjectList.reserve(InitSize);
	MaxObjectNum = InitSize;
	for (size_t ind = 0; ind < InitSize; ++ind)
	{
		ListQueue.push_back(InitSize - ind - 1);
	}
	ObjectManager = this;
}

void SObjectManager::DeleteObject(CObject* Obj)
{
	size_t ID = Obj->GetObjectID();
	ListQueue.push_back(ID);
	ObjectList.at(ID) = nullptr;

	delete Obj;
}

void SObjectManager::ClearAll()
{
	for (CObject* Obj : ObjectList)
	{
		if (Obj != nullptr)
		{
			DeleteObject(Obj);
		}
	}
}

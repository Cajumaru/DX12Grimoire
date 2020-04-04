#pragma once
#include "CoreObject.h"

class CObjectSimpleDrawer : public CObject
{
public:
	CObjectSimpleDrawer(int shownum);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Draw() override;

protected:

private:
	TextureData Tex = TextureNatsu;
	
};

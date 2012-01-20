#pragma once

#include "Singleton.h"
#include <Cg/cg.h>
#include <map>
#include "Effect.h"

class CGManager : public Singleton<CGManager>
{
public:
	CGManager():m_Initialized(false){};
	bool Initialize();

	Effect* LoadEffect(const std::string& aFilename, const std::string& aName);

	static void ErrorCallback( CGcontext context, CGerror error, void* appdata );
private:
	bool m_Initialized;

	//CG
	CGcontext m_CGContext;

	typedef std::map<std::string, Effect*> EffectMap;
	EffectMap m_EffectMap;
};
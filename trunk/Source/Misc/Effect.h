#pragma once

#include <Cg/cg.h>
#include <string>
#include <map>

class Effect
{
public:
	Effect( CGcontext aContext, const std::string& aFilename, const std::string& aName);

	CGparameter GetParameter(const std::string& aName);
	CGeffect GetEffect(){return m_CGEffect;}

private:
	CGcontext m_CGContext;
	CGeffect m_CGEffect;
	std::string m_Name;
};
#include "Effect.h"

Effect::Effect( CGcontext aContext, const std::string& aFilename, const std::string& aName)
{
	m_CGContext = aContext;
	m_Name = aName;
	m_CGEffect = cgCreateEffectFromFile( aContext, aFilename.c_str(), NULL );
}

CGparameter Effect::GetParameter(const std::string& aName)
{
	return cgGetNamedEffectParameter(m_CGEffect, aName.c_str());
}
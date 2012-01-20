#include "CGManager.h"

#include <Cg/cgGL.h>

bool CGManager::Initialize()
{
	if( m_Initialized) return true;

	 cgSetErrorHandler( CGManager::ErrorCallback, this );

	 m_CGContext = cgCreateContext();

	 cgGLRegisterStates( m_CGContext );
	 cgGLSetManageTextureParameters( m_CGContext, CG_TRUE );

	 printf("> CG Initialized\n");

	 m_Initialized = true;
	 return true;
}

void CGManager::ErrorCallback( CGcontext context, CGerror error, void* appData )
{
	if ( error != CG_NO_ERROR )
	{
		const char* strError = cgGetErrorString(error);
		printf("> CG Error: [%s]\n",( strError == 0 ) ? "" : strError );

		if (error == CG_COMPILER_ERROR) {
			printf("%s\n", cgGetLastListing(context));
		}
	}
}

Effect* CGManager::LoadEffect(const std::string& aFilename, const std::string& aName)
{
	Effect* aEffect = new Effect(m_CGContext, aFilename, aName);
	m_EffectMap[aName] = aEffect;

	printf("> Loaded effect: %s\n", aFilename.c_str());

	return aEffect;
}
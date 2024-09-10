#include "pch.h"
#include "resource/Material.h"
#include "resource/Shader.h"

bool Material::operator<(const Material& rhs) const
{
	return mShader->mShaderID < rhs.mShader->mShaderID;
}

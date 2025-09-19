

#include <iostream>
#include "Bindable/VertexShader.h"

namespace Bind
{
	VertexShader::VertexShader(ID3D11Device* device, LPCWSTR filePath)
	{
		ID3DBlob* errorVertexCode = nullptr;
		HRESULT hr = D3DCompileFromFile(
			filePath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"VSMain",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pShaderBytecodeBlob,
			&errorVertexCode);

		if (FAILED(hr)) {
			// If the shader failed to compile it should have written something to the error message.
			if (errorVertexCode) {
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

				std::cout << compileErrors << std::endl;
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				std::wcout << filePath << L" - Missing Shader File\n";
			}	

		}

		device->CreateVertexShader(
			pShaderBytecodeBlob->GetBufferPointer(),
			pShaderBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		);
	}

	void VertexShader::Bind(ID3D11DeviceContext* context) noexcept
	{
		context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return pShaderBytecodeBlob.Get();
	}
}
#include "Graphics/GraphicsResources/GeometryShader.h"
#include <iostream>


namespace SE_G {
	Bind::GeometryShader::GeometryShader(ID3D11Device* device, LPCWSTR filePath)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
		ID3DBlob* errorGeometryCode;
		HRESULT hr = D3DCompileFromFile(
			filePath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"GSMain",
			"gs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pShaderBytecodeBlob,
			&errorGeometryCode);

		if (FAILED(hr)) {
			// If the shader failed to compile it should have written something to the error message.
			if (errorGeometryCode) {
				char* compileErrors = (char*)(errorGeometryCode->GetBufferPointer());

				std::cout << compileErrors << " - // -- " << std::endl;
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				//std::cout << filePath << L" - Missing Shader File\n";
			}

			return;
		}
		if (FAILED(hr)) return;

		hr = device->CreateGeometryShader(
			pShaderBytecodeBlob->GetBufferPointer(),
			pShaderBytecodeBlob->GetBufferSize(),
			nullptr,
			&pGeometryShader);
		pShaderBytecodeBlob->Release();
	}

	Bind::GeometryShader::~GeometryShader()
	{
	}

	void Bind::GeometryShader::Bind(ID3D11DeviceContext* context) noexcept
	{
		context->GSSetShader(pGeometryShader.Get(), nullptr, 0u);
	}
}
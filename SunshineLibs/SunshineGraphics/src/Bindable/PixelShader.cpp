

#include "Bindable/PixelShader.h"
#include <iostream>

Bind::PixelShader::PixelShader(ID3D11Device* device, LPCWSTR filePath)
{
	Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
	ID3DBlob* errorPixelCode;
	HRESULT hr = D3DCompileFromFile(
		filePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pShaderBytecodeBlob,
		&errorPixelCode);

	if (FAILED(hr)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());

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

	hr = device->CreatePixelShader(
		pShaderBytecodeBlob->GetBufferPointer(),
		pShaderBytecodeBlob->GetBufferSize(),
		nullptr,
		&pPixelShader);
	pShaderBytecodeBlob->Release();
}

void Bind::PixelShader::Bind(ID3D11DeviceContext* context) noexcept
{
	context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

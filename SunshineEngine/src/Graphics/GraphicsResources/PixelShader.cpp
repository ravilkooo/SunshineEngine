#include "Graphics/GraphicsResources/PixelShader.h"
#include <d3dcompiler.h>
#include <iostream>


namespace SE_G {
	namespace Bind {
		PixelShader::PixelShader(ID3D11Device* device, LPCWSTR filePath) {
			ChangeShader(device, filePath);
		}

		PixelShader::~PixelShader()
		{
			Release();
		}

		void PixelShader::Release() {
			pPixelShader.Reset();
		}

		void PixelShader::ChangeShader(ID3D11Device* device, eastl::wstring filePath) {
			ChangeShader(device, filePath.c_str());
		}

		void PixelShader::ChangeShader(ID3D11Device* device, LPCWSTR filePath) {
			Release();

			Microsoft::WRL::ComPtr<ID3DBlob> pShaderBytecodeBlob;
			Microsoft::WRL::ComPtr<ID3DBlob> errorPixelCode;
			HRESULT hr = D3DCompileFromFile(
				filePath,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"PSMain",
				"ps_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				pShaderBytecodeBlob.GetAddressOf(),
				errorPixelCode.GetAddressOf());

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
				pShaderBytecodeBlob.Reset();
				errorPixelCode.Reset();
				Release();
				return;
			}

			hr = device->CreatePixelShader(
				pShaderBytecodeBlob->GetBufferPointer(),
				pShaderBytecodeBlob->GetBufferSize(),
				nullptr,
				&pPixelShader);
			// Reset the ComPtr instead of manually releasing the underlying pointer
			pShaderBytecodeBlob.Reset();
			errorPixelCode.Reset();
		}

		void PixelShader::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
		}
	}
}
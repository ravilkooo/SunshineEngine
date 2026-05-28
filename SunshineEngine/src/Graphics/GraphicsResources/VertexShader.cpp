#include <iostream>
#include "Graphics/GraphicsResources/VertexShader.h"
#include <d3dcompiler.h>

namespace SE_G {
	namespace Bind
	{
		VertexShader::VertexShader(ID3D11Device* device, AssetPath assetPath)
			: VertexShader(
				device, assetPath.GetFullPath(),
				assetPath.m_params.asShader.numInputElements,
				assetPath.m_params.asShader.IALayoutInputElements)
		{
		}

		VertexShader::VertexShader(ID3D11Device* device, LPCWSTR filePath)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> errorVertexCode;
			HRESULT hr = D3DCompileFromFile(
				filePath,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VSMain",
				"vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				pShaderBytecodeBlob.GetAddressOf(),
				errorVertexCode.GetAddressOf());

			if (FAILED(hr)) {
				// If the shader failed to compile it should have written something to the error message.
				if (errorVertexCode) {
					char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
					std::cout << compileErrors << std::endl;
				}
				// If there was nothing in the error message then it simply could not find the shader file itself.
				else
				{
					std::wcout << filePath << L" - Missing Shader File\n";
				}
				// Compilation failed; avoid using null bytecode blob
				errorVertexCode.Reset();
				Release();
				return;
			}

			device->CreateVertexShader(
				pShaderBytecodeBlob->GetBufferPointer(),
				pShaderBytecodeBlob->GetBufferSize(),
				nullptr,
				pVertexShader.GetAddressOf()
			);

			UINT numInputElements;
			D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr;
			FillStandartInputLayout(numInputElements, IALayoutInputElements);

			device->CreateInputLayout(
				IALayoutInputElements,
				numInputElements,
				pShaderBytecodeBlob->GetBufferPointer(),
				pShaderBytecodeBlob->GetBufferSize(),
				pInputLayout.GetAddressOf());

			// free(IALayoutInputElements);
			delete[] IALayoutInputElements;
			errorVertexCode.Reset();
		}

		VertexShader::VertexShader(ID3D11Device* device, LPCWSTR filePath,
			UINT numInputElements,
			D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements) {

			Microsoft::WRL::ComPtr<ID3DBlob> errorVertexCode;
			HRESULT hr = D3DCompileFromFile(
				filePath,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VSMain",
				"vs_5_0",
				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
				0,
				pShaderBytecodeBlob.GetAddressOf(),
				errorVertexCode.GetAddressOf());

			if (FAILED(hr)) {
				// If the shader failed to compile it should have written something to the error message.
				if (errorVertexCode) {
					char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
					std::cout << compileErrors << std::endl;
					errorVertexCode->Release();
				}
				// If there was  nothing in the error message then it simply could not find the shader file itself.
				else
				{
					std::wcout << filePath << L" - Missing Shader File\n";
				}
				errorVertexCode.Reset();
				Release();
				return;
			}

			device->CreateVertexShader(
				pShaderBytecodeBlob->GetBufferPointer(),
				pShaderBytecodeBlob->GetBufferSize(),
				nullptr,
				pVertexShader.GetAddressOf()
			);

			device->CreateInputLayout(
				IALayoutInputElements,
				numInputElements,
				pShaderBytecodeBlob->GetBufferPointer(),
				pShaderBytecodeBlob->GetBufferSize(),
				pInputLayout.GetAddressOf());

			errorVertexCode.Reset();
		}

		VertexShader::~VertexShader()
		{
			Release();
		}

		void VertexShader::Release() {
			pShaderBytecodeBlob.Reset();
			pVertexShader.Reset();
			pInputLayout.Reset();
		}

		void VertexShader::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->IASetInputLayout(pInputLayout.Get());
			context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
		}

		ID3DBlob* VertexShader::GetBytecode() const noexcept
		{
			return pShaderBytecodeBlob.Get();
		}
		SunshineResource::ResourceType VertexShader::GetType() const
		{
			return SunshineResource::ResourceType::SHADER;
		}
		ResourceGUID VertexShader::GetGUID() const
		{
			return m_GUID;
		}
		size_t VertexShader::GetSizeInMemory() const
		{
			return m_MemorySize;
		}

		void VertexShader::FillStandartInputLayout(UINT& numInputElements, D3D11_INPUT_ELEMENT_DESC* &IALayoutInputElements)
		{
			numInputElements = 4;
			IALayoutInputElements = new D3D11_INPUT_ELEMENT_DESC[numInputElements];

			IALayoutInputElements[0] =
				D3D11_INPUT_ELEMENT_DESC{
					"POSITION",
					0,
					DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					0,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 };
			IALayoutInputElements[1] =
				D3D11_INPUT_ELEMENT_DESC{
					"COLOR",
					0,
					DXGI_FORMAT_R32G32B32A32_FLOAT,
					0,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 };
			IALayoutInputElements[2] =
				D3D11_INPUT_ELEMENT_DESC{
					"TEXCOORD",
					0,
					DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
					0,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 };
			IALayoutInputElements[3] =
				D3D11_INPUT_ELEMENT_DESC{
					"NORMAL",
					0,
					DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
					0,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA,
					0 };
		}
	}
}
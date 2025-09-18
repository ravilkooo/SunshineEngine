

#include "Drawable.h"
#include "Bindable/Bindable.h"

using namespace Bind;

void Drawable::DrawTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept {
	context->DrawIndexed(indicesNum, 0, 0);
}
bool Drawable::HasTechnique(std::string technique)
{
	return techniques.find(technique) != techniques.end();
}
void Drawable::PassTechnique(std::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	techniques[technique]->BindAll(context);
	DrawTechnique(technique, context);
}
;

DirectX::XMMATRIX Drawable::GetViewMatrix() const
{
	return camera->GetViewMatrix();
}

DirectX::XMMATRIX Drawable::GetProjectionMatrix() const
{
	return camera->GetProjectionMatrix();
}

DirectX::XMFLOAT3 Drawable::GetCameraPosition() const
{
	return camera->GetPosition();
}

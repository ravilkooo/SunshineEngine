#pragma once
#include <Engine/Game.h>
#include <RenderingSystem/DeferredRenderer.h>
#include <RenderingSystem/GBufferPass.h>
#include <RenderingSystem/LightPass.h>
#include <RenderingSystem/MainColorPass.h>

#include <LightObjects/LightCollection.h>
#include <GraphicsUtils/FullScreenQuad.h>

#include "TestCube.h"

#define DEBUG_LIGHT_OBJECTS

class DeferredGame :
    public Game
{
public:
    DeferredGame();
    ~DeferredGame();

    void Update(float deltaTime) override;
    void Render() override;

    void HandleKeyDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
    SpotLight* _sl_1;
    float currTime = 0.0f;
    DirectionalLight* _dl_1;

    // Particle test
    LightPass* gLightPass;
};

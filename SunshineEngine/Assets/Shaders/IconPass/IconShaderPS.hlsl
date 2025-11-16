// Texture sampler
SamplerState gSampler : register(s0);

// Diffuse texture
Texture2D gTextureSprite : register(t0);

struct SpritesheetInfoPCB
{
    uint width; //  = 1024u;
    uint height; //= 1024u;
    uint uStep; // = 128u;
    uint vStep; // = 128u;
    uint uSteps; // = 8u;
    uint vSteps; // = 8u;
};

struct IconDataPCB
{
    uint uIdx; // = 0u;
    uint vIdx; //  = 0u;
    uint uStepSize; //  = 1u;
    uint vStepSize; //  = 1u;
    
    uint hi;
    uint lo;
};

cbuffer SpritesheetInfoBuffer : register(b0) // per frame
{
    SpritesheetInfoPCB spritesheetInfo;
};

cbuffer IconDataBuffer : register(b1) // per object
{
    IconDataPCB iconData;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texture_coord : TEXTURE;
};

struct PSOutput
{
    float4 Color : SV_Target0;
    uint2 UUID : SV_Target1;
};

PSOutput PSMain(PS_IN input) : SV_Target
{
    // sprite u start pixel pos
    uint2 pixelStartPos = uint2(
        iconData.uIdx * spritesheetInfo.uStep,
        iconData.vIdx * spritesheetInfo.vStep
    );
    
    uint2 pixelEndPos = uint2(
        (iconData.uIdx + iconData.uStepSize) * spritesheetInfo.uStep,
        (iconData.vIdx + iconData.vStepSize) * spritesheetInfo.vStep
    );
    
    float2 uvStartPos =
        float2(pixelStartPos) /
        float2(
            spritesheetInfo.width,
            spritesheetInfo.height
        );
    
    float2 uvEndPos =
        float2(pixelEndPos) /
        float2(
            spritesheetInfo.width,
            spritesheetInfo.height
        );
    
    float4 sprite_texture = gTextureSprite.Sample(gSampler,
        uvStartPos + input.texture_coord * (uvEndPos - uvStartPos)
    );
    
    PSOutput output;
    output.Color = sprite_texture;
    output.UUID = uint2(iconData.hi, iconData.lo);
    return output;
}
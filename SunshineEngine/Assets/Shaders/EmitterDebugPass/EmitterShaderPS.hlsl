struct PS_IN
{
    float4 pos : SV_POSITION;
};

struct PSOutput
{
    float4 Color : SV_Target0;
};

PSOutput PSMain(PS_IN input)
{
    PSOutput output;
    // PANTONE Warm Red
    // HEX color #F9423A
    output.Color = float4(0.870588f, 0.352941f, 0.290196f, 1.0f);
    return output;
}
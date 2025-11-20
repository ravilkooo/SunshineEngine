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
    // HEX color #0859C6
    output.Color = float4(0.03137f, 0.34902f, 0.77647f, 1.0f);
    return output;
}
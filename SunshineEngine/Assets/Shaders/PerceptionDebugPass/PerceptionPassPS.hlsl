struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

struct PSOutput
{
    float4 Color : SV_Target0;
};

PSOutput PSMain(PS_IN input)
{
    PSOutput output;
    output.Color = input.col;
    return output;
}
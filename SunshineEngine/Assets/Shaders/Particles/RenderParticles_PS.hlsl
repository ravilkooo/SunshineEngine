struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float3 center : TEXCOORD2;
    float radius : TEXCOORD3;
    float2 uv : TEXCOORD4;
    //float4 Normal : TEXCOORD2;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

float sdLine(float2 p, float2 a, float2 b)
{
    float2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

SamplerState textureClampSampler : register(s0);
Texture2D diffuseTexture1 : register(t0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    /*float3 normal = normalize(input.Normal.xyz);

    float3 ambient = sunColor.xyz * sceneAmbientPower;

    float NdotL = max(dot(normal, sunDirection.xyz), 0.0);
    float3 diffuse = NdotL * sunColor.xyz * sunColor.w;

    float3 viewDirection = normalize(camPosition.xyz - input.oPosition);
    float3 h = normalize(sunDirection.xyz + viewDirection);

    float NdotH = max(dot(normal, h), 0.0);
    float specIntensity = pow(saturate(NdotH), 32.0);
    float3 specular = specIntensity * sunSpecColor.xyz * sunSpecColor.w;

    output.color.xyz = ambient + diffuse + specular;// * input.Color.xyz;

    //output.color = 0.5 + normalize(input.Normal) * 0.5; //input.Color;
    //output.color = normalize(input.Normal); //input.Color;
    //output.color.a = 0.8;
    output.color.a = 1.0;//input.Color.a;
    */

    //float4 diffuseColor = float(1).xxxx;
    //output.color = input.Color * diffuseColor;
    float4 diffuseColor = diffuseTexture1.Sample(textureClampSampler, input.uv);
    output.color = input.Color * diffuseColor;
    // output.color.a *= 0.8;


    //float len = length(input.oPosition - input.center);
    //output.color.a = output.color.a * (1.0 - smoothstep(0.0, input.radius, len));

    /*float thickness = 0.115;
    float blurness = 5.0;
    float d = sdLine(input.uv, float2(0.2, 0.5), float2(0.8, 0.5));
    float g = fwidth(d) * blurness;
    float a = 1.0 * (1.0 - smoothstep(thickness - g, thickness + g, d));
    output.color.a = output.color.a * a;*/

    return output;

}
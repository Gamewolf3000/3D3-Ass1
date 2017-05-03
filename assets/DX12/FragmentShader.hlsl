Texture2D sampleTexture : register(t0);
sampler samplerState : register(s0);

struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: UV;
};

cbuffer DiffuseAndShouldTextureBeUsed : register(b0)
{
	float4 diffuseValueAndUseOnW;
};

float4 main(VSOut input) : SV_TARGET0
{
	float4 colour;
	
	if (diffuseValueAndUseOnW.w == 1.0f)
		colour = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else
		colour = float4(sampleTexture.Sample(samplerState, input.uv).xyz, 1.0f);


	return colour*float4(diffuseValueAndUseOnW.xyz,1.0f);
}
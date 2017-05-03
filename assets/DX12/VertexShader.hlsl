struct VSIn
{
	float4 pos[3] : POSITION;
	float4 normal[3] : NORMAL;
	float2 uv[3] : UV;
};

struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: UV;
};

cbuffer Translation : register(b0)
{
	float4 translate;
}

VSOut main(VSIn input, uint index : SV_VertexID)
{
	VSOut output = (VSOut)0;
	//float3 position;
	//switch (index)
	//{
	//case 0:
	//	position = float3(0.0f, 0.5f, 0.0f);
	//	break;

	//case 1:
	//	position = float3(0.5f, -0.5f, 0.0f);
	//	break;

	//case 2:
	//	position = float3(-0.5f, -0.5f, 0.0f);
	//	break;
	//}



	output.pos = float4(input.pos[index].xyz +translate.xyz /*+ translate.xyz*/, 1.0f);
	output.color = float4(0.0f, 1.0f, 0.0f,1.0f);//float4(input.normal[index].xyz, 1.0f);
	output.uv = input.uv[index];

	return output;
}
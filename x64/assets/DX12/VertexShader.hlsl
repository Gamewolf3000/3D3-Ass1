struct VSOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};

//cbuffer CB : register(b0)
//{
//	float R, G, B, A;
//}

VSOut main(uint index : SV_VertexID)
{
	VSOut output = (VSOut)0;
	float3 position;
	switch (index)
	{
	case 1:
		position = float3(-1.0f, -1.0f, 0.0f);
			break;

	case 2:
		position = float3(1.0f, -1.0f, 0.0f);
		break;

	case 0:
		position = float3(1.0f, 1.0f, 0.0f);
		break;
	}

	output.pos = float4(position, 1.0f);
	output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}
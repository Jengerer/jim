sampler2D g_samSrColor;

float4 TintShader( float2 Tex : TEXCOORD0 ) : COLOR0
{
	float4 Color = tex2D( g_samSrcColor, Tex.xy );
	return Color;
}

technique PostProcess
{
	pass p1
	{
		VertexShader = null;
		PixelShader = compile ps_2_0 TintShader();
	}
}
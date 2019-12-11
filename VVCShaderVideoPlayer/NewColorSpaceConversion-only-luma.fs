#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureY;
//uniform sampler2D textureU;
//uniform sampler2D textureV;
uniform float tex_format = 0;   //tex_format value is 0 for YUV420p , tex_format value is 1 for YUV420p
uniform float alpha = 1.0;

void main()
{
	vec3 yuv;
	vec4 rgba;

	if(tex_format == 0) //yuv420p
	{
		yuv.r=texture(textureY, TexCoord).r;
		//yuv.g=texture(textureU, TexCoord).r;
		//yuv.b=texture(textureV, TexCoord).r;

		yuv.r=1.1643*(yuv.r-0.0625);
		//yuv.g=yuv.g-0.5;
		//yuv.b=yuv.b-0.5;
	}
	

	if(tex_format == 0 || tex_format == 1)
	{
		//rgba.r=yuv.r+1.5958*yuv.b ;
		//rgba.g=yuv.r-0.39173*yuv.g-0.81290*yuv.b;
		//rgba.b=yuv.r+2.017*yuv.g;

	    rgba.r=yuv.r;
		rgba.g=yuv.r;
		rgba.b=yuv.r;
	}

	rgba.a = alpha;
	FragColor = rgba;
}

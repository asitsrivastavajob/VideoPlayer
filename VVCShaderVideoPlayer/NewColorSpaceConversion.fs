#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureY;
uniform sampler2D textureU;
uniform sampler2D textureV;
uniform float tex_format = 1;   //tex_format value is 0 for YUV420p , tex_format value is 1 for YUV420p
uniform float alpha = 1.0;


void main()
{
	vec3 yuv;
	vec3 yuv2;
	vec4 rgba;

	if(tex_format == 0) //yuv420p
	{
		yuv.r=texture(textureY, TexCoord).r;  //Y
		yuv.g=texture(textureU, TexCoord).r;  //U
		yuv.b=texture(textureV, TexCoord).r;  //V

		yuv.r=1.1643*(yuv.r-0.0625);   //Y
		yuv.g=yuv.g-0.5;			   //U
		yuv.b=yuv.b-0.5;               //V

		rgba.r=yuv.r+1.5958*yuv.b ;					//R
		rgba.g=yuv.r-0.39173*yuv.g-0.81290*yuv.b;   //G
		rgba.b=yuv.r+2.017*yuv.g;	
	}

	if(tex_format == 1) //yuv420p10ble
	{
		vec3 rgb;    

		yuv.r = texture2D(textureY, TexCoord).r*64.0;
		yuv.g = texture2D(textureU, TexCoord).r*64.0 - 0.5;
		yuv.b = texture2D(textureV, TexCoord).r*64.0 - 0.5;   

		yuv.r=1.1643*(yuv.r-0.0625);   //Y
		yuv.g=yuv.g-0.5;			   //U
		yuv.b=yuv.b-0.5;               //V

		rgba.r=yuv.r+1.5958*yuv.b ;					//R
		rgba.g=yuv.r-0.39173*yuv.g-0.81290*yuv.b;   //G
		rgba.b=yuv.r+2.017*yuv.g;	
	}

	rgba.a = alpha;
	FragColor = rgba;
}

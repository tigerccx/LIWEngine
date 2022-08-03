#version 450 core

layout (local_size_x = 16, local_size_y = 8) in;

layout(r32ui) readonly uniform uimage2D indexImg;
layout(r32ui) uniform uimage2D testImgOut;

void main(void)
{
	//Do nothing
}
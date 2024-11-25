#version 460

layout (location = 0) out vec2 outUV;

void main()
{
    outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    outUV = outUV *2.f - 1.f;

    if(outUV.x == 3 )
    {
        outUV = vec2(outUV.x/3,outUV.y );
    }
    if(outUV.y == 3 )
    {
        outUV = vec2(outUV.x,outUV.y/3 );
    }
    gl_Position = vec4(outUV.xy, 0.0f, 1.0f);
}

//
//  circle_quad_shader.glsl
//  ikan
//
//  Created by iKan on 27/12/22.
//

// vertex Shader
#type vertex
#version 330 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in float a_Thickness;
layout(location = 6) in float a_Fade;
layout(location = 7) in int   a_ObjectID;

uniform mat4 u_ViewProjection;

out VS_OUT
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
    float Thickness;
    float Fade;
    float ObjectID;
} vs_out;

void main()
{
    vs_out.Color         = a_Color;
    vs_out.TexCoord      = a_TexCoord;
    vs_out.TexIndex      = a_TexIndex;
    vs_out.TilingFactor  = a_TilingFactor;
    vs_out.Thickness     = a_Thickness;
    vs_out.Fade          = a_Fade;
    vs_out.ObjectID      = a_ObjectID;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// Fragment Shader
#type fragment
#version 330 core
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int  o_IDBuffer;

in VS_OUT
{
    vec4  Color;
    vec2  TexCoord;
    float TexIndex;
    float TilingFactor;
    float Thickness;
    float Fade;
    float ObjectID;
} fs_in;

uniform sampler2D u_Textures[16];

void main()
{
    vec4 texColor = fs_in.Color;
    switch(int(fs_in.TexIndex))
    {
        case 0: texColor *= texture(u_Textures[0], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 1: texColor *= texture(u_Textures[1], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 2: texColor *= texture(u_Textures[2], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 3: texColor *= texture(u_Textures[3], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 4: texColor *= texture(u_Textures[4], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 5: texColor *= texture(u_Textures[5], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 6: texColor *= texture(u_Textures[6], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 7: texColor *= texture(u_Textures[7], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 8: texColor *= texture(u_Textures[8], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 9: texColor *= texture(u_Textures[9], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 10: texColor *= texture(u_Textures[10], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 11: texColor *= texture(u_Textures[11], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 12: texColor *= texture(u_Textures[12], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 13: texColor *= texture(u_Textures[13], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 14: texColor *= texture(u_Textures[14], fs_in.TexCoord * fs_in.TilingFactor); break;
        case 15: texColor *= texture(u_Textures[15], fs_in.TexCoord * fs_in.TilingFactor); break;
    }
    if(texColor.a < 0.1)
        discard;
    
    float distance = 1.0 - length(fs_in.TexCoord);
    float color = smoothstep(0.0, fs_in.Fade, distance);
    color *= smoothstep(fs_in.Thickness + fs_in.Fade, fs_in.Thickness, distance);
    
    o_Color = texColor;
    o_Color.a *= color;
    
    if (o_Color.a > 0)
    {
        o_IDBuffer = int(fs_in.ObjectID);
    }
    else
    {
        o_IDBuffer = -1;
    }
  
}

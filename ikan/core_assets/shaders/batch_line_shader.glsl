//
//  batch_quad_shader.glsl
//  ikan
//
//  Created by iKan on 27/12/22.
//

// vertex Shader
#type vertex
#version 330 core

layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;

uniform mat4 u_ViewProjection;

out VS_OUT
{
  vec4 Color;
} vs_out;

void main()
{
  vs_out.Color = a_Color;
  
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// Fragment Shader
#type fragment
#version 330 core
layout(location = 0) out vec4 o_Color;

in VS_OUT
{
  vec4 Color;
} fs_in;

void main()
{
  o_Color = fs_in.Color;
}

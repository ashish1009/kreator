//
//  basic_shader.glsl
//  Kreator
//
//  Created by Ashish . on 25/13/22.
//

// This file includes the Basic Shader code

// Vertex Shader
#type vertex

#version 330 core
layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec2  a_TexCoords;

uniform mat4 u_ViewProjection;
out vec2 v_TexCoords;

void main()
{
  v_TexCoords = a_TexCoords;
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// Fragment Shader
#type fragment

#version 330 core
layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoords;
uniform sampler2D u_Texture;

void main()
{
  o_Color = texture(u_Texture, v_TexCoords);
}

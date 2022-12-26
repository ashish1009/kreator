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

uniform mat4 u_ViewProjection;

void main()
{
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// Fragment Shader
#type fragment

#version 330 core
layout(location = 0) out vec4 o_Color;

void main()
{
  o_Color = vec4(1.0f);
}

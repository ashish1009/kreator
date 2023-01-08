//
//  box_3d_shader.glsl
//  ikan
//
//  Created by Ashish . on 08/01/23.
//

// This file includes the Basic Shader code

// Vertex Shader
#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;

void main()
{
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

// Fragment Shader
#type fragment
#version 330 core
out vec4 FragColor;

void main()
{
  FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}

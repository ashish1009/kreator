//
//  TextShader.glsl
//  Kreator
//
//  Created by iKan on 28/12/22.
//

// vertex Shader
#type vertex
#version 330 core
layout(location = 0) in vec3  a_Position;
layout(location = 1) in vec4  a_Color;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in int   a_ObjectID;

out vec2 v_TexCoords;
out vec4 v_Color;
out float v_ObjectID;

uniform mat4 u_ViewProjection;

void main()
{
  gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
  v_TexCoords = a_TexCoord;
  v_Color = a_Color;
  v_ObjectID = a_ObjectID;
}

// Fragment Shader
#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int  o_IDBuffer;

in vec2 v_TexCoords;
in vec4 v_Color;
in float v_ObjectID;

uniform sampler2D u_Text;

void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Text, v_TexCoords).r);
    
  o_Color = v_Color * sampled;
  o_IDBuffer = int(v_ObjectID);
}

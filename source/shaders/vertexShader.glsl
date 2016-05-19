#version 440
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fnormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(){
  gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
  fnormal = normal;
}

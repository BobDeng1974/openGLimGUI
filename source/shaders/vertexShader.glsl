#version 400
in vec3 position;

//uniform mat4 modelmatrix;
//uniform mat4 viewmatrix;
//uniform mat4 projectionmatrix;
uniform mat4 MVP;

void main(){
  gl_Position = MVP * vec4(position, 1.0);
}

#version 440
in vec3 fnormal;
out vec4 fcolor;

void main(){
  fcolor = vec4(fnormal, 1.0);
  //fcolor = vec4(1.0,0.0,0.0, 1.0);
}

#version 330
layout( location = 0 ) in vec3 vertex_modelSpace;

uniform mat4 MVP;

out vec3 interpolateColor;

void main()
{
    gl_Position = MVP * vec4( vertex_modelSpace, 1.0 );
    interpolateColor = vec3(0,1,1);
}

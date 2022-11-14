#version 400
in vec2 a_Position;
uniform mat4 u_ProjectionMatrix;

void main()
{
    gl_Position = u_ProjectionMatrix * vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}

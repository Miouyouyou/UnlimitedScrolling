precision highp float;

attribute vec2 xy;

uniform mat4 projection;
uniform vec2 global_offset;

void main()
{
	gl_Position = projection * vec4(xy+global_offset, 0.99, 1.0);
}

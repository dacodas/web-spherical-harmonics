attribute vec2 vertex_position;
varying vec2 texture_coordinates;

void main()
{
    gl_Position = vec4( 0.5 + ( vertex_position / 2.0 ), 0, 1 );
    texture_coordinates = vertex_position;
}

precision mediump float;

varying vec2 texture_coordinates;

uniform sampler2D rho_sampler;
uniform float screen_scale;

void main()
{
    gl_FragColor = texture2D(rho_sampler, vec2(texture_coordinates.x, texture_coordinates.y));
}

precision mediump float;

attribute vec2 vertex_position;
// attribute vec3 model_position;

varying vec2 texture_coordinates;
varying vec3 position_tpr;
varying vec3 position;
varying vec3 normal;

uniform float amplitude;
uniform float phi_offset;

uniform sampler2D rho_sampler_vertex;
uniform sampler2D x_sampler;
uniform sampler2D y_sampler;
uniform sampler2D z_sampler;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

float rho(vec2 samplerPosition)
{
    float textureValue = texture2D( rho_sampler_vertex, samplerPosition.xy ).r;
    float rho = 1.0 + 0.5 * amplitude * ( 2.0 * textureValue - 1.0 );
    return rho;
}

vec3 calculateNormal(vec2 samplerPosition, float theta, float phi, float _rho)
{
    const float delta = 0.01;
    const float delta_theta = M_PI * delta;
    const float delta_phi = 2.0 * M_PI * delta;

    vec2 position_plus_theta = vec2( samplerPosition.x + delta, samplerPosition.y );
    vec2 position_plus_phi = vec2( samplerPosition.x, samplerPosition.y + delta );
    float delta_rho_theta = rho(samplerPosition) - rho(position_plus_theta);
    float delta_rho_phi = rho(samplerPosition) - rho(position_plus_phi);

    vec3 _normal = vec3( - delta_phi * delta_rho_theta , delta_theta * delta_rho_phi, delta_theta * delta_phi );
    vec3 normal = _normal / length(_normal);

    // We convert unit vectors here, not components
    // http://www.physics.purdue.edu/~jones105/phys310/coordinates.pdf
    vec3 normal_xyz = mat3(
        _rho * cos(theta) * cos(phi),   _rho * cos(theta) * sin(phi),  _rho * sin(theta),
        -_rho * sin(theta) * sin(phi),  _rho * sin(theta) * cos(phi),   0,
        sin(theta) * cos(phi),         sin(theta) * sin(phi),         cos(theta)
    ) * normal;

    return normal_xyz;
}

void main()
{
    // Might be nice to grab these from a texture rather than computing them every time
    // float x = rho * texture( x_sampler, vertex_position.xy ).r;
    // float y = rho * texture( y_sampler, vertex_position.xy ).r;
    // float z = rho * texture( z_sampler, vertex_position.xy ).r;

    float theta = M_PI * ( vertex_position.x );
    float phi = 2.0 * M_PI * ( vertex_position.y );
    float rho = rho(vertex_position);

    float x = rho * sin( theta ) * cos( phi );
    float y = rho * sin( theta ) * sin( phi );
    float z = rho * cos( theta );

    vec4 position4 = model * vec4( vec3(x, y, z), 1.0 );
    position = position4.xyz;
    position_tpr = vec3( theta, phi, rho );
    texture_coordinates = vertex_position.xy;
    normal = calculateNormal(vertex_position, theta, phi, rho);

    gl_Position = proj * view * position4;
}

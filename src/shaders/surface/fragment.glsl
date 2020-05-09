precision mediump float;

varying vec2 texture_coordinates;
varying vec3 position;
varying vec3 position_tpr;
varying vec3 normal;

uniform sampler2D rho_sampler;
// uniform sampler2D normals_sampler;
uniform sampler2D gridlines_sampler;
uniform float max_amplitude;
uniform float amplitude;
uniform vec3 light_position;
uniform vec3 view_position;

uniform bool draw_gridlines;
uniform bool normal_coloring;

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

float modI(float a,float b) {
	float m=a-floor((a+0.5)/b)*b;
	return floor(m+0.5);
}

vec3 gridline(vec3 color) {
	float rho = position_tpr.z;
	float theta = position_tpr.x;
	float phi = position_tpr.y;
	float thetaIndex = theta * 1000.0;
	float phiIndex = phi  * 1000.0;

	if ( modI(thetaIndex, 100.0) < 5.0 || modI(phiIndex, 100.0) < 5.0 )
	{
		 return vec3(0.0, 0.0, 0.0);
	}

	return color;
}

vec3 colorNormal(vec3 normal) {
	return ( normal / 2.0 ) + 0.5;
}

// https://learnopengl.com/Lighting/Basic-Lighting
void main()
{
	const vec3 lightColor = vec3(1, 1, 1);
	const float ambientStrength = 0.8;
	const float specularStrength = 0.2;
	const vec3 ambient = ambientStrength * lightColor;

	vec3 objectColor = vec3(0.5 + 0.2 * sin(position_tpr.y), 0, 0.5 + 0.2 * cos(position_tpr.x));
	vec3 color;

	if ( normal_coloring )
	{
		color = colorNormal(normal);
	}
	else
	{
		vec3 light_direction = normalize(light_position - position);
		vec3 view_direction = normalize(view_position - position);
		vec3 reflect_direction = reflect(-light_direction, normal);

		float diff = max(dot(normal, light_direction), 0.0);
		vec3 diffuse = diff * lightColor;

		float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 5.0);
		vec3 specular = specularStrength * spec * lightColor;

		color = ( ambient + diffuse + specular ) * objectColor;
	}

	vec4 color_a;
	if ( draw_gridlines )
	{
		vec4 grid = texture2D( gridlines_sampler, texture_coordinates );
		color_a = mix(vec4(color, 1.0), grid, grid.a);
	}
	else
	{
		color_a = vec4(color, 1.0);
	}

	gl_FragColor = color_a;
}

// vec3 oldNormal()
// {
// 	// Why does the lighting only line up when subtracting 45 degrees from phi
// 	vec3 normal_tpr = ( 2.0 * texture2D( normals_sampler, texture_coordinates ) - 1.0 ).xyz;
//
// 	float rho = position_tpr.z;
// 	float theta = position_tpr.x;
// 	float phi = position_tpr.y;
//
// 	// We convert unit vectors here, not components
// 	// http://www.physics.purdue.edu/~jones105/phys310/coordinates.pdf
// 	vec3 normal_xyz = mat3(
// 		rho * cos(theta) * cos(phi),   rho * cos(theta) * sin(phi),  rho * sin(theta),
// 		-rho * sin(theta) * sin(phi),  rho * sin(theta) * cos(phi),   0,
// 		sin(theta) * cos(phi),         sin(theta) * sin(phi),         cos(theta)
// 	) * normal_tpr;
//
// 	vec3 norm = - normalize(normal_xyz);
//
// 	return norm;
// }
//

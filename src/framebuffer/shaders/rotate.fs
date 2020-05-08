#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float beta;
uniform float Theta;
uniform float Phi;

const float PI = 3.1415926535897932384626433832795;

float rotatedPhi(
		float theta,
		float phi,
		float Theta,
		float Phi,
		float beta
		)
{
	return atan((((-1.0*pow(cos(Phi),2.0)*pow(sin(Theta),2.0))-1.0*pow(cos(Theta),2.0))*(1.0-1.0*cos(beta))+1.0)*sin(phi)*sin(theta)+(cos(Theta)*sin(beta)+cos(Phi)*sin(Phi)*pow(sin(Theta),2.0)*(1.0-1.0*cos(beta)))*cos(phi)*sin(theta)+(sin(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta))-1.0*cos(Phi)*sin(Theta)*sin(beta))*cos(theta),(cos(Phi)*sin(Phi)*pow(sin(Theta),2.0)*(1.0-1.0*cos(beta))-1.0*cos(Theta)*sin(beta))*sin(phi)*sin(theta)+(((-1.0*pow(sin(Phi),2.0)*pow(sin(Theta),2.0))-1.0*pow(cos(Theta),2.0))*(1.0-1.0*cos(beta))+1.0)*cos(phi)*sin(theta)+(sin(Phi)*sin(Theta)*sin(beta)+cos(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta)))*cos(theta));
}

float rotatedTheta(
		float theta,
		float phi,
		float Theta,
		float Phi,
		float beta
		)
{
	return atan(pow(pow((cos(Phi)*sin(Phi)*pow(sin(Theta),2.0)*(1.0-1.0*cos(beta))-1.0*cos(Theta)*sin(beta))*sin(phi)*sin(theta)+(((-1.0*pow(sin(Phi),2.0)*pow(sin(Theta),2.0))-1.0*pow(cos(Theta),2.0))*(1.0-1.0*cos(beta))+1.0)*cos(phi)*sin(theta)+(sin(Phi)*sin(Theta)*sin(beta)+cos(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta)))*cos(theta),2.0)+pow((((-1.0*pow(cos(Phi),2.0)*pow(sin(Theta),2.0))-1.0*pow(cos(Theta),2.0))*(1.0-1.0*cos(beta))+1.0)*sin(phi)*sin(theta)+(cos(Theta)*sin(beta)+cos(Phi)*sin(Phi)*pow(sin(Theta),2.0)*(1.0-1.0*cos(beta)))*cos(phi)*sin(theta)+(sin(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta))-1.0*cos(Phi)*sin(Theta)*sin(beta))*cos(theta),2.0),0.5),(cos(Phi)*sin(Theta)*sin(beta)+sin(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta)))*sin(phi)*sin(theta)+(cos(Phi)*cos(Theta)*sin(Theta)*(1.0-1.0*cos(beta))-1.0*sin(Phi)*sin(Theta)*sin(beta))*cos(phi)*sin(theta)+(((-1.0*pow(sin(Phi),2.0)*pow(sin(Theta),2.0))-1.0*pow(cos(Phi),2.0)*pow(sin(Theta),2.0))*(1.0-1.0*cos(beta))+1.0)*cos(theta));
}

float normal_pdf(float x, float m, float s)
{
	const float inv_sqrt_2pi = 0.3989422804014327;
	float a = (x - m) / s;

	// return inv_sqrt_2pi / s * exp( -0.5 * a * a );
	return exp( -0.5 * a * a );
}

void main()
{
	float theta = TexCoords.g * PI;
	float phi = TexCoords.r * 2 * PI;

	float rotatedTheta = rotatedTheta(theta, phi, Theta, Phi, beta);
	float rotatedPhi = rotatedPhi(theta, phi, Theta, Phi, beta);

	if ( rotatedPhi < 0.0 ) { rotatedPhi += 2.0 * PI ; }

	vec2 newTexCoords = vec2( rotatedPhi / ( PI * 2.0 ), rotatedTheta / PI);

	vec3 col = texture(screenTexture, newTexCoords).rgb;

	// envelop in gaussian
	// {
	// 	col = 2.0 * ( col - 0.5 );
	// 	col.x = col.x * normal_pdf(rotatedPhi * 5, PI, 1);
	// 	col /= 10.0;
	// 	col = ( col / 2.0 ) + 0.5;
	// 	col.yz = vec2(0.0, 0.0);
	// }

	// draw gridlines
	{
		int thetaIndex = int(newTexCoords.g * 1000);
		int phiIndex = int(newTexCoords.r * 1000);
		if ( thetaIndex % 100 < 5 || phiIndex % 100 < 5 )
		{
			col = vec3(0.0, 0.0, 0.0);
		}
	}

	FragColor = vec4(col, 1.0);
}

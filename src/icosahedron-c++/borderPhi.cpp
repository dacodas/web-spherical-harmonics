const float epsilon = 0.05 * ( 2.0 * M_PI );

bool borderPhiLow(float phi, float epsilon)
{
	return std::abs(phi) < epsilon;
}

bool borderPhiHigh(float phi, float epsilon)
{
	return std::abs( 2.0 * M_PI - phi ) < epsilon;
}

bool borderPhiLow(float phi)
{
	return borderPhiLow(phi, epsilon);
}

bool borderPhiHigh(float phi)
{
	return borderPhiHigh(phi, epsilon);
}

float adjustPhi(float phi)
{
	float margin_epsilon = epsilon + 0.5 * ( 2.0 * M_PI );

	if ( borderPhiLow(phi, margin_epsilon) )
	{
		return phi + 2 * M_PI;
		// return phi;
	}
	else if ( borderPhiHigh(phi, margin_epsilon) )
	{
		// return phi - 2 * M_PI;
		return phi;
	}
	else
	{
		return phi;
	}
}


#include <iostream>

#include "rotation.h"

int main(int argc, char* argv[]) 
{
	constexpr double 
		Theta {0.0d}, 
		Phi {0.0d}, 
		theta {M_PI * 0.5d}, 
		phi {0.0d}, 
		beta {M_PI * 0.25d};

	std::cout 
		<< "Rotating (" << theta << ", " << phi << ") " 
		<< "around (" << Theta << ", " << Phi << ") "
		<< "by " << beta << " " 
		<< "gives (" 
			<< rotatedTheta(theta, phi, Theta, Phi, beta) 
			<< ", " 
			<< rotatedPhi(theta, phi, Theta, Phi, beta) 
			<< ")\n";
}

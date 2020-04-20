
int main(int argc, char* argv[])
{
	if ( argc < 2 )
	{
		std::cout 
			<< "Specify subcommand among: " << "\n"
			// $SUBCOMMANDS
			;
		return(1);
	}

	// $PROCESS_ARGUMENTS

	std::cout << "Unrecognized subcommand" << "\n";
	return 1;
}

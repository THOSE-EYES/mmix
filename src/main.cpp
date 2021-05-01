#include <application.h>

int main(int argc, char** argv) {
	// TODO : create a possibility for the program to use different translators by
	//			creating a superclass and using it as a parameter for the app
    Application* application;

    // Check if the number arguments is correct
    if (argc != 3)
        throw new std::invalid_argument("There are not enough arguments!");

    application = new Application(std::string(argv[1]), std::string(argv[2]));
    application->start();

    return 0;
}
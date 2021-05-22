// Include Boost headers
#include <boost/program_options.hpp>

// Include C++ STL headers
#include <vector>
#include <string>
#include <iostream>
#include <memory>

// Include project headers
#include "application.h"
#include "exceptions.h"

using CompilationMode = Application::CompilationMode;

int main(int argc, char** argv) {
	// Add options
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("input,i", boost::program_options::value<std::vector<std::string>>()->multitoken(), "input "
                "file")
		    ("output,o", boost::program_options::value<std::string>(), "output "
                "file")
			("preprocessor,E", boost::program_options::bool_switch()->default_value(false), "Invoke preprocessor only");

	// Parse arguments
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
            options(desc).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: options_description [options]" << std::endl << desc;
        return 0;
    }
	else if (!vm.count("input")) 
		throw mmix::exceptions::application::MissingParameterException("input");
	else if (!vm.count("output")) 
		throw mmix::exceptions::application::MissingParameterException("output");

    auto application = std::make_shared<Application>(vm["input"].as<std::vector<std::string>>(),
		vm["output"].as<std::string>());
	CompilationMode mode = vm["preprocessor"].as<bool>() ? 
		CompilationMode::PREPROCESSING : 
		CompilationMode::FULL;
	application->set_mode(mode);
    application->start();

    return 0;
}
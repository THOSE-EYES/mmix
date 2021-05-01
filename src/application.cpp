#include "application.h"

using mmix::parser::RawProgram;
using mmix::compiler::CompiledProgram;
using mmix::preprocessor::PreprocessedProgram;

Application::Application(std::string input_file, std::string output_file) :
	input_file_{input_file},
	output_file_{output_file} {}

void Application::start(void) {
	auto raw_program = read_program();
	mmix::Parser parser(raw_program);
	mmix::Preprocessor preprocessor(parser.get());

	// Compile the program and write it to the file
	compiler_ = std::make_shared<mmix::Compiler>(preprocessor.get());
	write_program(compiler_->get());
}

std::shared_ptr<RawProgram> Application::read_program(void) {
	std::ifstream input_stream(input_file_);
	std::string line;
	auto program = std::make_shared<RawProgram>();

	// Check if the file was opened
	if (!input_stream.is_open())
		throw std::ifstream::failure("File was not opened!");

	// Store every line of the program
	while (!input_stream.eof()) {
		std::getline(input_stream, line);
		if (not line.empty()) program->push_back(line);
	}

	// Close the stream
    input_stream.close();

	return program;
}

void Application::write_program(std::shared_ptr<CompiledProgram> program) {
	std::ofstream output_stream(output_file_);
	std::stringstream hex_stream;

	// Check if the file was opened
    if (!output_stream.is_open())
        throw std::invalid_argument("The output file is not correct!");

	// Store every value into the file
	for (auto code : *program) {
		std::stringstream hex_stream;
		hex_stream << std::setfill('0') << std::setw(16) << std::left << std::hex << code; 
		output_stream << hex_stream.str() << std::endl;
	}

	// Close the stream
    output_stream.close();
}

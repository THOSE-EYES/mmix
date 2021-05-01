#pragma once

// Include C++ STL headers
#include <exception>
#include <string>

namespace mmix {
	namespace exceptions {
		namespace preprocessor {
			/**
			 * The exception is thrown within Preprocessor class
			 * when the block is not found in the table
			 */
			class BlockNotFoundException : public std::exception {
			protected:
				std::string label_;												// label of the block
				std::string message_ =  "There is no block with a label :  ";
			public:
				/**
				 * Constructor
				 * @param label the label of the blovk that caused the exception
				 */
				explicit BlockNotFoundException(std::string& label) : label_{label} {
					message_ += label;
				}
			public:
				/**
				 * Get the descriprion of the exception
				 * @return C-string with a message
				 */
				virtual const char* what() const throw() {
					return message_.c_str();
				}
			};

			/**
			 * The exception is thrown within Preprocessor class
			 * when the block is found in the table (but it 
			 * shouldn't be)
			 */
			class BlockExistsException : public std::exception {
			protected:
				std::string label_;								// Label of the block
				std::string message_ = "Block exists :  ";
			public:
				/**
				 * Constructor
				 * @param label the label of the blovk that caused the exception
				 */
				explicit BlockExistsException(std::string& label) : label_{label} {
					message_ += label;
				}
			public:
				/**
				 * Get the descriprion of the exception
				 * @return C-string with a message
				 */
				virtual const char* what() const throw() {
					return message_.c_str();
				}
			};

			/**
			 * The exception is thrown within Preprocessor class
			 * when the label is not found in the table
			 */
			class LabelNotFoundException : public std::exception {
			protected:
				std::string label_;										// Name of the label that caused the exception
				std::string message_ = "There is no such label:  ";
			public:
				/**
				 * Constructor
				 * @param label name of the label that caused the exception
				 */
				explicit LabelNotFoundException(std::string& label) : label_{label} {
					message_ += label;
				}
			public:
				/**
				 * Get the descriprion of the exception
				 * @return C-string with a message
				 */
				virtual const char* what() const throw() {
					return message_.c_str();
				}
			};
		} // preprocessor

		namespace compiler {

		} // compiler

		namespace parser {
			/**
			 * The exception is thrown within Preprocessor class
			 * when the label is not found in the table
			 */
			class WrongLineException : public std::exception {
			protected:
				std::string line_;														// The line that caused the exception
				std::string message_ = "The following line was not parsed :  \n";
			public:
				/**
				 * Constructor
				 * @param line the line that caused the exception
				 */
				explicit WrongLineException(const std::string line) : line_{line} {
					message_ += line;
				}
			public:
				/**
				 * Get the descriprion of the exception
				 * @return C-string with a message
				 */
				virtual const char* what() const throw() {
					return message_.c_str();
				}
			};

			/**
			 * The exception is thrown within Preprocessor class
			 * when the label is not found in the table
			 */
			class UnexpectedTokenException : public std::exception {
			protected:
				std::string token_;														// The line that caused the exception
				std::string message_ = "The following token is unexpected :  \n";
			public:
				/**
				 * Constructor
				 * @param line the line that caused the exception
				 */
				explicit UnexpectedTokenException(const std::string token) : token_{token} {
					message_ += token;
				}
			public:
				/**
				 * Get the descriprion of the exception
				 * @return C-string with a message
				 */
				virtual const char* what() const throw() {
					return message_.c_str();
				}
			};
		} // parser
	} // exceptions
} // mmix
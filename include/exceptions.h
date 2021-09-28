/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

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
				std::string message_ =  "There is no block with the label :  ";
			public:
				/**
				 * Constructor
				 * @param label the label of the blovk that caused the exception
				 */
				explicit BlockNotFoundException(std::string& label) : label_{label} {
					message_ += "\"" + label + "\"";
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
					message_ += "\"" + label + "\"";
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
			 * when the block has mismatched ranges
			 */
			class BadBlockException : public std::exception {
			protected:
				std::string label_;								// Label of the block
				std::string message_ = "Bad block :  ";
			public:
				/**
				 * Constructor
				 * @param label the label of the blovk that caused the exception
				 */
				explicit BadBlockException(std::string& label) : label_{label} {
					message_ += "\"" + label + "\"";
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
					message_ += "[" + label + "]";
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
			 * when the directive is not found in the table
			 */
			class UnknownDirectiveException : public std::exception {
			protected:
				std::string directive_;							// Name of the unknown token that caused the exception
				std::string message_ = "Unknown directive :  ";
			public:
				/**
				 * Constructor
				 * @param label name of the label that caused the exception
				 */
				explicit UnknownDirectiveException(std::string& directive) : directive_{directive} {
					message_ += "[" + directive + "]";
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
			 * The exception is thrown within Parser class
			 * when the line has errors
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
					message_ += "\"" + line + "\"";
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
			 * The exception is thrown within Parser class
			 * when the token is not recognized
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

		namespace application {
			/**
			 * The exception is thrown at the start of the application
			 * to indicate a missing parameter
			 */
			class MissingParameterException : public std::exception {
			protected:
				std::string parameter_;														// The line that caused the exception
				std::string message_ = "The following parameter is missing : ";
			public:
				/**
				 * Constructor
				 * @param parameter the parameter that caused the exception
				 */
				explicit MissingParameterException(const std::string parameter) : parameter_{parameter} {
					message_ += "[" + parameter_ + "]";
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
		} // namespace application

		namespace macroprocessor {
			/**
			 * The exception is thrown when the macro is already in the 
			 * table
			 */
			class MacroExistsException : public std::exception {
			protected:
				std::string label_;								// Label of the block
				std::string message_ = "Macro exists :  ";
			public:
				/**
				 * Constructor
				 * @param label the label of the blovk that caused the exception
				 */
				explicit MacroExistsException(const std::string& label) : label_{label} {
					message_ += "\"" + label + "\"";
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
			 * The exception is thrown when the macro is not correct
			 * and cant be processed
			 */
			class UnknownMacroException : public std::exception {
			protected:
				std::string macro_;							// Name of the unknown macro that caused the exception
				std::string message_ = "Unknown macro :  ";
			public:
				/**
				 * Constructor
				 * @param macro name of the macro that caused the exception
				 */
				explicit UnknownMacroException(std::string& macro) : macro_{macro} {
					message_ += "[" + macro + "]";
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
			 * The exception is thrown when there is no label "Main"
			 * in the source files
			 */
			class NoMainFileException : public std::exception {
			protected:
				std::string message_ = "Main file was not found";

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
			 * The exception is thrown when a macro is not found in the table
			 */
			class MacroNotFoundException : public std::exception {
			protected:
				std::string macro_;								// Name of the unknown macro that caused the exception
				std::string message_ = "Macro not found :  ";	// The message to print
			public:
				/**
				 * Constructor
				 * @param macro name of the macro that caused the exception
				 */
				explicit MacroNotFoundException(const std::string macro) : macro_{macro} {
					message_ += "[" + macro + "]";
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
			 * The exception is thrown when the file doesn't exist
			 * in the table
			 */
			class FileNotFoundException : public std::exception {
			protected:
				std::string file_;								// Name of the unknown file that caused the exception
				std::string message_ = "File not found :  ";	// The message to print
			public:
				/**
				 * Constructor
				 * @param macro name of the file that caused the exception
				 */
				explicit FileNotFoundException(const std::string file) : file_{file} {
					message_ += "[" + file + "]";
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
		} // namespace macroprocessor
	} // exceptions
} // mmix
#pragma once

// Include C++ STL headers
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <tuple>
#include <functional>

// Include project headers
#include "instruction.h"
#include "parser.h"

namespace mmix {
	namespace macroprocessor {
		using MacroprocessedProgram = std::vector<std::shared_ptr<Instruction>>;
	} // namespace macroprocessor
	
	/**
	 * Macroprocessor is used to process macros. It
	 * can expand macros an include files into a 
	 * target file. The class returns a single (main)
	 * file with a complete program.
	 */
	class Macroprocessor {
	private:
		/**
		 * This is a superclass for all macro structures.
		 * It holds common fields and is used as a element
		 * type for containers.
		 */
		struct MacroEntry {
		public:
			using Parameters = std::vector<std::string>;

			Parameters	parameters;
			std::string label;

		public:
			/**
			 * Constructor
			 */
			MacroEntry() = default;

			/**
			 * Constructor
			 * @param parameters_ 	macro parameters
			 * @param label_ 		the label on the macro 
			 */
			MacroEntry(Parameters parameters_, std::string label_ = "") : 
				parameters{parameters_}, label{label_} {}

			/**
			 * Destructor
			 */
			virtual ~MacroEntry() = default;
		};

		/**
		 * This structure is used with MacroExpression
		 * structure in order to expand a macro into an
		 * actual instruction which the compiler 
		 * understands.
		 */
		struct UseMacro : MacroEntry {
		public:
			size_t offset;

		public:
			/**
			 * Constructor
			 */
			UseMacro() = default;

			/**
			 * Constructor
			 * @param parameters_	macro parameters
			 * @param offset_		the address of the macro to be expanded
			 */
			UseMacro(Parameters parameters_, size_t offset_ = 0, const std::string& label_ = "") : 
				MacroEntry{parameters_, label_}, offset{offset_} {}

		};

		/**
		 * The class is used to store data about 
		 * macros with "MACRO" in it. It's expanded
		 * by the data in UseMacro object.
		 */
		struct MacroExpression : MacroEntry {
		public:
			using Expression = Instruction;

			std::shared_ptr<Expression>	expression;

		public:
			/**
			 * Constructor
			 */
			MacroExpression() = default;

			/**
			 * Constructor
			 * @param label_		the label on the macro 
			 * @param expression_	the expression which is to be changed
			 * @param parameters_	macro parameters
			 */
			MacroExpression(const std::string& label_, std::shared_ptr<Expression> expression_, 
							Parameters parameters_) :
							MacroEntry{parameters_,label_}, expression{expression_}
							{}
		};

		/**
		 * The macro is used when there is a
		 * file to include into a given target
		 * file.
		 */
		struct IncludeMacro : MacroEntry {
		public:
			std::string filename;

		public:
			/**
			 * Constructor
			 */
			IncludeMacro() = default;

			/**
			 * Constructor
			 * @param filename_ the name of the file to include
			 */
			IncludeMacro(const std::string& filename_) : filename{filename_} {}

		private:
			using MacroEntry::Parameters;
			using MacroEntry::parameters;
			using MacroEntry::label;
		};

		using MacroEntries	= std::set<std::shared_ptr<MacroEntry>>;
		using MacroTable 	= std::map<std::string, MacroEntries>;

	protected:
		std::shared_ptr<parser::ParsedProgram> 					sources_;		// The sources of the program
		std::shared_ptr<macroprocessor::MacroprocessedProgram>	program_;		// The result of processing macros
		std::shared_ptr<MacroTable> 							macro_table_;	// The table of macro's to process

	protected:
		/**
		 * Fill the macro table with data
		 */
		virtual void fill_tables(void);

		/**
		 * Include files to the given file
		 * @param target_filename the file to which include other files
		 */
		void include_files(const std::string& target_filename);

		/**
		 * Replace all the macros with actual data
		 */
		void replace_macros(void);

		/**
		 * Expand a macro into an actual instruction
		 * @param value 	data for macro expanding
		 * @param filename 	the file where the instruction residents
		 * @return 			the instruction from the expanded macro
		 */
		std::shared_ptr<Instruction> expand_macro(std::shared_ptr<UseMacro>& value, const std::string& filename);

		/**
		 * Extract data from the macro and differentiate macro types
		 * @param value 	the macro to process
		 * @param offset 	macro's offset in the memory
		 * @return 			a macro with a specific type
		 */
		std::shared_ptr<MacroEntry> process_macro(const std::shared_ptr<Macro>& value, size_t offset);

		/**
		 * Find a macro with a given label
		 * @param label 	macro's label
		 * @param filename 	the name of the file where the macro is stored
		 * @return 			the macro (if it exists)
		 */
		std::shared_ptr<MacroEntry> find_label(const std::string& label, const std::string& filename);

	public:
		/**
		 * Constructor
		 * @param sources the source files of the 
		 */
		Macroprocessor(std::shared_ptr<parser::ParsedProgram> program);

	public:
		friend bool operator<(const IncludeMacro& rhs, const IncludeMacro& lhs);
		friend bool operator<(const MacroExpression& lhs, const MacroExpression& rhs);
		friend bool operator<(const UseMacro& lhs, const UseMacro& rhs);

		/**
		 * Get the result of the processing operation
		 * @return the processed program
		 */
		std::shared_ptr<macroprocessor::MacroprocessedProgram> get();

	};
} // namespace mmix 
#pragma once

// Include C++ STL headers
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <stack>
#include <cstdint>

// Include project headers
#include "instruction.h"
#include "mnemonics.h"
#include "exceptions.h"
#include "constants.h"
#include "macroprocessor.h"

namespace mmix {
	namespace preprocessor {
		using PreprocessedProgram = std::vector<std::shared_ptr<Instruction>>;
	} // namespace preprocessor

	/**
	 * Preprocessor completes changes to the code on the first stage :
	 * moving blocks, replacing labels etc.
	 */
	class Preprocessor {
	private:
		/**
	 	 * The strucure holds info about program blocks
	 	 */
		struct Block {
			std::string label;
			uint64_t 	origin;
			uint64_t 	start;
			uint64_t 	end;
		};
		using BlockTable = std::vector<Block>;

		std::shared_ptr<BlockTable> block_table_;			// Table of found blocks

	private :
		/**
		 * Create a new block with given address and name
		 * @param label the name of the block
		 * @param address address of the block
		 */
		void create_block(std::string label, uint64_t address);

		/**
		 * Find a block with a given name
		 * @param label
		 * @return a block with the name
		 */
		Block& find_block(std::string& label);

		/**
		 * Update addresses inside of the block
		 * @param block the block to update
		 */
		void update_block_addresses(Block& block);

		/**
		 * Move the block to it's place
		 * @param block the block to move
		 */
		void emplace_block(Block& block);

	protected :
		using Label 		= std::pair<const std::string, std::string>;
		using LabelTable 	= std::map<std::string, std::string>;

		std::shared_ptr<preprocessor::PreprocessedProgram> 	program_;				// The preprocessed program
		std::shared_ptr<LabelTable>							label_table_;     		// Table of found labels

	protected :
		/**
		 * Add a new labeled expression to the list
		 * @param label a new label
		 * @param expression the expression to associate with the label
		 */
		void create_label(std::string& label, std::string& expression);

		/**
		 * Find a certain label in the table
		 * @param label name of table to find
		 * @return the found label
		 */
		Label& find_label(std::string& label);

		/**
		 * Replace labels with the expressions from the table
		 * @param instruction the instruction to process
		 */
		void replace_labels(std::shared_ptr<Instruction>& instruction);

		/**
		 * Relocate instruction (used with  "LOC")
		 */
		void relocate_instructions(void);

		/**
		 * Fill label and block tables with data
		 */
		void fill_tables(void);

		/**
		 * Start the preprocessing
		 */
		void preprocess(void);

	public:
		/**
		 * Constructor
		 * @param program the parsed program
		 */
		Preprocessor(std::shared_ptr<macroprocessor::MacroprocessedProgram> program);

		/**
		 * Get the preprocessed program
		 * @return the program
		 */
		std::shared_ptr<preprocessor::PreprocessedProgram> get(void);
	};
} // namespace mmix 
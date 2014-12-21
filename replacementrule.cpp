/*
 * replacementrule.cpp
 *
 *  Created on: 21 dec 2014
 *      Author: Mattias Larsson Sköld
 */


#include "replacementrule.h"

std::vector<ReplacementRule> patternInterpretations = {
		{{
				SourceTree::Type,
				SourceTree::Raw,
		},
				SourceTree::VariableDeclaration,
				{
						SourceTree::Type,
						SourceTree::DefinitionName
				}
		},

		{anonymousClassDeclarationPattern, SourceTree::ClassDeclaration},
		{{

				SourceTree::ClassKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::ClassDeclaration,
				{
						SourceTree::ClassKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},
		//Templated class
		{{
				SourceTree::TemplateBlock,
				SourceTree::ClassKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::ClassDeclaration,
				{
//						SourceTree::TemplateBlock,
//						SourceTree::ClassKeyword,
//						SourceTree::DeclarationName,
//						SourceTree::BraceBlock,
						SourceTree::TemplateBlock,
						SourceTree::ClassKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock,
				}
		},

		{{

				SourceTree::StructKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::StructDeclaration,
				{
						SourceTree::StructKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},

		//Templated struct
		{{
				SourceTree::TemplateBlock,
				SourceTree::StructKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::StructDeclaration,
				{
						SourceTree::TemplateBlock,
						SourceTree::StructKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},

		{controlStatementPattern, SourceTree::ControlStatement},

		//Namespace
		{{
				SourceTree::NamespaceKeyWord,
				SourceTree::Raw,
				SourceTree::BraceBlock,
		},
				SourceTree::Namespace,
				{
						SourceTree::NamespaceKeyWord,
						SourceTree::DefinitionName,
						SourceTree::BraceBlock,
				}
		},

//		{{
//				SourceTree::VariableDeclaration,
//				SourceTree::Equals
//		}, //Probably another pattern in the future
//				SourceTree::AssignmentStatement
//		},

		{{
			SourceTree::VariableDeclaration,
			SourceTree::ParanthesisBlock,
		},
			SourceTree::FunctionDeclaration
		},

		{{
			SourceTree::FunctionDeclaration,
			SourceTree::BraceBlock,
		},
			SourceTree::FunctionDefinition,
		},


		{{
			SourceTree::BracketBlock, //Todo make sure this is identified even when it is not at an end of a line
			SourceTree::ParanthesisBlock,
			SourceTree::BraceBlock,
		},
			SourceTree::LambdaFunction,
		},
};


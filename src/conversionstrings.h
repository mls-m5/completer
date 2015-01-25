/*
 * conversionstrings.h
 *
 *  Created on: 20 dec 2014
 *      Author: Mattias Larsson Sköld
 */


#pragma once

#include <map>
#include "sourcetree.h"
#include <string>


//For converting string to type
std::map<std::string, SourceTree::DataType> nameInterpretations = {
		{"if", SourceTree::ControlStatementKeyword},
		{"while", SourceTree::ControlStatementKeyword},
		{"for", SourceTree::ControlStatementKeyword},
		{"switch", SourceTree::ControlStatementKeyword},
		{"class", SourceTree::ClassKeyword},
		{"template", SourceTree::TemplateKeyword},
		{"struct", SourceTree::StructKeyword},
		{"return", SourceTree::ReturnKeyword},
		{"new", SourceTree::NewKeyword},
		{"delete", SourceTree::DeleteKeyword},
		{"namespace", SourceTree::NamespaceKeyWord},
		{"public", SourceTree::PublicKeyword},
		{"protected", SourceTree::ProtectedKeyword},
		{"private", SourceTree::PrivateKeyword},

		{"+", SourceTree::Operator},
		{"++", SourceTree::Operator},
		{"-", SourceTree::Operator},
		{"--", SourceTree::Operator},
		{",", SourceTree::ComaOperator},
		{";", SourceTree::Semicolon},
		{"=", SourceTree::Equals},
		{"::", SourceTree::ScopeResolution},
		{"->", SourceTree::ElementSelectionThroughPointer},
		{".", SourceTree::ElementSelectionThroughPointer},
		{":", SourceTree::Colon},

};

//For converting type to string
const std::map<SourceTree::DataType, std::string> typeNameStrings = {
		{SourceTree::Equals, "equals"},
		{SourceTree::Raw, "raw"},
		{SourceTree::ClassKeyword, "class keyword"},
		{SourceTree::Type, "type"},
		{SourceTree::ControlStatementKeyword, "control statement keyword"},
		{SourceTree::ReturnKeyword, "return keyword"},
		{SourceTree::Operator, "operator"},
		{SourceTree::DeclarationName, "declaration name"},
		{SourceTree::DefinitionName, "name"},
		{SourceTree::NamespaceKeyWord, "namespace keyword"},
		{SourceTree::Namespace, "namespace"},
		{SourceTree::Variable, "variable"},
		{SourceTree::Digit, "digit"},
		{SourceTree::BinaryOperation, "binary operation"},
		{SourceTree::PublicKeyword, "public keyword"},
		{SourceTree::ProtectedKeyword, "protected keyword"},
		{SourceTree::PrivateKeyword, "private keyword"},

		//Groups
		{SourceTree::ParanthesisBlock , "()"},
		{SourceTree::BraceBlock, "{}"},
		{SourceTree::BracketBlock, "[]"},
		{SourceTree::VariableDeclaration, "variable definition"},
		{SourceTree::AssignmentStatement, "assignment"},
		{SourceTree::ClassDeclaration, "class"},
		{SourceTree::StructDeclaration, "struct"},
		{SourceTree::TemplateBlock, "template"},
		{SourceTree::FunctionDeclaration, "function declaration"},
		{SourceTree::FunctionDefinition, "function definition"},
		{SourceTree::LambdaFunction, "lambda function"},
		{SourceTree::AccessModifiers, "access modifier"},
};




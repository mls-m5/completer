/*
 * binaryoperators.h
 *
 *  Created on: 24 nov 2014
 *      Author: Mattias Larsson Sköld
 */


#pragma once


static const std::list<std::string> orderedBinaryOperators = {
	".*",
	"->*",
	"*", //Multiplication only
	"/",
	"%",
	"+",
	"-",
	"<<",
	">>",
	"<",
	"<=",
	">",
	">=",
	"==",
	"!=",
	"&",
	"^",
	"|",
	"&&",
	"||",
	"?:",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"<<=",
	">>=",
	"&=",
	"^=",
	"|=",
	"throw",
	",",
};

static bool isBinaryOperator(std::string name){
	for (auto it: orderedBinaryOperators) {
		if (it == name){
			return true;
		}
	}
	return false;
}



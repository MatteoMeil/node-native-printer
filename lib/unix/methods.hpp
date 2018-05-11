#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/ipp.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <stdio.h>

using namespace std;

namespace methods{
	//methods

	/**
	 * Return printer infos. If passed name is invalid, this method will return default printer infos
	 * @param printer printer name
	 */
	cups_dest_t* getPrinter(const char*);

	/**
	 * Copies a destination from src to dest
	 * @param src cups_dest_t* from which to copy informations
	 * @param dest cups_dest_t* in which to copy informations
	 * @return copy of src
	 */
	bool copyDest(cups_dest_t*, cups_dest_t*);

	/**
	 * Copies options from src to dest
	 * @param src cups_option_t* from which to copy informations
	 * @param num_options number of options to copy starting from the first
	 * @param dest cups_option_t* in which to copy informations
	 * @return copy of src 
	 */
	bool copyOptions(cups_option_t*, int, cups_option_t*);

	/**
	 * Return job status from job status string
	 * @param status IPP_JOB status constant
	 * @return status string
	 */
	const char* getJobStatusString(int);

	/**
	 * It has the same behaviour of system() but returns command's output string
	 * @pararm cmd command to execute
	 * @return output string
	 */
	string exec(const char*);

	/**
	 * Transform a string to lowercase
	 * @param data string to transform
	 * @return string lowercased
	 */
	string strtolower(string);

	/**
	 * Transform a char* to lowercase
	 * @param data char* to transform
	 * @return char* string lowercased
	 */
	const char* strtolower(char*);
}


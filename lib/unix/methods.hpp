#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/ipp.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <stdio.h>

using namespace std;

namespace methods{
	//structures

	/**
	 * Defines a general info for the job.
	 * I.e. the status of the job will be represented with job_info status = {"status", "waiting"})
	 */
	typedef struct job_info{
		const char* key;
		const char* value;
	} job_info;

	/**
	 * Collects all job_infos
	 */
	typedef struct printer_job{
		job_info* info;
		int num_info = 10;
		int num_jobs;
	} printer_job;

	//methods

	/**
	 * Return printer infos. If passed name is invalid, this method will return default printer infos
	 * @param printer printer name
	 */
	cups_dest_t* getPrinter(const char*);

	/**
	 * Return a pointer to the list of all printer_job.
	 * @param dest printer from which take the jobs
	 * @return pointer to printer_job list
	 */
	printer_job* getJobs(cups_dest_t*);

	/**
	 * Alias of system() that return output string
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


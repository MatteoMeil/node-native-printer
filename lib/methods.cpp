#include "methods.hpp"

using namespace std;

namespace methods{

	unordered_map<ipp_jstate_t, string> job_statuses = {
		{IPP_JOB_ABORTED, "aborted"},
		{IPP_JOB_CANCELED, "canceled"},
		{IPP_JOB_COMPLETED, "completed"},
		{IPP_JOB_HELD, "held"},
		{IPP_JOB_PENDING, "pending"},
		{IPP_JOB_PROCESSING, "processing"},
		{IPP_JOB_STOPPED, "stopped"}
	};
	
	cups_dest_t* getPrinter(char* printer){
		cups_dest_t* dests;
		int num_dests = cupsGetDests(&dests);

		cups_dest_t* result;

		if(printer != NULL && strlen(printer) > 0)
			result = cupsGetDest(printer, NULL, num_dests, dests);
		else
			result = cupsGetDest(cupsGetDefault(), NULL, num_dests, dests);

		return result;
	}

	printer_job* getJobs(cups_dest_t* dest){
		cups_job_t* jobs;
		int totalJobs = cupsGetJobs(&jobs, dest->name, 0, CUPS_WHICHJOBS_ACTIVE);

		printer_job* resultJob = new printer_job[totalJobs];

		resultJob->num_jobs = totalJobs;

		for(int i = 0; i < totalJobs; i++){
			
			job_info* info = new job_info[10];

			info[0] = {"completed_time", httpGetDateString(jobs->completed_time)};
			info[1] = {"creation_time", httpGetDateString(jobs->creation_time)};
			info[2] = {"format", jobs->format};
			info[3] = {"id", to_string(jobs->id).c_str()};
			info[4] = {"priority", to_string(jobs->priority).c_str()};
			info[5] = {"processing_time", httpGetDateString(jobs->processing_time)};
			info[6] = {"size", to_string(jobs->size).c_str()};
			info[7] = {"status", (job_statuses.at(jobs->state)).c_str()};
			info[8] = {"title", jobs->title};
			info[9] = {"user", jobs->user};

			resultJob[i].info = info;
		}

		return resultJob;
	}

	string exec(const char* cmd) {
		char buffer[128];
		std::string result = "";
		FILE* pipe = popen(cmd, "r");
		if (!pipe){ 
			result = "popen() failed!";
			return result;
		}
		try {
			while (!feof(pipe)) {
				if (fgets(buffer, 128, pipe) != NULL)
					result += buffer;
			}
		} catch (...) {
			pclose(pipe);
			throw;
		}
		
		pclose(pipe);
		
		return result;
	}
}
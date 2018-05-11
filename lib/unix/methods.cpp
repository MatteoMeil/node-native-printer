#include "methods.hpp"

using namespace std;

namespace methods{

	cups_dest_t* getPrinter(const char* printer){
		cups_dest_t* dests;
		int num_dests = cupsGetDests(&dests);

		cups_dest_t* temp;
		
		if(printer != NULL){
			string printerName = strtolower(string(printer)); 
			
			if(strlen(printer) > 0 && printerName.compare("null") != 0 && printerName.compare("undefined") != 0){
				temp = cupsGetDest(printer, NULL, num_dests, dests);
			}
			else
				temp = cupsGetDest(cupsGetDefault(), NULL, num_dests, dests);
		}
		else
			temp = cupsGetDest(cupsGetDefault(), NULL, num_dests, dests);			

		cups_dest_t* result = new cups_dest_t;
		if(temp != NULL){
			if(! copyDest(temp, result))
				return NULL;
		}
		else{
			free(result);
			result = NULL;
		}

		cupsFreeDests(num_dests, dests);
		dests = temp = NULL;
		return result;
	}

	bool copyDest(cups_dest_t* source, cups_dest_t* dest){
		if(source->instance != NULL){
			dest->instance = strdup(source->instance);

			if(dest->instance == NULL)
				return false;
		}

		if(source->name != NULL){
			dest->name = strdup(source->name);

			if(dest->name == NULL)
				return false;
		}
		
		dest->is_default = source->is_default;
		dest->num_options = source->num_options;

		dest->options = new cups_option_t[source->num_options];
		
		if(! copyOptions(source->options, source->num_options, dest->options)){
			cerr << "Errore durante la copia delle opzioni\n";
			return 1;
		}

		return true;
	}

	bool copyOptions(cups_option_t* source, int num, cups_option_t* dest){

		for(int i = 0; i < num; i++){
			dest[i].name = strdup(source[i].name);
			dest[i].value = strdup(source[i].value);
			
			if(dest[i].name == NULL)
				return false;
			if(dest[i].value == NULL)
				return false;
		}
		
		return true;
	}

	const char* getJobStatusString(int status){
		switch(status){
			case IPP_JOB_ABORTED:
				return "aborted";

			case IPP_JOB_CANCELED:
				return "canceled";

			case IPP_JOB_COMPLETED:
				return "completed";

			case IPP_JOB_HELD:
				return "held";

			case IPP_JOB_PENDING:
				return "pending";

			case IPP_JOB_PROCESSING:
				return "processing";

			case IPP_JOB_STOPPED:
				return "stopped";

			default:
				return "UNDEFINED";
		}
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

	string strtolower(string data){
		transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}
	
	const char* strtolower(char* str){
		string data(str);
		transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data.c_str();
	}

}
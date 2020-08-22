#include "API.hpp"

using namespace methods;

namespace API{
	CALLBACK(listPrinters){
		
		ISOLATE; 
		
		cups_dest_t* dests;
		int numDests = cupsGetDests(&dests);

		Local<Array> printers = Array::New(isolate, numDests);

		for(int i = 0; i < numDests; i++){
			Local<Object> printer = Object::New(isolate);
			printer->Set(isolate->GetCurrentContext(), UTF8_STRING("name"), UTF8_STRING(dests[i].name));
			printer->Set(isolate->GetCurrentContext(), UTF8_STRING("default"), Boolean::New(isolate, static_cast<bool>(dests[i].is_default)));
			
			printers->Set(isolate->GetCurrentContext(), i, printer);
		}

		cupsFreeDests(numDests, dests);
		args.GetReturnValue().Set(printers);
	}

	CALLBACK(printerInfo){

		ISOLATE;

		if(args.Length() < 1){
			THROW_EXCEPTION("Too few arguments");
			return;
		}

		String::Utf8Value printer(S_STR(args[0]));

		cups_dest_t* dest = getPrinter(*printer);

		if(dest == NULL){
			THROW_EXCEPTION("Printer not found or error retrieving printer");
			return;
		}

		cups_job_t* printerJobs;
		int num_jobs = cupsGetJobs(&printerJobs, dest->name, 0, CUPS_WHICHJOBS_ALL);

		Local<Object> result = Object::New(isolate);
		Local<Object> jobs = Array::New(isolate);
		Local<Object> CUPSOptions = Object::New(isolate);

		for(int i = 0; i < dest->num_options; i++){
			CUPSOptions->Set(isolate->GetCurrentContext(), UTF8_STRING(dest->options[i].name), UTF8_STRING(dest->options[i].value));
		}

		char id[5], priority[5], size[5];

		for(int i = 0; i < num_jobs; i++){
			Local<Object> job = Object::New(isolate);
			
			sprintf(id, "%d", printerJobs[i].id);
			sprintf(priority, "%d", printerJobs[i].priority);
			sprintf(size, "%d", printerJobs[i].size);

			job->Set(isolate->GetCurrentContext(), UTF8_STRING("completed_time"), UTF8_STRING(httpGetDateString(printerJobs[i].completed_time)));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("creation_time"), UTF8_STRING(httpGetDateString(printerJobs[i].creation_time)));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("format"), UTF8_STRING(printerJobs[i].format));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("id"), UTF8_STRING(id));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("priority"), UTF8_STRING(priority));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("processing_time"), UTF8_STRING(httpGetDateString(printerJobs[i].processing_time)));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("size"), UTF8_STRING(size));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("status"), UTF8_STRING(getJobStatusString(printerJobs[i].state)));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("title"), UTF8_STRING(printerJobs[i].title));
			job->Set(isolate->GetCurrentContext(), UTF8_STRING("user"), UTF8_STRING(printerJobs[i].user));

			jobs->Set(isolate->GetCurrentContext(), i, job);
		}

		cupsFreeJobs(num_jobs, printerJobs);
		free(dest);
		// result->Set(UTF8_STRING("infos"), infos);
		result->Set(isolate->GetCurrentContext(), UTF8_STRING("jobs"), jobs);
		result->Set(isolate->GetCurrentContext(), UTF8_STRING("CUPSOptions"), CUPSOptions);

		args.GetReturnValue().Set(result);
	}

	CALLBACK(printerOptions){
		ISOLATE;

		if(args.Length() < 1){
			THROW_EXCEPTION("Too few arguments");
			return;
		}
		
		String::Utf8Value printer(S_STR(args[0]));

		cups_dest_t* dest = getPrinter(*printer);

		if(dest == NULL){
			THROW_EXCEPTION("Printer not found");
			return;
		}
	
		const char* filename = cupsGetPPD(dest->name);
		ppd_file_t* ppd = ppdOpenFile(filename);
		
		ppdMarkDefaults(ppd);
		cupsMarkOptions(ppd, dest->num_options, dest->options);

		Local<Object> result = Object::New(isolate);
		Local<Object> resOptions = Object::New(isolate);
		Local<Object> resDefaults = Object::New(isolate);

		ppd_group_t* group = ppd->groups;

		for (int i = 0; i < ppd->num_groups; i++)
        {
			ppd_option_t* option = group->options;
			for (int j = 0; j < group->num_options; j++)
			{
				Local<Array> choices = Array::New(isolate, option->num_choices);
				resOptions->Set(isolate->GetCurrentContext(), UTF8_STRING(option->keyword), choices);
				ppd_choice_t* choice = option->choices;
				
				for(int h = 0; h < option->num_choices; h++){
					choices->Set(isolate->GetCurrentContext(), h, UTF8_STRING(choice->text));
					
					if(choice->marked)
						resDefaults->Set(isolate->GetCurrentContext(), UTF8_STRING(option->keyword), UTF8_STRING(choice->text));

					choice++;
				}

				option++;
			}

			group++;
		}

		ppdClose(ppd);
		free(dest);
		
		result->Set(isolate->GetCurrentContext(), UTF8_STRING("options"), resOptions);
		result->Set(isolate->GetCurrentContext(), UTF8_STRING("defaultOptions"), resDefaults);
		args.GetReturnValue().Set(result);
	}

	CALLBACK(defaultPrinterName){
		
		ISOLATE;
		
		cups_dest_t* printer = getPrinter(NULL);
		args.GetReturnValue().Set(UTF8_STRING(strdup(printer->name)));
		free(printer);
	}

	CALLBACK(print){
		using namespace std;

		ISOLATE;
		if(args.Length() < 3){
			THROW_EXCEPTION("Too few arguments");
			return;
		}

		string printer(*(String::Utf8Value(S_STR(args[0]))));
		string file(*(String::Utf8Value(S_STR(args[1]))));
		string options(*(String::Utf8Value(S_STR(args[2]))));
		
		cups_dest_t* dest = getPrinter(printer.c_str());
		printer = string(dest->name);

		string cmd = "lp -d " + printer + " " + file + " " + options;

		string result = exec(cmd.c_str());
		
		args.GetReturnValue().Set(UTF8_STRING(result.c_str()));
		free(dest);
	}
}
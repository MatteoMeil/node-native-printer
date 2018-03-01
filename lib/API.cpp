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
			printer->Set(UTF8_STRING("name"), UTF8_STRING(dests[i].name));
			printer->Set(UTF8_STRING("default"), Boolean::New(isolate, static_cast<bool>(dests[i].is_default)));
			
			printers->Set(i, printer);
		}

		cupsFreeDests(numDests, dests);
		args.GetReturnValue().Set(printers);
	}

	CALLBACK(printerInfo){

		ISOLATE;

		if(args.Length() < 1)
			THROW_EXCEPTION("Too few arguments");
		
		String::Utf8Value printer(args[0]->ToString());

		cups_dest_t* dest = getPrinter(*printer);

		if(dest == NULL)
			THROW_EXCEPTION("Printer not found");
		
		cups_option_t* options = dest->options;
		printer_job* printerJob = getJobs(dest);

		Local<Object> result = Object::New(isolate);
		
		Local<Object> infos = Object::New(isolate);
		Local<Object> jobs = Array::New(isolate);
		Local<Object> CUPSOptions = Object::New(isolate);

		for(int i = 0; i < dest->num_options; i++){
			CUPSOptions->Set(UTF8_STRING(options[i].name), UTF8_STRING(options[i].value));
		}

		for(int i = 0; i < printerJob->num_jobs; i++){
			Local<Object> job = Object::New(isolate);
			job_info* info = printerJob[i].info;

			for(int j = 0; j < printerJob->num_info; j++){
				job->Set(UTF8_STRING(info[j].key), UTF8_STRING(info[j].value));
			}

			jobs->Set(i, job);
		}

		for (int i = 0; i < dest->num_options; i++){
			CUPSOptions->Set(UTF8_STRING(options[i].name), UTF8_STRING(options[i].value));
		}
		
		result->Set(UTF8_STRING("infos"), infos);
		result->Set(UTF8_STRING("jobs"), jobs);
		result->Set(UTF8_STRING("CUPSOptions"), CUPSOptions);

		args.GetReturnValue().Set(result);
	}

	CALLBACK(printerOptions){
		ISOLATE;

		if(args.Length() < 1)
			THROW_EXCEPTION("Too few arguments");
		
		String::Utf8Value printer(args[0]->ToString());

		cups_dest_t* dest = getPrinter(*printer);

		if(dest == NULL)
			THROW_EXCEPTION("Printer not found");
	
		const char* filename = cupsGetPPD(*printer);
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
				resOptions->Set(UTF8_STRING(option->keyword), choices);
				ppd_choice_t* choice = option->choices;
				
				for(int h = 0; h < option->num_choices; h++){
					choices->Set(h, UTF8_STRING(choice->text));
					
					if(choice->marked)
						resDefaults->Set(UTF8_STRING(option->keyword), UTF8_STRING(choice->text));

					choice++;
				}

				option++;
			}

			group++;
		}
		
		result->Set(UTF8_STRING("options"), resOptions);
		result->Set(UTF8_STRING("defaultOptions"), resDefaults);
		args.GetReturnValue().Set(result);
	}

	CALLBACK(defaultPrinterName){
		
		ISOLATE;
		
		cups_dest_t* printer = getPrinter(NULL);
		args.GetReturnValue().Set(UTF8_STRING(printer->name));
	}

	CALLBACK(print){
		using namespace std;

		ISOLATE;
		if(args.Length() < 3)
			THROW_EXCEPTION("Too few arguments");

		string printer(*(String::Utf8Value(args[0]->ToString())));
		string file(*(String::Utf8Value(args[1]->ToString())));
		string options(*(String::Utf8Value(args[2]->ToString())));
		
		string cmd = "lp -d " + printer + " " + file + " " + options;

		string result = exec(cmd.c_str());
		
		args.GetReturnValue().Set(UTF8_STRING(result.c_str()));
	}
}
#include <node.h>

#include "methods.hpp"

using namespace v8;

#define CALLBACK(name) 			void name(const FunctionCallbackInfo<Value>& args)
#define ISOLATE 				v8::Isolate* isolate = v8::Isolate::GetCurrent()
#define THROW_EXCEPTION(msg) 	isolate->ThrowException(v8::Exception::TypeError(String::NewFromUtf8(isolate, msg)))
#define UTF8_STRING(str) 		String::NewFromUtf8(isolate, str)


//Method declarations
namespace API{	
	/**
	 * List installed printers
	 * @return array of printers 
	 */
	CALLBACK(listPrinters);
	
	/**
	 * Returns printer infos with active jobs
	 * @param printer printer name; if it's null it'll be used default printer name
	 * @return object containing infos
	 */
	CALLBACK(printerInfo);

	/**
	 * List CUPS options and PPD options
	 * @param printer printer name
	 * @return object containing options
	 */
	CALLBACK(printerOptions);
	
	/**
	 * Returns default printer name
	 */
	CALLBACK(defaultPrinterName);

	/**
	 * Prints file or a plain text passed directly to this method.
	 * If a text is passed to JS function, it will be created a txt file from where to print text.
	 * @param filename
	 */
	CALLBACK(print);
}

using namespace API;

void init(Local<Object> exports) {
	NODE_SET_METHOD(exports, "listPrinters", listPrinters);
	NODE_SET_METHOD(exports, "printerInfo", printerInfo);
	NODE_SET_METHOD(exports, "printerOptions", printerOptions);
	NODE_SET_METHOD(exports, "defaultPrinterName", defaultPrinterName);
	NODE_SET_METHOD(exports, "print", print);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)
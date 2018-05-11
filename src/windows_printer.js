const dotenv = require('dotenv');
const fs = require('fs');
const os = require('os');

dotenv.config({path: fs.realpathSync(__dirname + '/../.env')});

const edge = require(`../../${process.env.ELECTRON == 'true' ? 'electron-' : ''}edge`);
const dllPath = fs.realpathSync(__dirname + '/../lib/windows/windows_printer.dll');

module.exports = class WinPrinter{
	constructor(){
		this.printer = '';
	}

	listPrinters(){
		var list = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'ListPrinters' // This must be Func<object,Task<object>>
		});

		return new Promise((resolve, reject) => {
			list('', function(error, response){
				if(error)
					reject(error);
				else
					resolve(response);
			});
		})
	}

	defaultPrinterName() {
		var defaultName = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'DefaultPrinterName' // This must be Func<object,Task<object>>
		});

		return new Promise((resolve, reject) => {
			defaultName('', function(error, response){
				if(error)
					reject(error);
				else
					resolve(response);
			})
		});
	}

	setPrinter(printer){
		this.printer = printer;
	}

	getCurrentPrinter(){
		return this.printer;
	}

	printerInfo(printer = ''){
		var infos = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'PrinterInfo' // This must be Func<object,Task<object>>
		});

		return new Promise((resolve, reject) => {
			infos(printer || this.printer, function(error, response){
				if(error)
					reject(error);
				else
					resolve(response);
			})
		});
	}
	
	printerOptions(printer = ''){
		var getOptions = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'GetOptions' // This must be Func<object,Task<object>>
		});

		return new Promise((resolve, reject) => {
			getOptions(printer || this.printer, function(error, response){
				if(error)
					reject(error);
				else
					resolve(response);
			})
		});
	}

	print(filename = null, userOptions = {}, printer = ''){
		if(!printer){ 
			if(this.printer)
				printer = this.printer
			else
				console.warn("No printer specified. Will be used default printer");
		}

		let defaultOptions = {
			collate : true,
			duplex : "Default",
			fromPage : 0,
			toPage : 0,
			color : true,
			landscape : false,
			paperSize : '',
			printerName: printer,
			copies: 1,
			filePath: filename
		}

		let options = {};
		Object.keys(defaultOptions).forEach(value => {
			if(userOptions[value] != null || userOptions[value] != undefined)
				options[value] = userOptions[value] 
			else 
				options[value] = defaultOptions[value];
		});

		var printFile = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'Print' // This must be Func<object,Task<object>>
		});

		if(!filename)
			throw "File path not specified";

		return new Promise((resolve, reject) => {
			printFile(options, function(error, response){
				if(error)
					reject(error);
				else
					resolve(response);
			});
		})
	}

	printText(text = '', options = {}, printer = ''){
		let filepath = os.tmpdir() + "/nnp_tmp.txt";
		
		fs.writeFileSync(filepath, text);

		this.print(filepath, options, printer);
	}
}
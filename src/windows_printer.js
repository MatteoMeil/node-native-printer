const edge;
if(process.argv.includes('--electron'))
	edge = require('../../electron-edge');
else
	edge = require('../../edge');

const dllPath = '../lib/windows/windows_printer.dll';
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

		return list('', true);
	}

	defaultPrinterName() {
		var defaultName = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'DefaultPrinterName' // This must be Func<object,Task<object>>
		});

		return defaultName('', true);
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

		return infos(printer ? printer : this.printer, true);
	}
	
	printerOptions(printer = ''){
		var getOptions = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'GetOptions' // This must be Func<object,Task<object>>
		});

		return getOptions(printer ? printer : this.printer, true);
	}

	print(filename = null, userOptions = {}, printer = ''){
		let defaultOptions = {
			collate : true,
			duplex : "Default",
			fromPage : 0,
			toPage : 0,
			color : true,
			landscape : false,
			paperSize : '',
			printerName: printer || this.printer,
			copies: 1,
			filePath: fileName
		}

		let options = {};
		Object.keys(defaultOptions).forEach(value => {
			options[value] = userOptions[value] || defaultOptions[value];
		});

		var printPDF = edge.func({
			assemblyFile: dllPath,
			typeName: 'windows_printer.API',
			methodName: 'PrintPDF' // This must be Func<object,Task<object>>
		});

		if(!filename)
			throw "File path not specified";

		return printPDF(options, true);
	}

}
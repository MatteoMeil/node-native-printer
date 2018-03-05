const unixPrinter = require('../build/Release/unix_printer');
const fs = require('fs');

module.exports = class UnixPrinter{
	constructor(){
		this.printer = '';
	}

	listPrinters(){
		return unixPrinter.listPrinters();
	}

	defaultPrinterName() {
		return unixPrinter.defaultPrinterName();
	}

	setPrinter(printer){
		this.printer = printer;
	}

	getCurrentPrinter(){
		return this.printer;
	}

	printerInfo(printer = ''){
		if(!printer && !this.printer)
			console.warn("No printer specified. Will be used default printer");
		
		return unixPrinter.printerInfo(printer ? printer : this.printer);
	}
	
	printerOptions(printer = ''){
		if(!printer && !this.printer)
			console.warn("No printer specified. Will be used default printer");
		
		return unixPrinter.printerOptions(printer ? printer : this.printer);
	}

	//options refer to https://www.cups.org/doc/options.html
	print(filename = null, options = {}, printer = ''){
		if(!printer && !this.printer)
			console.warn("No printer specified. Will be used default printer");
		
		if(!filename)
			throw "No file specified";

		if(!fs.existsSync(filename))
			throw "ENOENT no such file or directory";

		filename = fs.realpathSync(filename);

		let noValueOptions = [
			'landscape',
			'fit-to-page',
			'mirror',
			'raw'
		];
		
		let optionString = '';

		let keys = Object.keys(options);
		keys.forEach(key => {
			if(noValueOptions.includes(key)){
				if(options[key])
					optionString += '-o ' + key + ' ';
			}
			else
				optionString += '-o ' + key + '=' + options[key] + ' ';
		})

		return unixPrinter.print(printer, filename, optionString);
	}
}


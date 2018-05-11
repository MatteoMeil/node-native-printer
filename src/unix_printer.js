const unixPrinter = require('../build/Release/unix_printer');
const fs = require('fs');
const os = require('os');

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
		if(!printer){ 
			if(this.printer)
				printer = this.printer
			else
				console.warn("No printer specified. Will be used default printer");
		}
		
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
			if(key == 'n')
				optionString += '-n ' + options[key] + ' ';
			else if (key == '#')
				optionString += '-#' + options[key] + ' ';				
			else{
				if(noValueOptions.includes(key)){
					if(options[key])
						optionString += '-o ' + key + ' ';
				}
				else
					optionString += '-o ' + key + '=' + options[key] + ' ';
			}
		})

		return unixPrinter.print(printer, filename, optionString);
	}

	printText(text = '', options = {}, printer = ''){
		let filepath = os.tmpdir() + "/nnp_tmp.txt";
		
		fs.writeFileSync(filepath, text);

		this.print(filepath, options, printer);
	}
}


if(process.platform == 'win32')
	module.exports = new (require('./src/windows_printer'))();
else
	module.exports = new (require('./src/unix_printer'))();
const win = require('./install_win.js');
const unix = require('./install_unix.js');

var platform = process.platform;

process.on('exit', code => {
	if (code === 1) {
		console.log("Installation aborted.\n\n");
	}
});

if(platform === 'win32'){
	win();
}
else{
	unix();
}

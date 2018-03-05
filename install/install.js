const win = require('./install_win.js');
const unix = require('./install_unix.js');

var platform = process.platform;

if(platform == 'win32'){
	win();
}
else{
	unix();
}
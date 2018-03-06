const fs = require('fs');

module.exports = function(){

	console.log(__dirname);
	if(fs.existsSync(__dirname + "/../../electron-edge"))
		makeEnv(true);
	else{
		console.warn("\nelectron-edge not found. Trying to find edge.js\n")

		if(fs.existsSync(__dirname + "/../edge"))
			makeEnv(false);
		
		else{
			console.error("edge not found\n\nInstallation aborted.\n");
			process.exit(1);
		}
	}

}

function makeEnv(electron){
	fs.writeFileSync(fs.realpathSync(__dirname + '\\..') + "\\.env", electron ? "ELECTRON=true" : "ELECTRON=false");
}
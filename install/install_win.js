const fs = require('fs');

module.exports = function(){
	
	if(fs.existsSync(__dirname + "/../../electron-edge")){
		console.log("Found electron-edge!\n");
		makeEnv(true);
	}
	else{
		console.warn("\nelectron-edge not found. Trying to find edge.js\n")

		if(fs.existsSync(__dirname + "/../../edge")){
			console.log("Found edge!\n");
			makeEnv(false);
		}
		
		else{
			console.error("edge not found\n");
			process.exit(1);
		}
	}

}

function makeEnv(electron){
	fs.writeFileSync(fs.realpathSync(__dirname + '\\..') + "\\.env", electron ? "ELECTRON=true" : "ELECTRON=false");
}
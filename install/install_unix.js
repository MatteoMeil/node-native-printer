const { spawnSync } = require('child_process');
const { spawn } = require('child_process');

module.exports = function(){
	var cmd, 
		output,
		platform = process.platform;
		
	if(platform != 'darwin'){
		cmd = spawnSync('dpkg', ['-l']);
		output = cmd.stdout.toString('utf-8');
		
		if(cmd.stderr.toString('utf-8')){
			console.log("Error: ");
			console.log(cmd.stderr.toString('utf-8'));
			process.exit(1);
		}

		if(output.includes('libcups2-dev')){
			console.log('libcups2-dev found!');
		}
		else{
			console.error('libcups2-dev is required but was not found');
			process.exit(1);
		}

	}

	cmd = spawnSync('npm', ['run', 'build']);
	
	if(output = cmd.stderr.toString('utf-8')){
		console.log(output);
		process.exit(1);
	}

	console.log(cmd.stdout.toString('utf-8'));
};

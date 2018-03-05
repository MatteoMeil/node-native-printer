const { spawnSync } = require('child_process');

module.exports = function(){

	var cmd, output;
	var electron = process.argv.includes('--electron');
	
	cmd = spawnSync(`npm list ${electron ? 'electron-' : ''}edge`);

	output = cmd.stdout.toString('utf-8');

	if(output.includes('empty')){
		console.error(`${electron ? 'electron-' : ''}edge is required but not found`);
		process.exit(1);
	}
}
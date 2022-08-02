const fs = require('fs');
const inquirer = require('inquirer');
const dotenv = require('dotenv');

module.exports = function(){

	var env = process.env.NODE_ENV || 'development';

	if (env === 'production') {
		dotenv.config({path: fs.realpathSync(__dirname + '/../.env')});
		if (process.env.NNP_PACKAGE) {
			return;
		} else {
			console.error('Installation with NODE_ENV=production requested, but NNP_PACKAGE is not set.');
			console.error('You can set NNP_PACKAGE during dev installation or in your .env file.');
			console.error('Aborting.');
			process.exit(1);
		}
	}

	var pattern = new RegExp(".*edge.*", "gi");
	var choices;

	try {
		choices = fs.readdirSync(__dirname + "/../../");
	} catch (error) {}

	if (choices) {
		choices = choices.filter(value => {
			return pattern.test(value);
		});

		choices.push('Not listed');

		inquirer.prompt([{
			type: 'list',
			name: 'package',
			message: 'Which package do you want to use?',
			choices
		}])
		.then(answer => {
			if (answer.package === 'Not listed') {
				return manuallyInsert();
			}

			makeEnv(answer.package);
		});
	} else {
		manuallyInsert();
	}

};

function makeEnv(pack) {
	fs.writeFileSync(fs.realpathSync(__dirname + '\\..') + "\\.env", `NNP_PACKAGE=${pack}`);
}

function manuallyInsert() {
	inquirer.prompt([{
		type: 'input',
		name: 'package',
		message: "Type the name of the package you want to use:\n"
	}]).then(answer => {
		makeEnv(answer.package)
	});
}

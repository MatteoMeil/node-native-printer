const fs = require('fs');
const inquirer = require('inquirer');
const dotenv = require('dotenv');

module.exports = function(){

	var args = JSON.parse(process.env.npm_config_argv).original;
	var test = new RegExp('^-.*$', 'g');

	var flags = args.filter(function(value) {
		return test.test(value);
	});

	if (flags.includes('-p') || flags.includes('--production')) {
		dotenv.config({path: fs.realpathSync(__dirname + '/../.env')});
		if (process.env.NNP_PACKAGE) {
			return;
		} else {
			console.error('It has been used flag -p but no package has been specified during dev installation.\nAborting.');
			process.exit(1);
		}
	}

	let packageSelection;
	var nnpFilter = new RegExp('^-nnp:.*$', 'g');
	let nnpFlag = flags.filter(function(value) {
		return nnpFilter.test(value);
	});

	if (nnpFlag.length > 0) {
		packageSelection = nnpFlag[0].split(':')[1];
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

		if (packageSelection) {
			let index =  choices.indexOf(packageSelection);
			if (index >= 0) {
				return makeEnv(choices[index]);
			}
		}

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

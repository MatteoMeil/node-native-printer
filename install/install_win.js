const fs = require('fs');
const inquirer = require('inquirer');

module.exports = function(){

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

function makeEnv(pack){
	fs.writeFileSync(fs.realpathSync(__dirname + '\\..') + "\\.env", "PACKAGE="+ pack);
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

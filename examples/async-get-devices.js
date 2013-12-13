var telldus = require('..');

// Async get devices
telldus.getDevices(function(r) {
	console.log('Then this! Devices returned:');
	console.log(r);
	process.exit(1);
});
console.log('This should be run first ...');

// Wait
process.stdin.resume();




var telldus = require('..');

// Async turn on
var deviceId = 1;
telldus.turnOn(deviceId,function(r) {
	console.log('Then this! (returned ' + r + ')');
	process.exit(1);
});
console.log('This should be run first ...');

// Wait
process.stdin.resume();




var telldus = require('..');

// This does not work, but i'm not sure it's supposed to work with the devices i test it on :)

// Async get and set name
var deviceId = 1,
    deviceName = "TestingDeviceName";

telldus.setName(deviceId,deviceName,function(r) {
	console.log('Then this!');
	if ( r ) {
		telldus.getName(deviceId,function(n) {
			console.log('And last but not least, this! (telldus-core returned ' + n + ' as name of ' + deviceId +')');
			process.exit(1);
		});
	} else {
		console.log( 'Name change failed...' );
		process.exit(1);
	}
});
console.log('This should be run first ...');

// Wait
process.stdin.resume();



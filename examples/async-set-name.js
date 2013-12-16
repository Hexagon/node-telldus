var telldus = require('..');

// This does not work properly, it returns success even though the name is not changed
// but i'm not sure it's supposed to work with the devices i test it on :)

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
		telldus.getName(deviceId,function(n) {
			console.log('Name change failed , name is still: ' + n);
			process.exit(1);
		});
	}
});
console.log('This should be run first ...');

// Wait
process.stdin.resume();



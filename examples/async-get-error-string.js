var telldus = require('..');

// Set to a non existing device-id to raise an error
var deviceId = 1123;

telldus.turnOn(deviceId,function(returnValue) {
	telldus.getErrorString(returnValue,function (errStr) {
		if( errStr == 'Success' ) {
			console.log(deviceId + ' is now ON');
			process.exit(0);
		} else {
			console.error('turnOn failed for device ' + deviceId + ', error: ' + errStr);
			process.exit(0);
		}
	});
});

// Wait
process.stdin.resume();



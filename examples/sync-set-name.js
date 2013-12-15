var telldus = require('..');

// This does not work, but i'm not sure it's supposed to work with the devices i test it on :)

// Sync set name
var deviceId = 1,
    deviceName = "TestingDeviceName";

var result = telldus.setNameSync(deviceId,deviceName);
if ( result ) {
	var newName = telldus.getNameSync(deviceId);
	console.log('And last but not least, this! (telldus-core returned ' + newName + ' as name of ' + deviceId +')');
} else {
	var oldName = telldus.getNameSync(deviceId);
	console.log( 'Name change failed, name is still ' + oldName );
}

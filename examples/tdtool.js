var telldus = require('..'),
	util = require('util'),

	i,
	devices, sensors;

function syntax(){
	console.log("--list|-l\t\t\t List devices");
	console.log("--on|-n <device id>\t\t Turn device on");
	console.log("--off|-f <device id>\t\t Turn device off");
	console.log("--remove|-r <device id>\t\t Remove device");
	console.log("--learn|-a <device id>\t\t Learn a new device");
	process.exit(1);
}

function checkAndSet(method, device){
	//check if TURNON is a valid thing
	if(device.methods.indexOf(method) > -1){
		console.log("%s for %s", method, device.name);
		if(method==='TURNON'){
			telldus.turnOnSync(device.id);
		}
		else if(method==='TURNOFF'){
			telldus.turnOffSync(device.id);
		}
	}
	else {
		console.error("Unsuported method, %s. %s", device.methods);
		process.exit(1);
	}
}

function removeDevice(device) {
	telldus.removeDevice(device.id, function (err) {
		if (err) {
			console.error('Could not remove device, error code: ' + err);
		}
		console.log('Device %s (%s) removed', device.id, device.name);
	});
}

function getDevice(id) {
	id = isNaN(parseInt(id, 10))  ? id: parseInt(id, 10);

	for(i=0; i<devices.length; i++){
		var d = devices[i];
		if (d.id === id) {
			return d;
		}
		else if(d.name === id){
			return d;
		}
	}

	console.error("No such device: %s", id);

	process.exit(1);
}


function parseArgs(args) {
	var i;
	for(i=2; i<args.length; i++){
		switch (args[i]){
			case '--on':
			case '-n':
				devices = telldus.getDevicesSync();
				checkAndSet('TURNON', getDevice(args[++i]));
				break;
			case '--off':
			case '-f':
				devices = telldus.getDevicesSync();
				checkAndSet('TURNOFF', getDevice(args[++i]));
				break;
			case '--list':
			case '-l':
				devices = telldus.getDevicesSync();
				console.log(devices);
				break;
			case '--sensors':
			case '-s':
				sensors = telldus.getSensors(function(err,sensors){
					console.log(util.inspect(sensors,false,null));	
				});
				break;
			case '--remove':
			case '-r':
				devices = telldus.getDevicesSync();
				removeDevice(getDevice(args[++i]));
				break;
			default:
				syntax();
				break;
		}
	}
}

if (process.argv.length <3) {
	syntax();
}

parseArgs(process.argv);









var telldus = require('..');
var i;
var devices = telldus.getDevicesSync();

var DEVICE_PARAM=3
var CMD_PARAM=2;


// FixMe: Asyncify


function syntax(){
	console.log("--list|-l\t\t\t List devices");
	console.log("--on|-n <device id>\t\t Turn device on");
	console.log("--off|-n <device id>\t\t Turn device off");
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
		console.log("Unsuported method, %s. %s", device.methods);
		process.exit(1);
	}
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
	console.log("No such device:%s", id);
	process.exit(1);
}


function parseArgs(args) {
	var i;
	for(i=2; i<args.length; i++){
		switch (args[i]){
			case '--on':
			case '-n':
				checkAndSet('TURNON', getDevice(args[++i]));
				break;
			case '--off':
			case '-f':
				checkAndSet('TURNOFF', getDevice(args[++i]));
				break;
			case '--list':
			case '-l':
				console.log(devices);
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









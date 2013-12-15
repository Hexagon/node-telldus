telldus - Node bindings for telldus-core
===

Latest release is 0.0.4, available at npm using ```npm install telldus```

---

Installation (From source):

_Note that the master branch isn't always top notch. If it doesn't compile, try an older revision or install the stable release_

1. Install telldus-core library
	* Windows, Mac: Install Telldus Center -- go [here](http://download.telldus.se/TellStick/Software/TelldusCenter/) and get the latest version of the appropriate DMG or EXE file and install
	* Linux Ubuntu/Debian: http://developer.telldus.com/wiki/TellStickInstallationUbuntu
	* Linux source install: http://developer.telldus.com/wiki/TellStickInstallationSource
2. Clone this project and enter the node-telldus directory ```cd node-telldus```
2. Compile this module ```npm install -g```
3. Link the module to your project ```cd yourprojectdirectory``` ```npm link telldus```

---

Installation (From npm):

1. Install telldus-core library
	* Windows, Mac: Install Telldus Center -- go [here](http://download.telldus.se/TellStick/Software/TelldusCenter/) and get the latest version of the appropriate DMG or EXE file and install
	* Linux Ubuntu/Debian: http://developer.telldus.com/wiki/TellStickInstallationUbuntu
	* Linux source install: http://developer.telldus.com/wiki/TellStickInstallationSource
2. Install this module using npm ```cd yourprojectdirectory``` ```npm install telldus```

---

# Basic Usage

Make sure telldusd is running on the same machine.

```javascript
var telldus = require('telldus');
var devices = telldus.getDevicesSync();
```

---

API
===

getDevicesSync
----------

Returns an array of device dictionary objects.
Only configured devices are returned.

Currently only available as a synchronous function.

Signature:

```javascript
var devices = telldus.getDevicesSync();
```

```javascript
[
  {
    id: 1,
    name: 'name from telldus.conf',
    methods: [ 'TURNON', 'TURNOFF' ],
    model: 'codeswitch',
    type: 'DEVICE',
    status: {status: 'OFF'}
  },
  ...
]
```


turnOn
------

Turns a configured device ON.

Synchronous version: ```javascript var returnValue = turnOnSync(deviceId);```

Signature:

```javascript
telldus.turnOn(deviceId,function(returnValue) {
	console.log('deviceId is now ON');
});
```

Similar to the command

```bash
tdtool --on deviceId
```


turnOff
-------

Turns a configured device OFF.

Synchronous version: ```var returnValue = turnOffSync(deviceId);```

Signature:

```javascript
telldus.turnOff(deviceId,function(returnValue) {
	console.log('Device' + deviceId + ' is now OFF');
});
```

Similar to the command

```bash
tdtool --off deviceId
```


dim
---

Dims a configured device to a certain level.

Synchronous version: ```javascript var returnValue = dimSync(deviceId,level);```

Signature:

```javascript
telldus.dim(deviceId, level,function(returnValue) {
	console.log('Device ' + deviceId + ' is now dimmed to level ' + level);
});
```


addRawDeviceEventListener
-------------------------

Add a listener for raw device events.
This is usefull for scanning for devices not yet configured

Signature:

```javascript
var listener = telldus.addRawDeviceEventListener(function(controllerId, data) {
	console.log('Raw device event: ' + data);
});
```

* `controllerId`: id of receiving controller, can identify the TellStick if several exists in the system.
* `data`: A semicolon separated string with colon separated key / value pairs.

```javascript
'class:command;protocol:arctech;model:selflearning;house:5804222;unit:2;group:0;method:turnon;'
```


addDeviceEventListener
----------------------

Add a listener for device events

Signature:

```javascript
var listener = telldus.addDeviceEventListener(function(deviceId, status) {
	console.log('Device ' + deviceId + ' is now ' + status.status);
});
```

* `status`: is an object of the form:
```
    {"status": "the status"}
```

addSensorEventListener
----------------------

Add a listener for sensor events

Signature:

```javascript
var listener = telldus.addSensorEventListener(function(deviceId,protocol,model,type,value,timestamp) {
	console.log('New sensor event received: ',deviceId,protocol,model,type,value,timestamp);
});
```


removeEventListener
-------------------

Remove a previously added listener.

Synchronous version: ```javascript var returnValue = telldus.removeEventListenerSync(listener);```
Signature:

```javascript
telldus.removeEventListener(listener,function(returnValue) {});
```


getErrorString
-------------------

Get the string representation of a return value

Synchronous version: ```javascript var errStr = telldus.getErrorStringSync(returnValue);```

Signature:

```javascript
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
```


---

License and Credits:

This project is licensed under the MIT license and is forked from telldus-core-js (https://github.com/evilmachina/telldus-core-js) by GitHub user evilmachina. 
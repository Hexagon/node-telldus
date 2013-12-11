telldus - Node bindings for telldus-core
===

Latest release is 0.0.1, available at npm using ```npm install telldus```

---

Installation (From source):

_Note that the master branch isn't always top notch. If it doesn't compile, try an older revision or install the stable release_

1. Install telldus-core library
	* Windows, Max: Install Telldus Center
	* Linux Ubuntu/Debian: http://developer.telldus.com/wiki/TellStickInstallationUbuntu
	* Linux source install: http://developer.telldus.com/wiki/TellStickInstallationSource
2. Clone this project and enter the node-telldus directory ```cd node-telldus```
2. Compile this module ```npm install -g```
3. Link the module to your project ```cd yourprojectdirectory``` ```npm link telldus```

---

Installation (From npm):

1. Install telldus-core library
	* Windows, Max: Install Telldus Center
	* Linux Ubuntu/Debian: http://developer.telldus.com/wiki/TellStickInstallationUbuntu
	* Linux source install: http://developer.telldus.com/wiki/TellStickInstallationSource
2. Install this module using npm ```cd yourprojectdirectory``` ```npm install telldus```

---

# Basic Usage

Make sure telldusd is running on the same machine.

```javascript
var tellduscore = require('telldus-core-js');
var devices = tellduscore.getDevices();
```

---

API
===

getDevices
----------

Returns an array of device dictionary objects.
Only configured devices are returned.

Signature:

```javascript
var devices = tellduscore.getDevices();
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

Signature:

```javascript
tellduscore.turnOn(deviceId);
```

Similar to the command

```bash
tdtool --on deviceId
```


turnOff
-------

Turns a configured device OFF.

Signature:

```javascript
tellduscore.turnOff(deviceId);
```

Similar to the command

```bash
tdtool --off deviceId
```


dim
---

Dims a configured device to a certain level.

Signature:

```javascript
tellduscore.dim(deviceId, level);
```


addRawDeviceEventListener
-------------------------

Add a listener for raw device events.
This is usefull for scanning for devices not yet configured

Signature:

```javascript
var listener = tellduscore.addRawDeviceEventListener(function(arg1, values, arg3, arg4, arg5) {});
```

* `arg1`, `arg3`, `arg4`, `arg5`: TODO
* `values`: A semicolon separated string with colon separated key / value pairs.

```javascript
'class:command;protocol:arctech;model:selflearning;house:5804222;unit:2;group:0;method:turnon;'
```


addDeviceEventListener
----------------------

Add a listener for device events

Signature:

```javascript
var listener = tellduscore.addDeviceEventListener(function(deviceId, status) {});
```

* `status`: is an object of the form:
```
    {"status": "the status"}
```


removeEventListener
-------------------

Remove a previously added listener.

Signature:

```javascript
tellduscore.removeEventListener(listener);
```

---

License and Credits:

This project is licensed under the MIT license and is forked from telldus-core-js (https://github.com/evilmachina/telldus-core-js) by GitHub user evilmachina. 


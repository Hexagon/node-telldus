'use strict';
var telldus = require('./build/Release/telldus');
var errors = require('./lib/errors');
var TELLSTICK_SUCCESS = 0;

//initialize the telldus library
telldus.SyncCaller(15, 0, 0, '', '');

//try to close before garbage collect
process.on('exit', function () {
	telldus.SyncCaller(16, 0, 0, '', '');
});

(function (exports, global) {

	// Async-only functions
	exports.addDeviceEventListener = function (callback) { return telldus.addDeviceEventListener(callback); };
	exports.addSensorEventListener = function (callback) { return telldus.addSensorEventListener(callback); };
	exports.addRawDeviceEventListener = function (callback) { return telldus.addRawDeviceEventListener(callback); };

	// Sync-only functions
	exports.getDevicesSync = function () { return telldus.getDevices(); };

	// Async versions
	exports.turnOn = function (id, callback) { return nodeAsyncCaller(0, id, 0, '', '', callback); };
	exports.turnOff = function (id, callback) { return nodeAsyncCaller(1, id, 0, '', '', callback); };
	exports.dim = function (id, levl, callback) { return nodeAsyncCaller(2, id, levl, '', '', callback); };
	exports.learn = function (id, callback) { return nodeAsyncCaller(3, id, 0, '', '', callback); };
	exports.addDevice = function (callback) { return nodeAsyncCaller(4, 0, 0, '', '', callback); };
	exports.setName = function (id, name, callback) { return nodeAsyncCaller(5, id, 0, name, '', callback); };
	exports.getName = function (id, callback) { return nodeAsyncCaller(6, id, 0, '', '', callback); };
	exports.setProtocol = function (id, name, callback) { return nodeAsyncCaller(7, id, 0, name, '', callback); };
	exports.getProtocol = function (id, callback) { return nodeAsyncCaller(8, id, 0, '', '', callback); };
	exports.setModel = function (id, name, callback) { return nodeAsyncCaller(9, id, 0, name, '', callback); };
	exports.getModel = function (id, callback) { return nodeAsyncCaller(10, id, 0, '', '', callback); };
	exports.getDeviceType = function (id, callback) { return nodeAsyncCaller(11, id, 0, '', '', callback); };
	exports.removeDevice = function (id, callback) { return nodeAsyncCaller(12, id, 0, '', '', callback); };
	exports.removeEventListener = function (id, callback) { return nodeAsyncCaller(13, id, 0, '', '', callback); };
	exports.getErrorString = function (id, callback) { return nodeAsyncCaller(14, id, 0, '', '', callback); };
	exports.getNumberOfDevices = function (callback) { return nodeAsyncCaller(17, 0, 0, '', '', callback); };
	exports.stop = function (id, callback) { return nodeAsyncCaller(18, id, 0, '', '', callback); };
	exports.bell = function (id, callback) { return nodeAsyncCaller(19, id, 0, '', '', callback); };
	exports.getDeviceId = function (id, callback) { return nodeAsyncCaller(20, id, 0, '', '', callback); };
	exports.getDeviceParameter = function (id, name, val, callback) { return nodeAsyncCaller(21, id, 0, name, val, callback); };
	exports.setDeviceParameter = function (id, name, val, callback) { return nodeAsyncCaller(22, id, 0, name, val, callback); };
	exports.execute = function (id, callback) { return nodeAsyncCaller(23, id, 0, '', '', callback); };
	exports.up = function (id, callback) { return nodeAsyncCaller(24, id, 0, '', '', callback); };
	exports.down = function (id, callback) { return nodeAsyncCaller(25, id, 0, '', '', callback); };

	// Sync versions
	exports.turnOnSync = function (id, callback) { return telldus.SyncCaller(0, id, 0, '', ''); };
	exports.turnOffSync = function (id, callback) { return telldus.SyncCaller(1, id, 0, '', ''); };
	exports.dimSync = function (id, levl, callback) { return telldus.SyncCaller(2, id, levl, '', ''); };
	exports.learnSync = function (id, callback) { return telldus.SyncCaller(3, id, 0, '', ''); };
	exports.addDeviceSync = function (callback) { return telldus.SyncCaller(4, 0, 0, '', ''); };
	exports.setNameSync = function (id, name, callback) { return telldus.SyncCaller(5, id, 0, name, ''); };
	exports.getNameSync = function (id, callback) { return telldus.SyncCaller(6, id, 0, '', ''); };
	exports.setProtocolSync = function (id, name, callback) { return telldus.SyncCaller(7, id, 0, name, ''); };
	exports.getProtocolSync = function (id, callback) { return telldus.SyncCaller(8, id, 0, '', ''); };
	exports.setModelSync = function (id, name, callback) { return telldus.SyncCaller(9, id, 0, name, ''); };
	exports.getModelSync = function (id, callback) { return telldus.SyncCaller(10, id, 0, '', ''); };
	exports.getDeviceTypeSync = function (id, callback) { return telldus.SyncCaller(11, id, 0, '', ''); };
	exports.removeDeviceSync = function (id, callback) { return telldus.SyncCaller(12, id, 0, '', ''); };
	exports.removeEventListenerSync = function (id, callback) { return telldus.SyncCaller(13, id, 0, '', ''); };
	exports.getErrorStringSync = function (id, callback) { return telldus.SyncCaller(14, id, 0, '', ''); };
	exports.getNumberOfDevicesSync = function (callback) { return telldus.SyncCaller(17, 0, 0, '', ''); };
	exports.stopSync = function (id, callback) { return telldus.SyncCaller(18, id, 0, '', ''); };
	exports.bellSync = function (id, callback) { return telldus.SyncCaller(19, id, 0, '', ''); };
	exports.getDeviceIdSync = function (id, callback) { return telldus.SyncCaller(20, id, 0, '', ''); };
	exports.getDeviceParameterSync = function (id, name, val, callback) { return telldus.SyncCaller(21, id, 0, name, val); };
	exports.setDeviceParameterSync = function (id, name, val, callback) { return telldus.SyncCaller(22, id, 0, name, val); };
	exports.executeSync = function (id, callback) { return telldus.SyncCaller(23, id, 0, '', ''); };
	exports.upSync = function (id, callback) { return telldus.SyncCaller(24, id, 0, '', ''); };
	exports.downSync = function (id, callback) { return telldus.SyncCaller(25, id, 0, '', ''); };


	/**
	 * Callback signature
	 *
	 * @callback requestCallback
	 * @param {Objecj|null} err - TelldusError object or null
	 * @param {...*} [args] - Different optional arguments depending on method.
	 */


	/***
	 * Nodify the response of telldus.AsyncCaller
	 * @param {number} worktype - the number of the method to execute
	 * @param {number} id - device id
	 * @param {number} num - ?
	 * @param {string} str - ?
	 * @param {requestCallback} callback - Node formated callback.
	 */
	var nodeAsyncCaller = function (worktype, id, num, str, str2, callback) {
		return telldus.AsyncCaller(worktype, id, num, str, str2, function (result) {
			var args = [];
			var rtype = typeof result;
			if (typeof callback !== 'function') {
				callback = function () {};
			}
			//did we get a number as first value?
			if (rtype === 'number') {
				//assume it represents an error code if <>0
				if (result < TELLSTICK_SUCCESS) {
					//get the description
					var description = exports.getErrorStringSync(result);
					return callback(new errors.TelldusError({code: result, message: description}));
				}
				else {
					//no error, first argument to callback should be null,
					//copy the rest from arguments
					if (result > 0) {
						//first argument is a value we want to get.
						args = [null].concat(Array.prototype.slice.call(arguments, 0));
					}
					else {
						args = [null].concat(Array.prototype.slice.call(arguments, 1));
					}
					return callback.apply(undefined, args);
				}
			}
			//was the first one a string..
			else if (rtype === 'string') {
				if (result === '') {
					return callback(new errors.TelldusError({code: result, message: 'Nothing to get!'}));
				}
				//all arguments are ok.
				args = [null].concat(Array.prototype.slice.call(arguments, 0));
				return callback.apply(undefined, args);
			}
			else if (rtype === 'boolean') {
				var e = result ? null : new errors.TelldusError({code: result, message: 'Operation failed!'});
				return callback(e);
			}
			else {
				//can't do much about it. send as is
				return callback.apply(undefined, Array.prototype.slice.call(arguments));
			}
		});
	};



})('object' === typeof module ? module.exports : (this.telldus = {}), this);


var telldus = require('./build/Release/telldus');

var TELLDUS_SUCCESS=0;

(function (exports, global) {

	// Async-only functions
	exports.addDeviceEventListener = function(callback) { return telldus.addDeviceEventListener(callback); };
	exports.addSensorEventListener = function(callback) { return telldus.addSensorEventListener(callback); };
	exports.addRawDeviceEventListener = function(callback) { return telldus.addRawDeviceEventListener(callback); };

	// Sync-only functions
	exports.getDevicesSync = function() { return telldus.getDevices(); };

	// Async versions
	exports.turnOn = function(id, callback) { return nodeAsyncCaller(0, id, 0, "", callback); };
	exports.turnOff = function(id, callback) { return nodeAsyncCaller(1, id, 0, "", callback); };
	exports.dim = function(id, levl, callback) { return nodeAsyncCaller(2, id, levl, "", callback); };
	exports.learn = function(id, callback) { return nodeAsyncCaller(3, id, 0, "", callback); };
	exports.addDevice = function(callback) { return nodeAsyncCaller(4, 0, 0, "", callback); };
	exports.setName = function(id, name, callback) { return nodeAsyncCaller(5, id, 0, name, callback); };
	exports.getName = function(id, callback) { return nodeAsyncCaller(6, id, 0, "", callback); };
	exports.setProtocol = function(id, name, callback) { return nodeAsyncCaller(7, id, 0, name, callback); };
	exports.getProtocol = function(id, callback) { return nodeAsyncCaller(8, id, 0, "", callback); };
	exports.setModel = function(id, name, callback) { return nodeAsyncCaller(9, id, 0, name, callback); };
	exports.getModel = function(id, callback) { return nodeAsyncCaller(10, id, 0, "", callback); };
	exports.getDeviceType = function(id, callback) { return nodeAsyncCaller(11, id, 0, "", callback); };
	exports.removeDevice = function(id, callback) { return nodeAsyncCaller(12, id, 0, "", callback); };
	exports.removeEventListener = function(id, callback) { return nodeAsyncCaller(13, id, 0, "", callback); };
	exports.getErrorString = function(id, callback) { return nodeAsyncCaller(14, id, 0, "", callback); };
	
	// Sync versions
	exports.turnOnSync = function(id) { return telldus.turnOn(id); };
	exports.turnOffSync = function(id) { return telldus.turnOff(id); };
	exports.dimSync = function(id, levl) { return telldus.dim(id, levl); };
	exports.learnSync = function(id) { return telldus.learn(id); };
	exports.addDeviceSync = function() { return telldus.addDevice(); };
	exports.getNameSync = function(id) { return telldus.getName(id); };
	exports.setNameSync = function(id, name) { return telldus.setName(id, name); };
	exports.getProtocolSync = function(id) { return telldus.getProtocol(id); };
	exports.setProtocolSync = function(id, protocol) { return telldus.setProtocol(id, protocol); };
	exports.getModelSync = function(id) { return telldus.getModel(id); };
	exports.setModelSync = function(id, model) { return telldus.setModel(id, model); };
	exports.getDeviceTypeSync = function(id) { return telldus.getDeviceType(id); };
	exports.removeDeviceSync = function(id) { return telldus.removeDevice(id); };
	exports.removeEventListenerSync = function(id) { return telldus.removeEventListener(id); };
	exports.getErrorStringSync = function(id) { return telldus.getErrorString(id); };

	// Not yet supported
	//  exports.setDeviceType = function(id, protocol) { return telldus.setDeviceType(id, protocol); };
	//exports.setDeviceParameterSync = function(id, name, val) { return telldus.setDeviceParameter(id, name, val); };
	//exports.getDeviceParameterSync = function(id, name, val) { return telldus.getDeviceParameter(id, name, val); };



	/**
	 * Callback signature
	 *
	 * @callback requestCallback
	 * @param {Objecj|null} err - Error object or null
	 * @param {...*} [args] - Different arguments depinging on method.
	 */


	/***
	 * Nodify the response of telldus.AsyncCaller
	 * @param {number} worktype - the number of the method to execute
	 * @param {number} id - device id
	 * @param {number} num - ?
	 * @param {string} str - ?
	 * @param {requestCallback} callback - Node formated callback.
	 */
	var nodeAsyncCaller = function (worktype, id, num, str, callback) {
		return telldus.AsyncCaller(worktype, id, num, str, function(result){
			var rtype = typeof result;
			if(typeof callback !== 'function'){
				console.log("callback is not a function", callback);
				callback = function(){};
			}

			//console.log("rtype:%j, result:%j", rtype, result);
			//did we get a number as first value?
			if(rtype === 'number'){
				//assume it represents an error code if <>0
				if(result !== TELLDUS_SUCCESS){				
					//console.log("was error");
					//get the description
					exports.getErrorString(result, function(err, description){
						//console.log("got error err:%j, desc:%j", err, description);
						if(err){
							//could not get description for this. return a generic one.
							return callback(new Error('Undefined telldus error:' + result));
						}
						//return error with description
						//console.log("returning callback");
						return callback(new Error(description));
					});
				}
				else{
					//no error, first argument to callback should be null,
					//copy the rest from arguments
					var args = [null].concat(Array.prototype.slice.call(arguments, 1));
					return callback.apply(undefined, args);
				}
			}
			//was the first one a string..
			else if (rtype === 'string'){
				if(result === ''){
					return callback(new Error('Nothing to get!'));
				}
				//all arguments are ok.
				var args = [null].concat(Array.prototype.slice.call(arguments, 0));
				return callback.apply(undefined, args);
			}
			else {
				//can't do much about it. send as is
				return callback.apply(undefined, Array.prototype.slice.call(arguments));
			}
		});
	}



})('object' === typeof module ? module.exports : (this.telldus = {}), this);


var telldus = require('./build/Release/telldus');

(function (exports, global) {

	exports.asyncTurnOn = function(id, callback) { return telldus.AsyncCaller(0, id, 0, callback); };
	exports.asyncTurnOff = function(id, callback) { return telldus.AsyncCaller(1, id, 0, callback); };
	exports.asyncDim = function(id, levl, callback) { return telldus.AsyncCaller(2, id, levl, callback); };
	
	exports.dim = function(id, levl) { return telldus.dim(id, levl); };
	
	exports.getDevices = function() { return telldus.getDevices(); };

	exports.addDevice = function() { return telldus.addDevice(); };
	exports.getName = function(id) { return telldus.getName(id); };
	exports.setName = function(id, name) { return telldus.setName(id, name); };
	exports.getProtocol = function(id) { return telldus.getProtocol(id); };
	exports.setProtocol = function(id, protocol) { return telldus.setProtocol(id, protocol); };
	exports.getModel = function(id) { return telldus.getModel(id); };
	exports.setModel = function(id, model) { return telldus.setModel(id, model); };
	exports.getDeviceType = function(id) { return telldus.getDeviceType(id); };
	//  exports.setDeviceType = function(id, protocol) { return telldus.setDeviceType(id, protocol); };
	exports.setDeviceParameter = function(id, name, val) { return telldus.setDeviceParameter(id, name, val); };
	exports.getDeviceParameter = function(id, name, val) { return telldus.getDeviceParameter(id, name, val); };
	exports.removeDevice = function(id) { return telldus.removeDevice(id); };

	exports.turnOn = function(id) { return telldus.turnOn(id); };
	exports.turnOff = function(id) { return telldus.turnOff(id); };
	exports.dim = function(id, levl) { return telldus.dim(id, levl); };
	exports.learn = function(id) { return telldus.learn(id); };
	exports.addDeviceEventListener = function(callback) { return telldus.addDeviceEventListener(callback); };
	exports.addSensorEventListener = function(callback) { return telldus.addSensorEventListener(callback); };
	exports.addRawDeviceEventListener = function(callback) { return telldus.addRawDeviceEventListener(callback); };
	exports.removeEventListener = function(id) { return telldus.removeEventListener(id); };
	exports.getErrorString = function(id) { return telldus.getErrorString(id); };

})('object' === typeof module ? module.exports : (this.telldus = {}), this);

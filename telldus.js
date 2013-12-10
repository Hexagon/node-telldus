var telldus = require('./build/Release/telldus');

(function (exports, global) {
<<<<<<< HEAD
  exports.getDevices = function() { return telldus.getDevices(); };
  exports.turnOn = function(id) { return telldus.turnOn(id); };
  exports.turnOff = function(id) { return telldus.turnOff(id); };
  exports.dim = function(id, levl) { return telldus.dim(id, levl); };
  exports.addDeviceEventListener = function(callback) { return telldus.addDeviceEventListener(callback); };
  exports.addSensorEventListener = function(callback) { return telldus.addSensorEventListener(callback); };
  exports.addRawDeviceEventListener = function(callback) { return telldus.addRawDeviceEventListener(callback); };
  exports.removeEventListener = function(id) { return telldus.removeEventListener(id); };
=======
	exports.asyncTurnOn = function(id, callback) { return tellduscore.AsyncCaller(0, id, 0, callback); };
	exports.asyncTurnOff = function(id, callback) { return tellduscore.AsyncCaller(1, id, 0, callback); };
	exports.asyncDim = function(id, levl, callback) { return tellduscore.AsyncCaller(2, id, levl, callback); };
	
	exports.dim = function(id, levl) { return tellduscore.dim(id, levl); };
	
	exports.getDevices = function() { return tellduscore.getDevices(); };

	exports.addDevice = function() { return tellduscore.addDevice(); };
	exports.getName = function(id) { return tellduscore.getName(id); };
	exports.setName = function(id, name) { return tellduscore.setName(id, name); };
	exports.getProtocol = function(id) { return tellduscore.getProtocol(id); };
	exports.setProtocol = function(id, protocol) { return tellduscore.setProtocol(id, protocol); };
	exports.getModel = function(id) { return tellduscore.getModel(id); };
	exports.setModel = function(id, model) { return tellduscore.setModel(id, model); };
	exports.getDeviceType = function(id) { return tellduscore.getDeviceType(id); };
	//  exports.setDeviceType = function(id, protocol) { return tellduscore.setDeviceType(id, protocol); };
	exports.setDeviceParameter = function(id, name, val) { return tellduscore.setDeviceParameter(id, name, val); };
	exports.getDeviceParameter = function(id, name, val) { return tellduscore.getDeviceParameter(id, name, val); };
	exports.removeDevice = function(id) { return tellduscore.removeDevice(id); };

	exports.turnOn = function(id) { return tellduscore.turnOn(id); };
	exports.turnOff = function(id) { return tellduscore.turnOff(id); };
	exports.dim = function(id, levl) { return tellduscore.dim(id, levl); };
	exports.learn = function(id) { return tellduscore.learn(id); };
	exports.addDeviceEventListener = function(callback) { return tellduscore.addDeviceEventListener(callback); };
	exports.addSensorEventListener = function(callback) { return tellduscore.addSensorEventListener(callback); };
	exports.addRawDeviceEventListener = function(callback) { return tellduscore.addRawDeviceEventListener(callback); };
	exports.removeEventListener = function(id) { return tellduscore.removeEventListener(id); };
	exports.getErrorString = function(id) { return tellduscore.getErrorString(id); };
>>>>>>> pb/master
})('object' === typeof module ? module.exports : (this.telldus = {}), this);

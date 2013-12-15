var telldus = require('./build/Release/telldus');

(function (exports, global) {

	// Async-only functions
	exports.addDeviceEventListener = function(callback) { return telldus.addDeviceEventListener(callback); };
	exports.addSensorEventListener = function(callback) { return telldus.addSensorEventListener(callback); };
	exports.addRawDeviceEventListener = function(callback) { return telldus.addRawDeviceEventListener(callback); };

	// Sync-only functions
	exports.getDevicesSync = function() { return telldus.getDevices(); };

	// Async versions
	exports.turnOn = function(id, callback) { return telldus.AsyncCaller(0, id, 0, "", callback); };
	exports.turnOff = function(id, callback) { return telldus.AsyncCaller(1, id, 0, "", callback); };
	exports.dim = function(id, levl, callback) { return telldus.AsyncCaller(2, id, levl, "", callback); };
	exports.learn = function(id, callback) { return telldus.AsyncCaller(3, id, 0, "", callback); };
	exports.addDevice = function(callback) { return telldus.AsyncCaller(4, 0, 0, "", callback); };
	exports.setName = function(id, name, callback) { return telldus.AsyncCaller(5, id, 0, name, callback); };
	exports.getName = function(id, callback) { return telldus.AsyncCaller(6, id, 0, "", callback); };
	exports.setProtocol = function(id, name, callback) { return telldus.AsyncCaller(7, id, 0, name, callback); };
	exports.getProtocol = function(id, callback) { return telldus.AsyncCaller(8, id, 0, "", callback); };
	exports.setModel = function(id, name, callback) { return telldus.AsyncCaller(9, id, 0, name, callback); };
	exports.getModel = function(id, callback) { return telldus.AsyncCaller(10, id, 0, "", callback); };
	exports.getDeviceType = function(id, callback) { return telldus.AsyncCaller(11, id, 0, "", callback); };
	exports.removeDevice = function(id, callback) { return telldus.AsyncCaller(12, id, 0, "", callback); };
	exports.removeEventListener = function(id, callback) { return telldus.AsyncCaller(13, id, 0, "", callback); };
	exports.getErrorString = function(id, callback) { return telldus.AsyncCaller(14, id, 0, "", callback); };
	
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


})('object' === typeof module ? module.exports : (this.telldus = {}), this);


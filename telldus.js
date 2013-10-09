var tellduscore = require('./build/Release/telldus-core-js');

(function (exports, global) {
  exports.getDevices = function() { return tellduscore.getDevices(); };
  
  exports.addDevice = function() { return tellduscore.addDevice(); };
  exports.setName = function(id, name) { return tellduscore.setName(id, name); };
  exports.setProtocol = function(id, protocol) { return tellduscore.setProtocol(id, protocol); };
  exports.setModel = function(id, model) { return tellduscore.setModel(id, model); };
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
})('object' === typeof module ? module.exports : (this.telldus = {}), this);
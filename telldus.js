var telldus = require('./build/Release/telldus');

(function (exports, global) {
  exports.getDevices = function() { return telldus.getDevices(); };
  exports.turnOn = function(id) { return telldus.turnOn(id); };
  exports.turnOff = function(id) { return telldus.turnOff(id); };
  exports.dim = function(id, levl) { return telldus.dim(id, levl); };
  exports.addDeviceEventListener = function(callback) { return telldus.addDeviceEventListener(callback); };
  exports.addSensorEventListener = function(callback) { return telldus.addSensorEventListener(callback); };
  exports.addRawDeviceEventListener = function(callback) { return telldus.addRawDeviceEventListener(callback); };
  exports.removeEventListener = function(id) { return telldus.removeEventListener(id); };
})('object' === typeof module ? module.exports : (this.telldus = {}), this);

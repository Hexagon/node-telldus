var tellduscore = require('./build/Release/telldus-core-js');

(function (exports, global) {
  exports.getDevices = function() { return tellduscore.getDevices(); };
  exports.turnOn = function(id) { return tellduscore.turnOn(id); };
  exports.turnOff = function(id) { return tellduscore.turnOff(id); };
  exports.dim = function(id, levl) { return tellduscore.dim(id, levl); };
  exports.addDeviceEventListener = function(callback) { return tellduscore.addDeviceEventListener(callback); };
  exports.addSensorEventListener = function(callback) { return tellduscore.addSensorEventListener(callback); };
  exports.addRawDeviceEventListener = function(callback) { return tellduscore.addRawDeviceEventListener(callback); };
  exports.removeEventListener = function(id) { return tellduscore.removeEventListener(id); };
})('object' === typeof module ? module.exports : (this.telldus = {}), this);
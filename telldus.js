var tellduscore = require('./build/Release/telldus');

(function (exports, global) {
  exports.getDevices = function() { return tellduscore.getDevices(); };
  exports.turnOn = function(id) { return tellduscore.turnOn(id); };
  exports.turnOff = function(id) { return tellduscore.turnOff(id); };
  exports.dim = function(id, lvel) { return tellduscore.dim(id, levl); };
})('object' === typeof module ? module.exports : (this.telldus = {}), this);
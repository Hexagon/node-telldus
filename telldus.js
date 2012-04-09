var tellduscore = require('./build/Release/telldus');

(function (exports, global) {
  exports.getDevices = function() { return tellduscore.getDevices(); };
})('object' === typeof module ? module.exports : (this.telldus = {}), this);
'use strict';
var util = require('util');

var errors = module.exports = {};


errors.TelldusError = function (options) {
	this.name = 'TelldusError';
	
	this.message = options.message || 'Undefined telldus Error';
	this.code = options.code;

	Error.captureStackTrace(this, errors.TelldusError);
};

util.inherits(errors.TelldusError, Error);
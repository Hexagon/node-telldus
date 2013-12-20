/*global describe, it, before, after */
var should = require('should');
var telldus = require('..');
var utils = require('./utils');


/* 
 * for this test to work there must be a running
 * telldusd and the first device must be a switch with 
 * TURNON,TURNOFF 
*/
/* example of a working config for these tests

user = 'nobody'
group = 'plugdev'
ignoreControllerConfirmation = 'false'
device {
  id = 1
  name = 'Example device'
  protocol = 'arctech'
  model = 'codeswitch'
  parameters {
    house = 'A'
    unit = '1'
  }
}
device {
  id = 2
  name = 'Nexa LKCT-614 Remote'
  protocol = 'arctech'
  model = 'selflearning'
  parameters {
    house = '11790353'
    unit = '1'
  }
}

*/

describe('async methods', function () {


  before(function () {
    this.devices = telldus.getDevicesSync();
  });


  after(function(){
    utils.cleanUp(this.devices);
  });


  describe('config related', function () {
  
    
    var deviceId;


    before(function (done) {
      telldus.addDevice(function(err, num){
        should.not.exist(err);
        num.should.be.above(2);
        deviceId = num;
        done();
      });
    });


    it('addDevice', function (done) {
      telldus.addDevice(function (err, id) {
        should.not.exist(err);
        should.exist(id);
        id.should.be.above(0);
        done();
      });
    });


    it('getName with error on bad device', function (done) {
      telldus.getName(utils.NON_EXISTING_DEVICE, function (err, name) {
        should.exist(err);
        err.should.be.an.instanceOf(telldus.errors.TelldusError);
        err.should.have.property('message', 'Device not found');
        err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_DEVICE_NOT_FOUND);
        should.not.exist(name);
        done();
      });
    });

    it('getName with new device', function (done) {
      telldus.getName(deviceId, function (err, name) {
        should.not.exist(err);
        name.should.equal('');
        done();
      });
    });
    

    it('getName', function (done) {
      telldus.setNameSync(deviceId, 'Newly created3');
      telldus.getName(deviceId, function (err, name) {
        should.not.exist(err);
        name.should.equal('Newly created3');
        done();
      });
    });


    it('setName on bad device', function (done) {
      telldus.setName(utils.NON_EXISTING_DEVICE, 'Newly created2', function (err) {
        err.should.be.an.instanceOf(telldus.errors.TelldusError);
        err.should.have.property('message', 'Unknown error');
        err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_UNKNOWN);
        done();
      });
    });


    it('setName', function (done) {
      telldus.setName(deviceId, 'Newly created2', function (err) {
        should.not.exist(err);
        validate();
      });

      function validate() {
        var name = telldus.getNameSync(deviceId);
        name.should.equal('Newly created2', 'setName did not fail but can not get new values');
        done();
      }
    });


    it('getProtocol on new device', function (done) {
      telldus.getProtocol(deviceId, function (err, p) {
        should.not.exist(err);
        p.should.equal('');
        done();
      });
    });


    it('setProtocol', function (done) {
      telldus.setProtocol(deviceId, utils.VALID_PROTOCOLS[0], function (err) {
        should.not.exist(err, 'setProtocol failed');

        var p = telldus.getProtocolSync(deviceId);
        p.should.equal(utils.VALID_PROTOCOLS[0], 'setProtocol did not fail but can not get new values');

        done(err);
      });
    });
    
  });//config related


  describe('switches', function () {


    it('turnOff', function (done) {
      var device = this.devices[0];
      telldus.turnOff(device.id, function (err) {
        should.not.exist(err);
        //refresh
        device = telldus.getDevicesSync()[0];
        device.status.should.have.property('name', 'OFF');
        done();
      });
    });


    it('turnOn', function (done) {
      var device = this.devices[0];
      telldus.turnOn(device.id, function (err) {
        should.not.exist(err);
        //refresh
        device = telldus.getDevicesSync()[0];
        device.status.should.have.property('name', 'ON');
        done();
      });
    });


    it('turnOn not existing should generate err', function (done) {
      telldus.turnOn(utils.NON_EXISTING_DEVICE, function (err) {
        should.exist(err);
        err.should.have.property('message');
        err.message.should.be.equal('Device not found');
        done();
      });
    });
  });//switches


  
});
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


    it('getDevices', function(done){
      telldus.getDevices(function (err, devices) {
        should.not.exist(err);
        devices.should.be.instanceOf(Array);
        devices.length.should.be.above(2);
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


    it('getModel', function (done) {
      telldus.getModel(1, function (err, result) {
        result.should.not.equal('UNKNOWN');
        result.should.not.equal('');
        utils.VALID_MODELS.should.include(result);
        done(err);
      });
    });


    it('getModel with bad device and get UNKNOWN', function (done) {
      telldus.getModel(utils.NON_EXISTING_DEVICE, function (err, result) {
        should.not.exist(result);
        err.should.be.an.instanceOf(telldus.errors.TelldusError);
        err.should.have.property('message', 'Device not found');
        err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_DEVICE_NOT_FOUND);
        done();
      });
    });


    it('getModel with new device and get ""', function (done) {
      telldus.getModel(deviceId, function (err, result) {
        result.should.equal('');
        done(err);
      });
    });


    it('setModel with bad device and get err', function (done) {
      telldus.setModel(utils.NON_EXISTING_DEVICE, utils.VALID_MODELS[0], function (err) {
        err.should.be.an.instanceOf(telldus.errors.TelldusError);
        err.should.have.property('message', 'Unknown error');
        err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_UNKNOWN);
        done();
      });
    });


    it('setModel with bad model', function (done) {
      var modelname = 'SHOULD WE ALLOW IT?';
      telldus.setModel(deviceId, modelname, function (err) {
        var r = telldus.getModelSync(deviceId);
        r.should.equal(modelname);
        done(err);
      });
    });


    it('setModel', function (done) {
      telldus.setModel(deviceId, utils.VALID_MODELS[0], function (err) {
        should.not.exist(err);
        var r = telldus.getModelSync(deviceId);
        r.should.equal(utils.VALID_MODELS[0]);
        done(err);
      });
    });


    it('getDeviceType with bad device and get err', function (done) {
      telldus.getDeviceType(utils.NON_EXISTING_DEVICE, function(err, result) {
        should.not.exist(result);
        err.should.be.an.instanceOf(telldus.errors.TelldusError);
        err.should.have.property('message', 'Device not found');
        err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_DEVICE_NOT_FOUND);
        done();
      });
    });


    it('getDeviceType', function (done) {
      telldus.getDeviceType(1, function(err, result) {
        result.should.be.within(1, 3);
        done(err);
      });
    });


    it('getDeviceParameter', function(done) {
      telldus.getDeviceParameter(deviceId, 'house', 'testdefault', function (err, result){
        result.should.equal('testdefault');
        done(err);
      });
    });


    it('setDeviceParameter', function(done) {
      telldus.setDeviceParameter(deviceId, 'house', '12345', callback);
      function callback(err) {
        done(err);
      }
    });


    it('getNumberOfDevices', function (done) {
      telldus.getNumberOfDevices(function (err, result) {
        should.not.exist(err);
        result.should.be.within(1, 50);
        done();
      });
      
    });

    it('getDeviceId', function (done) {
      var result = telldus.getDeviceId(0, function(err, result){
        should.not.exist(err);
        result.should.equal(1);
        next();
      });
      
      function next(){
        result = telldus.getDeviceId(9999, function(err, result){
          should.not.exist(result);
          err.should.be.an.instanceOf(telldus.errors.TelldusError);
          err.should.have.property('message', 'Device not found');
          err.should.have.property('code', telldus.enums.status.TELLSTICK_ERROR_DEVICE_NOT_FOUND);
          done();
        });
      }
    });


    it('removeDevice', function (done) {
      telldus.removeDevice(deviceId, function (err) {
        should.not.exist(err);
        var devices = telldus.getDevicesSync();
        for(var i=0; i<devices.length; i++){
          devices[i].id.should.not.equal(deviceId);
        }
        done();
      });
    });
  });//config related


  describe('actions', function () {

    var dimmerId;

    before(function (){
      //create a dimmer device
      dimmerId = utils.addDimmerSync();
    });


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


    it('dim should dim', function(done) {
      telldus.dim(dimmerId, 75, function(err){
        should.not.exist(err);
        validate();

      });
      function validate(){
        var devices = telldus.getDevicesSync();
        var found=false;
        for (var i=0; i<devices.length; i++){
          if (devices[i].id === dimmerId) {
            var d = devices[i];
            d.status.should.have.property('name', 'DIM');
            d.status.should.have.property('level', 75);
            found=true;
          }
        }
        found.should.be.equal(true);
        done();
      }
    });

    it('learn should learn...', function(done){
      telldus.learn(dimmerId, function(err){
        //TODO:how should this be validated
        done(err);
      });

    });

  });//switches


  
});
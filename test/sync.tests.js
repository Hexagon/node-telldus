/*global describe, it, before, after */
var should = require('should');
var telldus = require('..');
var utils = require('./utils');


describe('sync methods', function(){

  before(function () {
    this.devices = telldus.getDevicesSync();
  });


  after(function(){
    utils.cleanUp(this.devices);
  });

  describe('config related', function(){
   
    var deviceId;

    before(function () {
      deviceId = telldus.addDeviceSync();
    });


    it('getDevicesSync', function () {
      var devices = telldus.getDevicesSync();
      
      //if devices is zero length then 
      //telldusd is probably not running
      devices.length.should.be.above(0);
      
      //set the environment variable VERBOSE to something if 
      //you want to output the devices
      if (typeof process.env.VERBOSE !== 'undefined') {
        console.log(devices);
      }

      var dev1 = devices[0];
      dev1.should.have.property('name');
      dev1.should.have.property('methods');
      dev1.should.have.property('model');
      dev1.should.have.property('type');
      dev1.should.have.property('status');
      //test status
      dev1.status.should.have.property('name');

      //test methods
      var methods = dev1.methods;
      methods.should.be.an.instanceOf(Array);
      methods.should.include('TURNON');
      methods.should.include('TURNOFF');
    });


    it('addDeviceSync', function () {
      var id = telldus.addDeviceSync();
      should.exist(id);
      id.should.be.above(0);
    });


    it('getNameSync', function () {
      var result = telldus.getNameSync(1);
      result.should.not.equal('');
      result.should.not.equal('UNKNOWN');
    });

    
    it('getNameSync with new device and return ""', function () {
      var name = telldus.getNameSync(deviceId);
      name.should.equal('');
    });


    it('getNameSync with bad device and return UNKNOWN', function () {
      var result = telldus.getNameSync(utils.NON_EXISTING_DEVICE);
      result.should.equal('UNKNOWN');
    });


    it('setNameSync', function () {
      var setResult = telldus.setNameSync(deviceId, 'Newly created');
      var name = telldus.getNameSync(deviceId);
      name.should.equal('Newly created');
      setResult.should.equal(true, 'set worked but did still return error');
    });
    

    it('setNameSync with bad device and return false', function () {
      var setResult = telldus.setNameSync(utils.NON_EXISTING_DEVICE, 'Bad ! ');
      setResult.should.equal(false);
    });


    it('getProtocolSync', function () {
      var p = telldus.getProtocolSync(1);
      p.should.not.equal('');
      p.should.not.equal('UNKNOWN');
      utils.VALID_PROTOCOLS.should.include(p);
    });


    it('getProtocolSync with bad device and return UNKNOWN', function () {
      var p = telldus.getProtocolSync(utils.NON_EXISTING_DEVICE);
      p.should.equal('UNKNOWN'); //newly created device does not have a name.
    });


    it('getProtocolSync with new device and return ""', function () {
      var p = telldus.getProtocolSync(deviceId);
      p.should.equal(''); //newly created device does not have a name.
    });


    it('setProtocolSync', function () {
      var result = telldus.setProtocolSync(deviceId, utils.VALID_PROTOCOLS[0]);
      result.should.be.equal(true);
      var p = telldus.getProtocolSync(deviceId); //now it should have
      p.should.equal(utils.VALID_PROTOCOLS[0]);
    });


    it('getModelSync', function () {
      var result = telldus.getModelSync(1);
      result.should.not.equal('UNKNOWN');
      result.should.not.equal('');
      utils.VALID_MODELS.should.include(result);
    });


    it('getModelSync with bad device and get UNKNOWN', function () {
      var result = telldus.getModelSync(utils.NON_EXISTING_DEVICE);
      result.should.equal('UNKNOWN');
    });


    it('getModelSync with new device and get ""', function () {
      var result = telldus.getModelSync(deviceId);
      result.should.equal('');
    });


    it('setModelSync', function () {
      var r = telldus.setModelSync(deviceId, utils.VALID_MODELS[0]);
      r.should.equal(true);
      r = telldus.getModelSync(deviceId);
      r.should.equal(utils.VALID_MODELS[0]);
    });


    it('getDeviceTypeSync', function () {
      var r = telldus.getDeviceTypeSync(1);
      r.should.be.within(1, 3);
    });


    it('getDeviceParameterSync', function() {
      var result = telldus.getDeviceParameterSync(deviceId, 'house', 'testdefault');
      result.should.equal('testdefault');
    });


    it('setDeviceParameterSync', function() {
      var result = telldus.setDeviceParameterSync(deviceId, 'house', '12345');
      result.should.equal(true);
    });


    it('getNumberOfDevicesSync', function () {
      var result = telldus.getNumberOfDevicesSync();
      result.should.be.within(this.devices.length, this.devices.length + 3);
    });


    it('getDeviceIdSync', function(){
      var result = telldus.getDeviceIdSync(0);
      result.should.equal(1);

      result = telldus.getDeviceIdSync(9999);
      result.should.equal(-1);
    });


    it('removeDeviceSync', function () {
      var r = telldus.removeDeviceSync(deviceId);
      r.should.be.equal(true);
    });


  });//config related


  describe('actions', function(){
    
    var dimmerId;

    before(function (){
      //create a dimmer device
      dimmerId = utils.addDimmerSync();
    });


    it('turnOffSync', function () {
      var device = this.devices[0];
      var returnValue = telldus.turnOffSync(device.id);
      returnValue.should.be.equal(0);
      //refresh
      device = telldus.getDevicesSync()[0];
      device.status.should.have.property('name', 'OFF');
    });


    it('turnOnSync', function () {
      var device = this.devices[0];
      var returnValue = telldus.turnOnSync(device.id);
      returnValue.should.be.equal(0);
      //refresh
      device = telldus.getDevicesSync()[0];
      device.status.should.have.property('name', 'ON');
    });


    it('dimSync should dim', function() {
      telldus.dimSync(dimmerId, 75);
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
    });

    it('learn should learn...', function(){
      var result = telldus.learnSync(dimmerId);
      result.should.equal(telldus.enums.status.TELLSTICK_SUCCESS);

    });

  });
  

  describe('support events', function () {
    
    it('deviceEventListener', function (done) {
      var seconds = 5; // for how many seconds should we wait for an aevent
      this.timeout(seconds * 1000 + 1000);
      var count = 0;
      
      //listen and wait for something
      var listener = telldus.addDeviceEventListener(function (deviceId, evt) {
        if (typeof process.env.VERBOSE !== 'undefined') {
          console.log('device:%s, event:%j', deviceId, evt);
        }
        deviceId.should.be.above(0);
        evt.should.have.property('name', 'ON');
        count++;
      });
      
      listener.should.be.above(0);

      //send a device event
      telldus.turnOn(1);

      setTimeout(function () {
        var returnValue = telldus.removeEventListenerSync(listener);
        var msg = telldus.getErrorStringSync(returnValue);
        returnValue.should.equal(0, 'removeEventListenerSync failed with "' + msg + '"');
        //we should have 1 event
        count.should.be.equal(1);
        done(); //consider the test done
      }, seconds * 1000);
    });//deviceEventListener
  });//describe events

});
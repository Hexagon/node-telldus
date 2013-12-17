/*global describe, it, before */
/*jshint laxcomma:true, node:true */
"use strict";
var assert = require('assert')
  , should = require('should')
  , util = require('util');


var telldus = require('..');
var errors = require('../lib/errors');

var SOME_REALLY_BIG_NUMBER = 100;
var NON_EXISTING_DEVICE=999;

var VALID_PROTOCOL='arctech';


/* 
 * for this test to work there must be a running
 * telldusd and the first device must be a switch with 
 * TURNON,TURNOFF 
*/
/* example of a working config for these tests

user = "nobody"
group = "plugdev"
ignoreControllerConfirmation = "false"
device {
  id = 1
  name = "Example device"
  protocol = "arctech"
  model = "codeswitch"
  parameters {
    house = "A"
    unit = "1"
  }
}
device {
  id = 2
  name = "Nexa LKCT-614 Remote"
  protocol = "arctech"
  model = "selflearning"
  parameters {
    house = "11790353"
    unit = "1"
  }
}

*/


/*
 * Used to decode the values from rawDeviceEventListener
 */
function parseRaw(data) {
  var result={};
  var pairs = data.split(';');
  //all pairs end with ; including last
  for(var i = 0; i < pairs.length-1; i++) {
    var keyval = pairs[i].split(':');
    result[keyval[0]] = keyval[1];
  }
  return result;
}


describe("telldus library should", function () {


  before(function () {
    this.devices = telldus.getDevicesSync();
  });


  after(function(){
    //if you do not wan't cleanup of any created devices
    //in tellstick.conf uncomment the next line
    //return;

    var id, i, j, d, found, newDeviceList;
    newDeviceList = telldus.getDevicesSync();

    //remove all devices that weren't there to 
    //begin with
    for(i=0; i<newDeviceList.length;i++){
      d = newDeviceList[i];
      found=false;
      for(j=0; j<this.devices.length; j++) {
        if(d.id === this.devices[j].id){
          //console.log("%s existed", d.id);
          found=true;
        }
      }
      if(!found){
        //console.log("%s is to be removed", d.id);
        if(d.id>0){
          try {
            telldus.removeDeviceSync(d.id);
            //console.log("%s is removed", d.id);
          }
          catch (ex){
            console.log("Could not remove device %s, %s", d.id, ex);

          }
        }
      }
    }
  });


  it("getDevicesSync", function () {
    var devices = this.devices;
    
    //if devices is zero length then 
    //telldusd is probably not running
    devices.length.should.be.above(0);
    
    //set the environment variable VERBOSE to something if 
    //you want to output the devices
    if(typeof process.env['VERBOSE'] != 'undefined'){
      console.log(devices);
    }

    var dev1 = devices[0];
    dev1.should.have.property('name');
    dev1.should.have.property('methods');
    dev1.should.have.property('model');
    dev1.should.have.property('type');
    dev1.should.have.property('status');
    //test status
    dev1.status.should.have.property('status');

    //test methods
    var methods = dev1.methods;
    methods.should.be.an.instanceOf(Array);
    methods.should.include('TURNON');
    methods.should.include('TURNOFF');
  });


  describe('with config be able to', function(){
  
    describe('sync', function(){
      var deviceId;

      before(function(){
        deviceId = telldus.addDeviceSync();
      });


      it('addDeviceSync', function(){
        var id = telldus.addDeviceSync();
        id.should.be.above(0);
      });


      it('getNameSync', function(){
        var name = telldus.getNameSync(deviceId);
        name.should.equal('');
      });
      

      it('setNameSync', function(){
        var setResult = telldus.setNameSync(deviceId, 'Newly created');
        var name = telldus.getNameSync(deviceId);
        name.should.equal('Newly created');
        setResult.should.equal(true, 'set worked but did still return error');
      });


      it('getProtocolSync', function(){
        var p = telldus.getProtocolSync(deviceId);
        p.should.equal(''); //newly created device does not have a name.
      });


      it('setProtocolSync', function () {
        var result = telldus.setProtocolSync(deviceId, VALID_PROTOCOL);
        result.should.be.true;
        var p = telldus.getProtocolSync(deviceId); //now it should have
        p.should.equal(VALID_PROTOCOL);
      });

    }); //sync


    describe('async', function(){
      var deviceId;


      before(function(){
         deviceId = telldus.addDeviceSync();
      });


      it('addDevice', function(done){
        telldus.addDevice(function(err, id){
          should.not.exist(err);
          should.exist(id);
          id.should.be.above(0);
          done();
        });
      });


      it('getName with proper error', function(done){
        telldus.getName(deviceId, function(err, name){
          should.exist(err);
          err.should.be.an.instanceOf(errors.TelldusError);
          err.should.have.property('message', 'Nothing to get!');
          err.should.have.property('code', '');
          done();
        });
      });
      

      it('setName', function(done){
        telldus.setName(deviceId, 'Newly created2', function(err){
          should.not.exist(err);
          validate();
        });

        function validate() {
          var name = telldus.getNameSync(deviceId);
          name.should.equal('Newly created2', 'setName did not fail but can not get new values');
          done();
        }
      });


      it('getName', function(done){
        telldus.setNameSync(deviceId, 'Newly created3');
        telldus.getName(deviceId, function(err, name){
          should.not.exist(err);
          name.should.equal('Newly created3');
          done();
        });
      });


      it('getProtocol', function(done){
        telldus.getProtocol(deviceId, function(err, p){
          should.exist(err);
          err.should.have.property('message', 'Nothing to get!');
          should.not.exist(p);
          done();
        });
      });


      it('setProtocol', function (done) {
        telldus.setProtocol(deviceId, VALID_PROTOCOL, function(err){
          should.not.exist(err, "setProtocol failed");

          var p = telldus.getProtocolSync(deviceId);
          p.should.equal(VALID_PROTOCOL, 'setProtocol did not fail but can not get new values');

          done(err);
        });
      });

    }); //async
    
  });//end with a device


  describe.skip("support switches", function(){


    it('turnOff', function(done) {    
      var device = this.devices[0];
      telldus.turnOff(device.id, function(err){
        should.not.exist(err);
        //refresh
        device = telldus.getDevicesSync()[0];
        device.status.should.have.property('status', 'OFF');  
        done();
      });    
    });


    it('turnOffSync', function() {
      var device = this.devices[0];
      var returnValue = telldus.turnOffSync(device.id);
      returnValue.should.be.equal(0);
      //refresh
      device = telldus.getDevicesSync()[0];
      device.status.should.have.property('status', 'OFF');  
    });


    it('turnOn', function(done) {
      var device = this.devices[0];
      telldus.turnOn(device.id, function(err){
        should.not.exist(err);
        //refresh
        device = telldus.getDevicesSync()[0];
        device.status.should.have.property('status', 'ON');
        done();
      });    
    });


    it('turnOnSync', function() {
      var device = this.devices[0];
      var returnValue = telldus.turnOnSync(device.id);
      returnValue.should.be.equal(0);
      //refresh
      device = telldus.getDevicesSync()[0];
      device.status.should.have.property('status', 'ON');
    });


    it('turnOn not existing should generate err', function(done){
      telldus.turnOn(NON_EXISTING_DEVICE,function(err){
        should.exist(err);
        err.should.have.property('message');
        err.message.should.be.equal("Device not found");
        done();
      });
    });
  });//switches


  describe('support events', function () {
    /*
     * For getting these tests to work something will have
     * to generate at least 1 event during each test.
     * That could be you pushing a remote.
     */
    it("using rawDeviceEventListener", function (done) {
      var seconds = 5; //for how many seconds should we wait for an event
      console.log("\nWaiting", seconds, "seconds for some raw events.\nPlease trigger something.");
      var received = [];
      this.timeout(seconds*1000+1000); //increase the test timeout
      
      var listener = telldus.addRawDeviceEventListener(function (controllerId, data) {
        arguments.length.should.be.equal(2);
        should.exist(controllerId);
        should.exist(data);
        //set the environment variable VERBOSE to something if 
        //you want to output the received data
        if(typeof process.env['VERBOSE'] !== 'undefined' ){
          console.log(data);
        }
        //do some tests
        data.should.be.type('string');
        if(! (data.indexOf(';') >= 0 && data.indexOf(':') >= 0)) {
          //need to filter some junk
          return;
        }
        //data.should.include(':');
        //data.should.include(';');
        data.length.should.be.within(20, SOME_REALLY_BIG_NUMBER);
        
        //save it
        received.push(data);
        //parse the string
        var parsed = parseRaw(data);
        parsed.should.have.property('class');
        parsed.should.have.property('protocol');
        
      });

      listener.should.be.above(0);
      
      setTimeout(function () {
        telldus.removeEventListener(listener, function(err){
          done(err); //consider the test done    
        }); //remove the listener
        received.length.should.be.above(0); //we should have gotten at least 1 event. Likely more
      
      },seconds*1000);
    
    });//it should listen


    it("deviceEventListener", function (done) {
      var seconds = 2; // for how many seconds should we wait for an aevent
      this.timeout(seconds * 1000 + 1000);
      var count = 0;
      
      //listen and wait for something
      var listener = telldus.addDeviceEventListener( function (deviceId, evt) {
        if(typeof process.env['VERBOSE'] !== 'undefined' ){
          console.log("device:%s, event:%j", deviceId, evt);
        }
        deviceId.should.be.above(0);
        evt.should.have.property('status', 'ON'); 
        count++;
      });
      
      listener.should.be.above(0);

      //send a device event
      telldus.turnOn(1);

      setTimeout(function () {
        var returnValue = telldus.removeEventListenerSync(listener);
        var msg = telldus.getErrorStringSync(returnValue);
        returnValue.should.equal(0, "removeEventListenerSync failed with '" + msg + "'"  );
        //we should have 1 event
        count.should.be.equal(1);
        done(); //consider the test done
      },seconds * 1000);
    });//deviceEventListener
  });//describe events
});
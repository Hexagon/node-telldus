/*global describe, it, before */
/*jshint laxcomma:true, node:true */
"use strict";
var assert = require('assert')
  , should = require('should')
  , util = require('util');

var SOME_REALLY_BIG_NUMBER = 100;
var telldus = require('..');

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
    this.devices = telldus.getDevices();
  });


  it("list devices", function () {
    var devices = this.devices;
    
    //if devices is zero length then 
    //telldusd is probably not running
    devices.length.should.be.within(1, SOME_REALLY_BIG_NUMBER);
    
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

  
  it('turnOff', function() {
    var device = this.devices[0];
    telldus.turnOff(device.id);
    //refresh
    device = telldus.getDevices()[0];
    device.status.should.have.property('status', 'OFF');
  });


  it('turnOn', function() {
    var device = this.devices[0];
    telldus.turnOn(device.id);
    //refresh
    device = telldus.getDevices()[0];
    device.status.should.have.property('status', 'ON');
  });


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
        data.should.include(':');
        data.should.include(';');
        data.length.should.be.within(20, SOME_REALLY_BIG_NUMBER);
        
        //save it
        received.push(data);
        //parse the string
        var parsed = parseRaw(data);
        parsed.should.have.property('class');
        parsed.should.have.property('protocol');
        
      });
      
      setTimeout(function () {
        telldus.removeEventListener(listener); //remove the listener
        received.length.should.be.above(1); //we should have gotten at least 1 event. Likely more
        done(); //consider the test done
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
        telldus.removeEventListener(listener);
        //we should have 1 event
        count.should.be.equal(1);
        done(); //consider the test done
      },seconds * 1000);
    });//deviceEventListener
  });//describe events
});
/*global describe, it, before */
/*jshint laxcomma:true, node:true */
"use strict";
var assert = require('assert')
  , should = require('should')
  , util = require('util');


var telldus = require('..');

/*
 * Used to decode the values from rawDeviceEventListener
 */
function decode(data) {
  var result={};
  var pairs = data.split(';');
  //all pairs end with ; including last
  for(var i = 0; i < pairs.length-1; i++) {
    var keyval = pairs[i].split(':');
    result[keyval[0]] = keyval[1];
  }
  return result;
}

/* for this test to work there must be a running
 * telldusd and the first device must be a switch with 
 * TURNON,TURNOFF 
*/
describe("telldus library should", function () {
  before(function () {
    this.devices = telldus.getDevices();
  });
  it("list devices", function () {
    var devices = this.devices;
    var some_really_big_number = 100;
    //if devices is zero length then 
    //telldusd is probably not running
    devices.length.should.be.within(1, some_really_big_number);
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
      console.log("\nWaiting", seconds, "seconds for some raw events.\nPlease trigger a remote.");
      var count = 0; //counter for the events
      this.timeout(seconds*1000+1000); //increase the test timeout
      //set up a log file
      var fs = require('fs');
      var stream = fs.createWriteStream("eventdump.log");
      //when the file stream is open , go
      stream.once('open', function() {
        var listener = telldus.addRawDeviceEventListener(function (arg1, values, arg3, arg4, arg5) {
          //TODO:what is arg1 + arg3 - arg5
          var data = decode(values);
          //log the event
          stream.write(util.format((new Date()), arguments) + '\n');
          data.should.have.property('class');
          data.should.have.property('protocol');
          count++;
        });

        //after the defined time, remove listener and close stream
        setTimeout(function () {
          telldus.removeEventListener(listener); //remove the listener
          stream.end(); //close the file stream
          count.should.be.within(1,100); //we should have gotten at least 1 event.
          done(); //consider the test done
        },seconds*1000);
      });//stream open
    });//it should listen

    it("deviceEventListener", function (done) {
      var seconds = 5; // for how many seconds should we wait for an aevent
      this.timeout(seconds * 1000 + 1000);
            console.log("\nWaiting", seconds, "seconds for some raw events.\nPlease trigger a remote.");
      var count = 0;
      var listener = telldus.addDeviceEventListener( function (device, status) {
        device.should.be.within(1,100);
        status.should.have.property('status'); 
        count++;
      });
      setTimeout(function () {
        telldus.removeEventListener(listener);
        //we should have at least 1 event
        count.should.be.within(1, 100);
        done(); //consider the test done
      },seconds * 1000);
    });
  });//describe events
});
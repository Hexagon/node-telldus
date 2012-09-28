
var assert = require('assert')
  , should = require('should');


var telldus = require('../telldus');

describe("telldus wrapper", function () {

  /* for this test to work there must be a running
   * telldusd and the first device must be a switch with 
   * TURNON,TURNOFF 
   */
  it("list devices", function () {
    var devices = telldus.getDevices();
    //if devices is zero length then 
    //telldusd is probably not running
    devices.length.should.be.within(1, 50);
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
  
  it('turnOff', function() {
    var devices = telldus.getDevices();
    telldus.turnOff(devices[0].id);
    devices = telldus.getDevices();
    devices[0].status.should.have.property('name', 'OFF');
  });

  it('turnOn', function() {
    var devices = telldus.getDevices();
    telldus.turnOn(devices[0].id);
    devices = telldus.getDevices();
    devices[0].status.should.have.property('name', 'ON');
  });

});

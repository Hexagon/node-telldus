/*global describe, it, before, after */
var should = require('should');
var telldus = require('..');
var utils = require('./utils');


/*
 * For getting these tests to work something will have
 * to generate at least 1 event during each test.
 * That could be you pushing a remote.
 */
describe('with some help from you', function () {

  before(function () {
    this.devices = telldus.getDevicesSync();
  });


  after(function(){
    utils.cleanUp(this.devices);
  });


  
  it('using rawDeviceEventListener', function (done) {
      var seconds = 5; //for how many seconds should we wait for an event
      console.log('\nWaiting', seconds, 'seconds for some raw events.\nPlease trigger something.');
      var count = 0;
      this.timeout(seconds * 1000 + 1000); //increase the test timeout
      
      var listener = telldus.addRawDeviceEventListener(function (controllerId, data) {
        arguments.length.should.be.equal(2);
        should.exist(controllerId);
        should.exist(data);
        //set the environment variable VERBOSE to something if 
        //you want to output the received data
        if (typeof process.env.VERBOSE !== 'undefined') {
          console.log(data);
        }

        //do some tests
        data.should.be.type('string');
        if ((data.indexOf(';') < 0 || data.indexOf(':') < 0)) {
          //need to filter some junk
          return;
        }
        //count it
        count++;
        
        data.length.should.be.within(20, utils.SOME_REALLY_BIG_NUMBER);
        
        //parse the string
        var parsed = utils.parseRaw(data);
        parsed.should.have.property('class');
        parsed.should.have.property('protocol');
      });

      listener.should.be.above(0);
      
      setTimeout(function () {
        telldus.removeEventListener(listener, function (err) {
          done(err); //consider the test done    
        }); //remove the listener
        count.should.be.above(0); //we should have gotten at least 1 event. Likely more
      
      }, seconds * 1000);
    });//it should listen


});
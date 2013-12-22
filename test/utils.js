
var telldus = require('..');

var utils = module.exports = {
  VALID_PROTOCOLS: ['arctech'],
  VALID_MODELS: ['codeswitch','selflearning-switch', 'selflearning-dimmer'],
  SOME_REALLY_BIG_NUMBER: 1000,
  NON_EXISTING_DEVICE: 999
};


/*
 * Used to decode the values from rawDeviceEventListener
 */
utils.parseRaw = function (data) {
  var result = {};
  var pairs = data.split(';');
  //all pairs end with ; including last
  for (var i = 0; i < pairs.length - 1; i++) {
    var keyval = pairs[i].split(':');
    result[keyval[0]] = keyval[1];
  }
  return result;
};


utils.cleanUp = function (devices) {
  //if you do not wan't cleanup of any created devices
  //in tellstick.conf uncomment the next line
  //return;

  var i, j, d, found, newDeviceList;
  newDeviceList = telldus.getDevicesSync();

  //remove all devices that weren't there to 
  //begin with
  for (i = 0; i < newDeviceList.length; i++) {
    d = newDeviceList[i];
    found = false;
    for (j = 0; j < devices.length; j++) {
      if (d.id === devices[j].id) {
        //console.log('%s existed', d.id);
        found = true;
      }
    }
    if (! found) {
      //console.log('%s is to be removed', d.id);
      if (d.id > 0) {
        try {
          telldus.removeDeviceSync(d.id);
          //console.log('%s is removed', d.id);
        }
        catch (ex) {
          console.log('Could not remove device %s, %s', d.id, ex);
        }
      }
    }
  }
};


utils.addDimmerSync = function (){
  var deviceId = telldus.addDeviceSync();
  telldus.setNameSync(deviceId, 'Test Dimmer');
  telldus.setProtocolSync(deviceId, 'arctech');
  telldus.setModelSync(deviceId, 'selflearning-dimmer');
  telldus.setDeviceParameterSync(deviceId, 'house', '12345');
  telldus.setDeviceParameterSync(deviceId, 'unit', '1');

  return deviceId;
};
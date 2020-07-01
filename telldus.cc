#ifndef BUILDING_NODE_EXTENSION
  #define BUILDING_NODE_EXTENSION
#endif // BUILDING_NODE_EXTENSION

#include <cstdlib>
#include <string.h>
#include <list>

#include <node.h>
#include <v8.h>

#include <uv.h>
#include <time.h>

#include <telldus-core.h>

using namespace v8;
using namespace node;
using namespace std;

namespace telldus_v8 {

  const int DATA_LENGTH = 20;

  struct EventContext {
    Persistent<Function, CopyablePersistentTraits<Function> > callback;
    Isolate *isolate;
  };

  struct DeviceEventBaton {
    EventContext *callback;
    int deviceId;
    int lastSentCommand;
    int levelNum;
  };

  struct SensorEventBaton {
    EventContext *callback;
    int sensorId;
    char *model;
    char *protocol;
    char *value;
    int ts;
    int dataType;
  };

  struct RawDeviceEventBaton {
    EventContext *callback;
    int controllerId;
    char *data;
  };

  const int SUPPORTED_METHODS =
    TELLSTICK_TURNON
    | TELLSTICK_TURNOFF
    | TELLSTICK_BELL
    | TELLSTICK_TOGGLE
    | TELLSTICK_DIM
    | TELLSTICK_LEARN
    | TELLSTICK_EXECUTE
    | TELLSTICK_UP
    | TELLSTICK_DOWN
    | TELLSTICK_STOP;

  struct sensorValue {
    char *timeStamp;
    char *value;
  };

  struct telldusDeviceInternals {
    int supportedMethods;
    int deviceType;
    int lastSentCommand;
    int level;
    int id;
    char *name;
    char *model;
    char *protocol;
  };

  struct telldusSensorInternals {
    int sensorId;
    int dataTypes;
    char *model;
    char *protocol;
  };

  uv_mutex_t radioMutex;

  Local<Object> GetSupportedMethods(int id, int supportedMethods, Isolate* isolate){

    Local<Array> methodsObj = Array::New(isolate);

    int i = 0;

    if (supportedMethods & TELLSTICK_TURNON) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "TURNON", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_TURNOFF) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "TURNOFF", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_BELL) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "BELL", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_TOGGLE) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "TOGGLE", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_DIM) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "DIM", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_UP) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "UP", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_DOWN) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "DOWN", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_STOP) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "STOP", v8::NewStringType::kNormal).ToLocalChecked());
    if (supportedMethods & TELLSTICK_LEARN) methodsObj->Set(isolate->GetCurrentContext(), i++, String::NewFromUtf8(isolate, "LEARN", v8::NewStringType::kNormal).ToLocalChecked());

    return methodsObj;

  }

  Local<String> GetDeviceType(int id, int type, Isolate* isolate){

    if(type & TELLSTICK_TYPE_DEVICE) return String::NewFromUtf8(isolate, "DEVICE", v8::NewStringType::kNormal).ToLocalChecked();
    if(type & TELLSTICK_TYPE_GROUP) return String::NewFromUtf8(isolate, "GROUP", v8::NewStringType::kNormal).ToLocalChecked();
    if(type & TELLSTICK_TYPE_SCENE) return String::NewFromUtf8(isolate, "SCENE", v8::NewStringType::kNormal).ToLocalChecked();

    return String::NewFromUtf8(isolate, "UNKNOWN", v8::NewStringType::kNormal).ToLocalChecked();

  }

  Local<Object> GetDeviceStatus(int id, int lastSentCommand, int level, Isolate* isolate){

    Local<Object> status = Object::New(isolate);

    switch(lastSentCommand) {
      case TELLSTICK_TURNON:
        status->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "ON", v8::NewStringType::kNormal).ToLocalChecked());
        break;
      case TELLSTICK_TURNOFF:
        status->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "OFF", v8::NewStringType::kNormal).ToLocalChecked());
        break;
      case TELLSTICK_DIM:
        status->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "DIM", v8::NewStringType::kNormal).ToLocalChecked());
        status->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "level", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(isolate, level));
        break;
      default:
        status->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "UNNKOWN", v8::NewStringType::kNormal).ToLocalChecked());
    }

    return status;

  }

  Local<Object> GetDevice( telldusDeviceInternals deviceInternals, Isolate* isolate ) {

    Local<Object> obj = Object::New(isolate);

    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate,deviceInternals.name, v8::NewStringType::kNormal).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "id", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(isolate,deviceInternals.id));
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "methods", v8::NewStringType::kNormal).ToLocalChecked(), GetSupportedMethods(deviceInternals.id, deviceInternals.supportedMethods, isolate));
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "model", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate,deviceInternals.model, v8::NewStringType::kNormal).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "protocol", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate,deviceInternals.protocol, v8::NewStringType::kNormal).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), GetDeviceType(deviceInternals.id,deviceInternals.deviceType, isolate));
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "status", v8::NewStringType::kNormal).ToLocalChecked(), GetDeviceStatus(deviceInternals.id,deviceInternals.lastSentCommand,deviceInternals.level, isolate));

    // Cleanup
    tdReleaseString(deviceInternals.name);
    tdReleaseString(deviceInternals.model);
    tdReleaseString(deviceInternals.protocol);

    return obj;

  }

  Local<Value> getDevicesFromInternals( list<telldusDeviceInternals> t, Isolate* isolate ) {

    // Destination array
    Local<Array> devices = Array::New(isolate,t.size());
    int i=0;
    for (list<telldusDeviceInternals>::const_iterator iterator = t.begin(), end = t.end(); iterator != end; ++iterator) {
        devices->Set(isolate->GetCurrentContext(), i, GetDevice(*iterator, isolate));
        i++;
    }

    return devices;

  }

  telldusDeviceInternals getDeviceRaw(int idx) {

    telldusDeviceInternals deviceInternals;

    deviceInternals.id = tdGetDeviceId( idx );
    deviceInternals.name = tdGetName( deviceInternals.id );
    deviceInternals.model = tdGetModel( deviceInternals.id );
    deviceInternals.protocol = tdGetProtocol( deviceInternals.id );

    deviceInternals.supportedMethods = tdMethods( deviceInternals.id, SUPPORTED_METHODS );
    deviceInternals.deviceType = tdGetDeviceType( deviceInternals.id );
    deviceInternals.lastSentCommand = tdLastSentCommand( deviceInternals.id, SUPPORTED_METHODS );

    if( deviceInternals.lastSentCommand == TELLSTICK_DIM ) {

      char * levelStr = tdLastSentValue(deviceInternals.id);

      // Convert to number and add to object
      deviceInternals.level = atoi(levelStr);

      // Clean up the mess
      tdReleaseString(levelStr);

    }

    return deviceInternals;

  }

  sensorValue GetSensorValue(int currentType, telldusSensorInternals si) {

   sensorValue sv;
   
   char value[DATA_LENGTH];
   char timeBuf[80];
   time_t timestamp = 0;
   
   tdSensorValue(si.protocol, si.model, si.sensorId, currentType, value, DATA_LENGTH, (int *)&timestamp);

   strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", localtime(&timestamp));

   sv.timeStamp = strdup(timeBuf);
   sv.value = strdup(value);

   return sv;

  }

  Local<Object> GetSupportedSensorValues(telldusSensorInternals si, Isolate* isolate){

    Local<Array> methodsObj = Array::New(isolate);

    int i = 0;

    if (si.dataTypes & TELLSTICK_TEMPERATURE) {
        Local<Object> cur = Object::New(isolate);
        sensorValue sv =  GetSensorValue(TELLSTICK_TEMPERATURE, si);
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "TEMPERATURE", v8::NewStringType::kNormal).ToLocalChecked());
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
        methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
    }
    if (si.dataTypes & TELLSTICK_HUMIDITY) {
        Local<Object> cur = Object::New(isolate);
        sensorValue sv =  GetSensorValue(TELLSTICK_HUMIDITY, si);
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "HUMIDITY", v8::NewStringType::kNormal).ToLocalChecked());
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
        cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
        methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
    }
    #ifdef TELLSTICK_RAINRATE
      if (si.dataTypes & TELLSTICK_RAINRATE) {
          Local<Object> cur = Object::New(isolate);
          sensorValue sv =  GetSensorValue(TELLSTICK_RAINRATE, si);
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "RAINRATE", v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
          methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
      }
      if (si.dataTypes & TELLSTICK_RAINTOTAL) {
          Local<Object> cur = Object::New(isolate);
         sensorValue sv =   GetSensorValue(TELLSTICK_RAINTOTAL, si);
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "RAINTOTAL", v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
          methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
      }
      if (si.dataTypes & TELLSTICK_WINDDIRECTION) {
          Local<Object> cur = Object::New(isolate);
          sensorValue sv =  GetSensorValue(TELLSTICK_WINDDIRECTION, si);
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "WINDDIRECTION", v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
          methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
      }
      if (si.dataTypes & TELLSTICK_WINDAVERAGE) {
          Local<Object> cur = Object::New(isolate);
          sensorValue sv =  GetSensorValue(TELLSTICK_WINDAVERAGE, si);
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "WINDAVERAGE", v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
          methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
      }
      if (si.dataTypes & TELLSTICK_WINDGUST) {
          Local<Object> cur = Object::New(isolate);
          sensorValue sv =  GetSensorValue(TELLSTICK_WINDGUST, si);
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, "WINDGUST", v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.value, v8::NewStringType::kNormal).ToLocalChecked());
          cur->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "timestamp", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate, sv.timeStamp, v8::NewStringType::kNormal).ToLocalChecked());
          methodsObj->Set(isolate->GetCurrentContext(), i++, cur);
      }
    #endif

    return methodsObj;

  }

  list<telldusDeviceInternals> getDevicesRaw() {
    
    int intNumberOfDevices = tdGetNumberOfDevices();
    list<telldusDeviceInternals> deviceList;

    for ( int i = 0 ; i < intNumberOfDevices ; i++ ) {
      deviceList.push_back(getDeviceRaw(i));
    }

    return deviceList;

  }

  list<telldusSensorInternals> getSensorsRaw() {

   char protocol[DATA_LENGTH], model[DATA_LENGTH];
   int sensorId = 0, dataTypes = 0;

   list<telldusSensorInternals> sensorList;

   while(tdSensor(protocol, DATA_LENGTH, model, DATA_LENGTH, &sensorId, &dataTypes) == TELLSTICK_SUCCESS) {
     telldusSensorInternals t;
     
     t.protocol = strdup(protocol);
     t.model = strdup(model);
     t.sensorId = sensorId;
     t.dataTypes = dataTypes;

     // TODO: Cleanup of char* ?
     sensorList.push_back(t);

   }

    return sensorList;

  }

  Local<Object> GetSensor( telldusSensorInternals sensorInternals, Isolate* isolate ) {

    Local<Object> obj = Object::New(isolate);

    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "model", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate,sensorInternals.model, v8::NewStringType::kNormal).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "protocol", v8::NewStringType::kNormal).ToLocalChecked(), String::NewFromUtf8(isolate,sensorInternals.protocol, v8::NewStringType::kNormal).ToLocalChecked());
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "id", v8::NewStringType::kNormal).ToLocalChecked(), Number::New(isolate, sensorInternals.sensorId));
    obj->Set(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "data", v8::NewStringType::kNormal).ToLocalChecked(), GetSupportedSensorValues(sensorInternals, isolate));

    free(sensorInternals.model);
    free(sensorInternals.protocol);

    return obj;

  }

  Local<Value> getSensorsFromInternals( list<telldusSensorInternals> s, Isolate* isolate ) {

    // Destination array
    Local<Array> sensors = Array::New(isolate,s.size());
    int i=0;
    for (list<telldusSensorInternals>::const_iterator iterator = s.begin(), end = s.end(); iterator != end; ++iterator) {
      sensors->Set(isolate->GetCurrentContext(), i, GetSensor(*iterator, isolate));
      i++;
    }

    return sensors;

  }

  void DeviceHandleClosed(uv_handle_t* handle) {

    DeviceEventBaton *baton = static_cast<DeviceEventBaton *>(handle->data);

    delete baton;
    delete handle;
  }

  void DeviceCallbackAfter(uv_async_t *handle) {

    DeviceEventBaton *baton = static_cast<DeviceEventBaton *>(handle->data);
    
    HandleScope handleScope(baton->callback->isolate);

    Local<Function> func = Local<Function>::New(baton->callback->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)baton->callback->callback));

    Local<Value> args[] = {
      Number::New(baton->callback->isolate,baton->deviceId),
      GetDeviceStatus(baton->deviceId,baton->lastSentCommand,baton->levelNum, baton->callback->isolate),
    };

    func->Call(baton->callback->isolate->GetCurrentContext(), Null(baton->callback->isolate), 2, args);
    
    uv_close((uv_handle_t*) handle, DeviceHandleClosed);

  }

  void DeviceCallback( int deviceId, int method, const char * data, int callbackId, void* callbackVoid ) {

    DeviceEventBaton *baton = new DeviceEventBaton();

    baton->callback = static_cast<EventContext *>(callbackVoid);
    baton->deviceId = deviceId;
    baton->lastSentCommand = tdLastSentCommand(baton->deviceId, SUPPORTED_METHODS);
    baton->levelNum = 0;

    char *level;

    if(baton->lastSentCommand == TELLSTICK_DIM) {

      // Get level, returned from telldus-core as char
      level = tdLastSentValue(baton->deviceId);

      // Convert to number and add to object
      baton->levelNum = atoi(level);

      // Clean up the mess
      tdReleaseString(level);

    }

    uv_async_t *asyncDevice = new uv_async_t;
    asyncDevice->data = baton;

    // Start event loop and attach callbacks
    uv_async_init(uv_default_loop(), asyncDevice, DeviceCallbackAfter);
    uv_async_send(asyncDevice);

  }


  void addDeviceEventListener(const FunctionCallbackInfo<Value>& args){

    Isolate* isolate = args.GetIsolate();
    HandleScope handleScope(isolate);

    if (!args[0]->IsFunction()) {
      Local<Value> exception = Exception::TypeError(String::NewFromUtf8(isolate, "Expected 1 argument: (function callback)", v8::NewStringType::kNormal).ToLocalChecked());
      isolate->ThrowException(exception);
    }

    Local<Function> cb = Local<Function>::Cast(args[0]);
    Persistent<Function, CopyablePersistentTraits<Function> > value(isolate, cb);

    EventContext *ctx = new EventContext();
    ctx->callback = value;
    ctx->isolate = isolate;

    Local<Number> num = Number::New(isolate, tdRegisterDeviceEvent((TDDeviceEvent)&DeviceCallback, ctx));

    args.GetReturnValue().Set(num);

  }

  void SensorHandleClosed(uv_handle_t* handle) {

    SensorEventBaton *baton = static_cast<SensorEventBaton *>(handle->data);
    
    // Is this ok?
    free(baton->protocol);
    free(baton->model);
    free(baton->value);

    delete baton;
    delete handle;
  }

  void SensorCallbackAfter(uv_async_t* handle) {

    SensorEventBaton *baton = static_cast<SensorEventBaton *>(handle->data);

    HandleScope scope(baton->callback->isolate);
    
    Local<Function> func = Local<Function>::New(baton->callback->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)baton->callback->callback));

    Local<Value> args[] = {
      Number::New(baton->callback->isolate,baton->sensorId),
      String::NewFromUtf8(baton->callback->isolate, baton->model, v8::NewStringType::kNormal).ToLocalChecked(),
      String::NewFromUtf8(baton->callback->isolate, baton->protocol, v8::NewStringType::kNormal).ToLocalChecked(),
      Number::New(baton->callback->isolate,baton->dataType),
      String::NewFromUtf8(baton->callback->isolate, baton->value, v8::NewStringType::kNormal).ToLocalChecked(),
      Number::New(baton->callback->isolate,baton->ts)
    };

    func->Call(baton->callback->isolate->GetCurrentContext(), Null(baton->callback->isolate), 6, args);

    uv_close((uv_handle_t*) handle, SensorHandleClosed);

  }

  void SensorCallback( const char *protocol, const char *model, int sensorId, int dataType, const char *value, int ts, int callbackId, void *callbackVoid ) {

    EventContext *ctx = static_cast<EventContext *>(callbackVoid);

    SensorEventBaton *baton = new SensorEventBaton();

    baton->callback = ctx;
    baton->sensorId = sensorId;
    baton->protocol = strdup(protocol);
    baton->model = strdup(model);
    baton->ts = ts;
    baton->dataType = dataType;
    baton->value = strdup(value);

    uv_async_t *asyncSensor = new uv_async_t;
    asyncSensor->data = baton;

    // Start event loop and attach callbacks
    uv_async_init(uv_default_loop(), asyncSensor, SensorCallbackAfter);
    uv_async_send(asyncSensor);

  }

  void addSensorEventListener(const FunctionCallbackInfo<Value>& args){

    Isolate* isolate = args.GetIsolate();
    HandleScope handleScope(isolate);

    Local<Function> cb = Local<Function>::Cast(args[0]);
    Persistent<Function, CopyablePersistentTraits<Function> > value(isolate, cb);

    EventContext *ctx = new EventContext();
    ctx->callback = value;
    ctx->isolate = isolate;

    if (!args[0]->IsFunction()) {
      Local<Value> exception = Exception::TypeError(String::NewFromUtf8(isolate, "Expected 1 argument: (function callback)", v8::NewStringType::kNormal).ToLocalChecked());
      isolate->ThrowException(exception);
    }

    Local<Number> num = Number::New(isolate, tdRegisterSensorEvent((TDSensorEvent)&SensorCallback, ctx));

    args.GetReturnValue().Set(num);
    
  }

  void RawDataHandleClosed(uv_handle_t* handle) {

    RawDeviceEventBaton *baton = static_cast<RawDeviceEventBaton *>(handle->data);
    
    // Is this ok?
    free(baton->data);

    delete baton;
    delete handle;
  }

  void RawDataCallbackAfter(uv_async_t* handle) {

    RawDeviceEventBaton *baton = static_cast<RawDeviceEventBaton *>(handle->data);

    HandleScope scope(baton->callback->isolate);
    
    Local<Function> func = Local<Function>::New(baton->callback->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)baton->callback->callback));

    Local<Value> args[] = {
      Number::New(baton->callback->isolate, baton->controllerId),
      String::NewFromUtf8(baton->callback->isolate, baton->data, v8::NewStringType::kNormal).ToLocalChecked(),
    };

    func->Call(baton->callback->isolate->GetCurrentContext(), Null(baton->callback->isolate), 2, args);

    uv_close((uv_handle_t*) handle, RawDataHandleClosed);

  }

  void RawDataCallback(const char* data, int controllerId, int callbackId, void *callbackVoid) {

    RawDeviceEventBaton *baton = new RawDeviceEventBaton();

    baton->callback = static_cast<EventContext *>(callbackVoid);
    baton->data = strdup(data);
    baton->controllerId = controllerId;

    uv_async_t *asyncRawData = new uv_async_t;
    asyncRawData->data = baton;

    // Start event loop and attach callbacks
    uv_async_init(uv_default_loop(), asyncRawData, RawDataCallbackAfter);
    uv_async_send(asyncRawData);

  }

  void addRawDeviceEventListener(const FunctionCallbackInfo<Value>& args){
    
    Isolate* isolate = args.GetIsolate();
    HandleScope handleScope(isolate);

    Local<Function> cb = Local<Function>::Cast(args[0]);
    Persistent<Function, CopyablePersistentTraits<Function> > value(isolate, cb);

    if (!args[0]->IsFunction()) {
      Local<Value> exception = Exception::TypeError(String::NewFromUtf8(isolate, "Expected 1 argument: (function callback)", v8::NewStringType::kNormal).ToLocalChecked());
      isolate->ThrowException(exception);
    }

    EventContext *ctx = new EventContext();
    ctx->callback = value;
    ctx->isolate = isolate;
    
    Local<Number> num = Number::New(isolate, tdRegisterRawDeviceEvent((TDRawDeviceEvent)&RawDataCallback, ctx));

    args.GetReturnValue().Set(num);

  }


  struct js_work {

    uv_work_t req;
    
    EventContext *callback;

    //char* data;
    bool rb; // Return value, boolean
    int rn; // Return value, number
    char* rs; // Return value, string

    int f; // Worktype
    int devID; // Device ID
    int v; // Arbitrary number value
    char* s; // Arbitrary string value
    char* s2; // Arbitrary string value
    bool string_used;
    Isolate* isolate; // Global V8 isolate

    list<telldusDeviceInternals> l;
    list<telldusSensorInternals> si;

  };

  void RunWork(uv_work_t* req) {

    js_work* work = static_cast<js_work*>(req->data);
    switch(work->f) {
      case 0:

        uv_mutex_lock (&radioMutex);
        work->rn = tdTurnOn(work->devID);
        uv_mutex_unlock (&radioMutex);

        break;
      case 1:
        uv_mutex_lock (&radioMutex);
        work->rn = tdTurnOff(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 2:
        uv_mutex_lock (&radioMutex);
        work->rn = tdDim(work->devID,(unsigned char)work->v);
        uv_mutex_unlock (&radioMutex);
        break;
      case 3:
        uv_mutex_lock (&radioMutex);
        work->rn = tdLearn(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 4:
        work->rn = tdAddDevice();
        break;
      case 5: // SetName
        work->rb = tdSetName(work->devID,work->s);
        break;
      case 6: // GetName
        work->rs = tdGetName(work->devID);
        work->string_used = true;
        break;
      case 7: // SetProtocol
        work->rb = tdSetProtocol(work->devID,work->s);
        break;
      case 8: // GetProtocol
        work->rs = tdGetProtocol(work->devID);
        work->string_used = true;
        break;
      case 9: // SetModel
        work->rb = tdSetModel(work->devID,work->s);
        break;
      case 10: // GetModel
        work->rs = tdGetModel(work->devID);
        work->string_used = true;
        break;
      case 11: // GetDeviceType
        work->rn = tdGetDeviceType(work->devID);
        break;
      case 12:
        work->rb = tdRemoveDevice(work->devID);
        break;
      case 13:
        work->rn = tdUnregisterCallback(work->devID);
        break;
      case 14: // GetModel
        work->rs = tdGetErrorString(work->devID);
        work->string_used = true;
        break;
      case 15: // tdInit();
        uv_mutex_lock (&radioMutex);
        tdInit();
        uv_mutex_unlock (&radioMutex);
        work->rb = true; // tdInit() has no return value, so we augment true for a return value
        break;
      case 16: // tdClose();
        tdClose();
        work->rb = true; // tdClose() has no return value, so we augment true for a return value
        break;
      case 17: // tdGetNumberOfDevices();
        work->rn = tdGetNumberOfDevices();
        break;
      case 18: // tdStop
        uv_mutex_lock (&radioMutex);
        work->rn = tdStop(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 19: // tdBell
        uv_mutex_lock (&radioMutex);
        work->rn = tdBell(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;     
      case 20: // tdGetDeviceId(deviceIndex)
        work->rn = tdGetDeviceId(work->devID);
        break;
      case 21: // tdGetDeviceParameter(deviceId, name, val)
        work->rs = tdGetDeviceParameter(work->devID, work->s, work->s2);
        work->string_used = true;
        break;
      case 22: // tdSetDeviceParameter(deviceId, name, val)
        work->rb = tdSetDeviceParameter(work->devID, work->s, work->s2);
        break;
      case 23: // tdExecute
        uv_mutex_lock (&radioMutex);
        work->rn = tdExecute(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 24: // tdUp
        uv_mutex_lock (&radioMutex);
        work->rn = tdUp(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 25: // tdDown
        uv_mutex_lock (&radioMutex);
        work->rn = tdDown(work->devID);
        uv_mutex_unlock (&radioMutex);
        break;
      case 26: // getDevices
        work->l = getDevicesRaw();
        break;
      case 27: // getSensors
        work->si = getSensorsRaw();
        break;
    }

  }

  void RunCallback(uv_work_t* req, int status) {


    js_work* work = static_cast<js_work*>(req->data);
    work->string_used = false;

    HandleScope handleScope(work->isolate);

    Local<Value> argv[3];

    // This makes it possible to catch
    // the exception from JavaScript land using the
    // process.on('uncaughtException') event.
    TryCatch try_catch(work->isolate);

    EventContext *ctx;
    Local<Function> func;

    // Reenter the js-world
    switch(work->f) {

      // Return Number
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 11:
      case 13:
      case 17:
      case 18:
      case 19:
      case 20:
      case 23:
      case 24:
      case 25:
        argv[0] = Integer::New(work->isolate, work->rn); // Return number value
        argv[1] = Integer::New(work->isolate, work->f); // Return worktype

        ctx = static_cast<EventContext *>(work->callback);
        func = Local<Function>::New(work->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)ctx->callback));
        func->Call(work->isolate->GetCurrentContext(), work->isolate->GetCurrentContext()->Global(), 2, argv);

        break;

      // Return boolean
      case 5:
      case 7:
      case 9:
      case 12:
      case 15:
      case 16:
      case 22:
        argv[0] = Boolean::New(work->isolate, work->rb); // Return number value
        argv[1] = Integer::New(work->isolate, work->f); // Return worktype

        ctx = static_cast<EventContext *>(work->callback);
        func = Local<Function>::New(work->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)ctx->callback));
        func->Call(work->isolate->GetCurrentContext(), work->isolate->GetCurrentContext()->Global(), 2, argv);

        break;

      // Return String
      case 6:
      case 8:
      case 10:
      case 14:
      case 21:
        argv[0] = String::NewFromUtf8(work->isolate, work->rs, v8::NewStringType::kNormal).ToLocalChecked(); // Return string value
        argv[1] = Integer::New(work->isolate, work->f); // Return callback function

        ctx = static_cast<EventContext *>(work->callback);
        func = Local<Function>::New(work->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)ctx->callback));
        func->Call(work->isolate->GetCurrentContext(), work->isolate->GetCurrentContext()->Global(), 2, argv);

        break;

      // Return list<telldusDeviceInternals>
      case 26:
        argv[0] = getDevicesFromInternals(work->l, work->isolate); // Return Object
        argv[1] = Integer::New(work->isolate, work->f); // Return callback function

        ctx = static_cast<EventContext *>(work->callback);
        func = Local<Function>::New(work->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)ctx->callback));
        func->Call(work->isolate->GetCurrentContext(), work->isolate->GetCurrentContext()->Global(), 2, argv);

        break;

      // Return list<telldusSensorInternals>
      case 27: 
        argv[0] = getSensorsFromInternals(work->si, work->isolate); // Return Object
        argv[1] = Integer::New(work->isolate, work->f); // Return callback functio

        ctx = static_cast<EventContext *>(work->callback);
        func = Local<Function>::New(work->isolate, ((Persistent<Function, CopyablePersistentTraits<Function> >)ctx->callback));
        func->Call(work->isolate->GetCurrentContext(), work->isolate->GetCurrentContext()->Global(), 2, argv);

        break;

    }

    // Handle any exceptions thrown inside the callback
    if (try_catch.HasCaught()) {
      node::FatalException(work->isolate, try_catch);
    }

    // Check if we have an allocated string from telldus
    if( work->string_used ) {
      tdReleaseString(work->rs);
    }

    // properly cleanup, or death by millions of tiny leaks
    //work->callback.Dispose();
    //work->callback.Clear();

    free(work->s); // char* Created in AsyncCaller
    free(work->s2); // char* Created in AsyncCaller

    delete work;

  }

  void AsyncCaller(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = args.GetIsolate();
    HandleScope handleScope(isolate);

    // Make sure we don't get any funky data
    if(!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsString() || !args[4]->IsString()) {
      Local<Value> exception = Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments", v8::NewStringType::kNormal).ToLocalChecked());
      isolate->ThrowException(exception);
    }

    // Make a deep copy of the string argument as we don't want 
    // it memory managed by v8 in the worker thread
    String::Utf8Value str(isolate, args[3]);
    char * str_copy = strdup(*str); // Deleted at end of RunCallback

    String::Utf8Value str2(isolate, args[4]);
    char * str_copy2 = strdup(*str2); // Deleted at end of RunCallback

    Local<Function> cb = Local<Function>::Cast(args[5]);
    Persistent<Function, CopyablePersistentTraits<Function> > value(isolate, cb);

    EventContext *ctx = new EventContext();
    ctx->callback = value;
    ctx->isolate = isolate;

    js_work* work = new js_work;
    work->f = args[0].As<Integer>()->Value(); // Worktype
    work->devID = args[1].As<Integer>()->Value(); // Device ID
    work->v = args[2].As<Integer>()->Value(); // Arbitrary number value
    work->s = str_copy; // Arbitrary string value
    work->s2 = str_copy2; // Arbitrary string value
    work->isolate = isolate; 
    work->callback = ctx;
    work->req.data = work;

    uv_queue_work(uv_default_loop(), &work->req, (uv_work_cb)RunWork, (uv_after_work_cb)RunCallback);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Running asynchronous process initializer", v8::NewStringType::kNormal).ToLocalChecked());

  }

  void SyncCaller(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = args.GetIsolate();
    HandleScope handleScope(isolate);

    // Make sure we don't get any funky data
    if(!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsString() || !args[4]->IsString()) {
      Local<Value> exception = Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments", v8::NewStringType::kNormal).ToLocalChecked());
      isolate->ThrowException(exception);
    }

    // Make a deep copy of the string argument
    String::Utf8Value str(isolate, args[3]);
    char * str_copy = strdup(*str); // Deleted at end of this function

    String::Utf8Value str2(isolate, args[4]);
    char * str_copy2 = strdup(*str2); // Deleted at end of this function

    js_work* work = new js_work;
    work->f = args[0].As<Integer>()->Value(); // Worktype
    work->devID = args[1].As<Integer>()->Value(); // Device ID
    work->v = args[2].As<Integer>()->Value(); // Arbitrary number value
    work->s = str_copy; // Arbitrary string value
    work->s2 = str_copy2; // Arbitrary string value
    work->isolate = isolate;

    work->string_used = false; // Used to keep track of used telldus strings

    // Run requested operation
    switch(work->f) {
       case 0:
          uv_mutex_lock (&radioMutex); 
          work->rn = tdTurnOn(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
       case 1:
          uv_mutex_lock (&radioMutex); 
          work->rn = tdTurnOff(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
       case 2:
          uv_mutex_lock (&radioMutex); 
          work->rn = tdDim(work->devID,(unsigned char)work->v);
          uv_mutex_unlock (&radioMutex);
          break;
       case 3:
          uv_mutex_lock (&radioMutex); 
          work->rn = tdLearn(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
       case 4:
          work->rn = tdAddDevice();
          break;
       case 5: // SetName
          work->rb = tdSetName(work->devID,work->s);
          break;
       case 6: // GetName
          work->rs = tdGetName(work->devID);
          work->string_used = true;
          break;
       case 7: // SetProtocol
          work->rb = tdSetProtocol(work->devID,work->s);
          break;
       case 8: // GetProtocol
          work->rs = tdGetProtocol(work->devID);
          work->string_used = true;
          break;
       case 9: // SetModel
          work->rb = tdSetModel(work->devID,work->s);
          break;
       case 10: // GetModel
          work->rs = tdGetModel(work->devID);
          work->string_used = true;
          break;
       case 11: // GetDeviceType
          work->rn = tdGetDeviceType(work->devID);
          break;
       case 12:
          work->rb = tdRemoveDevice(work->devID);
          break;
       case 13:
          work->rn = tdUnregisterCallback(work->devID);
          break;
       case 14: // GetModel
          work->rs = tdGetErrorString(work->devID);
          work->string_used = true;
          break;
        case 15: // tdInit();
          uv_mutex_lock (&radioMutex); 
          tdInit();
          uv_mutex_unlock (&radioMutex);
          work->rb = true; // tdInit() has no return value, so we augment true for a return value
          break;
        case 16: // tdClose();
          tdClose();
          work->rb = true; // tdClose() has no return value, so we augment true for a return value
          break;
        case 17: // tdGetNumberOfDevices();
          work->rn = tdGetNumberOfDevices();
          break;
        case 18: // tdStop
          uv_mutex_lock (&radioMutex); 
          work->rn = tdStop(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
        case 19: // tdBell
          uv_mutex_lock (&radioMutex); 
          work->rn = tdBell(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;     
        case 20: // tdGetDeviceId(deviceIndex)
          work->rn = tdGetDeviceId(work->devID);
          break;
        case 21: // tdGetDeviceParameter(deviceId, name, val)
          work->rs = tdGetDeviceParameter(work->devID, work->s, work->s2);
          work->string_used = true;
          break;
        case 22: // tdSetDeviceParameter(deviceId, name, val)
          work->rb = tdSetDeviceParameter(work->devID, work->s, work->s2);
          break;
        case 23: // tdExecute
          uv_mutex_lock (&radioMutex); 
          work->rn = tdExecute(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
        case 24: // tdUp
          uv_mutex_lock (&radioMutex); 
          work->rn = tdUp(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
        case 25: // tdDown
          uv_mutex_lock (&radioMutex);
          work->rn = tdDown(work->devID);
          uv_mutex_unlock (&radioMutex);
          break;
        case 26: // getDevices
          work->l = getDevicesRaw();
	  [[fallthrough]];
        case 27: // getSensors
          work->si = getSensorsRaw();         
    }

    // Run callback
    Local<Value> argv;

    // Run the actual callback
    switch(work->f) {

      // Return Number
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 11:
      case 13:
      case 17:
      case 18:
      case 19:
      case 20:
      case 23:
      case 24:
      case 25:
        argv = Integer::New(isolate,work->rn); // Return number value
        break;

      // Return boolean
      case 5:
      case 7:
      case 9:
      case 12:
      case 15:
      case 16:
      case 22:
        argv = Boolean::New(isolate,work->rb); // Return number value
        break;

      // Return String
      case 6:
      case 8:
      case 10:
      case 14:
      case 21:
        argv = String::NewFromUtf8(isolate, work->rs, v8::NewStringType::kNormal).ToLocalChecked(); // Return string value
        break;

      // Return list<telldusDeviceInternals>
      case 26:
        argv = getDevicesFromInternals(work->l, work->isolate); // Return Object
        break;

      // Return list<telldusSensorInternals>
      case 27:
        argv = getSensorsFromInternals(work->si, work->isolate); // Return Object
        break;
    }

    // Check if we have an allocated string from telldus
    if( work->string_used ) {
      tdReleaseString(work->rs);
    }

    free(str_copy); // char* Created in the beginning of this function
    free(str_copy2); // char* Created in beginning of this function

    delete work;

    args.GetReturnValue().Set(argv);

  }

}

extern "C"

void init(Local<Object> exports) {

  uv_mutex_init (&telldus_v8::radioMutex);

  // Asynchronous function wrapper
  NODE_SET_METHOD(exports, "AsyncCaller", telldus_v8::AsyncCaller);

  // Syncronous function wrapper
  NODE_SET_METHOD(exports, "SyncCaller", telldus_v8::SyncCaller);

  // Functions to add event-listener callbacks 
  NODE_SET_METHOD(exports, "addDeviceEventListener", telldus_v8::addDeviceEventListener);
  NODE_SET_METHOD(exports, "addSensorEventListener", telldus_v8::addSensorEventListener);
  NODE_SET_METHOD(exports, "addRawDeviceEventListener", telldus_v8::addRawDeviceEventListener);

}

NODE_MODULE(telldus, init);

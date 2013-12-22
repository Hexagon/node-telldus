#ifndef BUILDING_NODE_EXTENSION
  #define BUILDING_NODE_EXTENSION
#endif // BUILDING_NODE_EXTENSION

#include <cstdlib>
#include <string.h>
#include <list>

#include <node.h>
#include <v8.h>

#include <telldus-core.h>

using namespace v8;
using namespace node;
using namespace std;

namespace telldus_v8 {

  struct DeviceEventBaton {
    Persistent<Function> callback;
    int deviceId;
    int lastSentCommand;
    int levelNum;
  };

  struct SensorEventBaton {
    Persistent<Function> callback;
    int sensorId;
    const char *model;
    const char *protocol;
    const char *value;
    int ts;
    int dataType;
  };

  struct RawDeviceEventBaton {
    Persistent<Function> callback;
    int controllerId;
    const char *data;
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

  Local<Object> GetSupportedMethods(int id, int supportedMethods){

    Local<Array> methodsObj = Array::New();

    int i = 0;

    if (supportedMethods & TELLSTICK_TURNON)  methodsObj->Set(i++, String::New("TURNON"));
    if (supportedMethods & TELLSTICK_TURNOFF) methodsObj->Set(i++, String::New("TURNOFF"));
    if (supportedMethods & TELLSTICK_BELL) methodsObj->Set(i++, String::New("BELL"));
    if (supportedMethods & TELLSTICK_TOGGLE) methodsObj->Set(i++, String::New("TOGGLE"));
    if (supportedMethods & TELLSTICK_DIM)  methodsObj->Set(i++, String::New("DIM"));
    if (supportedMethods & TELLSTICK_UP) methodsObj->Set(i++, String::New("UP"));
    if (supportedMethods & TELLSTICK_DOWN) methodsObj->Set(i++, String::New("DOWN"));
    if (supportedMethods & TELLSTICK_STOP) methodsObj->Set(i++, String::New("STOP"));
    if (supportedMethods & TELLSTICK_LEARN) methodsObj->Set(i++, String::New("LEARN"));

    return methodsObj;

  }

  Local<String> GetDeviceType(int id, int type){

    if(type & TELLSTICK_TYPE_DEVICE) return String::New("DEVICE");
    if(type & TELLSTICK_TYPE_GROUP) return String::New("GROUP");
    if(type & TELLSTICK_TYPE_SCENE) return String::New("SCENE");

    return String::New("UNKNOWN");

  }

  Local<Object> GetDeviceStatus(int id, int lastSentCommand, int level){

    Local<Object> status = Object::New();

    switch(lastSentCommand) {
      case TELLSTICK_TURNON:
        status->Set(String::NewSymbol("name"), String::New("ON"));
        break;
      case TELLSTICK_TURNOFF:
        status->Set(String::NewSymbol("name"), String::New("OFF"));
        break;
      case TELLSTICK_DIM:
        status->Set(String::NewSymbol("name"), String::New("DIM"));
        status->Set(String::NewSymbol("level"), Number::New(level));
        break;
      default:
        status->Set(String::NewSymbol("name"), String::New("UNNKOWN"));
    }

    return status;

  }

  Local<Object> GetDevice( telldusDeviceInternals deviceInternals ) {

    Local<Object> obj = Object::New();
    obj->Set(String::NewSymbol("name"), String::New(deviceInternals.name, strlen(deviceInternals.name)));
    obj->Set(String::NewSymbol("id"), Number::New(deviceInternals.id));
    obj->Set(String::NewSymbol("methods"), GetSupportedMethods(deviceInternals.id,deviceInternals.supportedMethods));
    obj->Set(String::NewSymbol("model"), String::New(deviceInternals.model, strlen(deviceInternals.model)));
    obj->Set(String::NewSymbol("protocol"), String::New(deviceInternals.protocol, strlen(deviceInternals.protocol)));
    obj->Set(String::NewSymbol("type"), GetDeviceType(deviceInternals.id,deviceInternals.deviceType));
    obj->Set(String::NewSymbol("status"), GetDeviceStatus(deviceInternals.id,deviceInternals.lastSentCommand,deviceInternals.level));

    // Cleanup
    tdReleaseString(deviceInternals.name);
    tdReleaseString(deviceInternals.model);
    tdReleaseString(deviceInternals.protocol);

    return obj;

  }

  Handle<Value> getDevicesFromInternals( list<telldusDeviceInternals> t ) {

    HandleScope scope;

    // Destination array
    Local<Array> devices = Array::New(t.size());
    int i=0;
    for (list<telldusDeviceInternals>::const_iterator iterator = t.begin(), end = t.end(); iterator != end; ++iterator) {
        devices->Set(i, GetDevice(*iterator));
        i++;
    }

    return scope.Close(devices);

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

  list<telldusDeviceInternals> getDevicesRaw() {
    
    int intNumberOfDevices = tdGetNumberOfDevices();
    list<telldusDeviceInternals> deviceList;

    for ( int i = 0 ; i < intNumberOfDevices ; i++ ) {
      deviceList.push_back(getDeviceRaw(i));
    }

    return deviceList;

  }

  void DeviceEventCallbackWorking(uv_work_t *req) { 

    DeviceEventBaton *baton = static_cast<DeviceEventBaton *>(req->data);

    // Get Status
    baton->lastSentCommand = tdLastSentCommand(baton->deviceId, SUPPORTED_METHODS);
    baton->levelNum = 0;
    char *level = 0;

    if(baton->lastSentCommand == TELLSTICK_DIM) {

      // Get level, returned from telldus-core as char
      level = tdLastSentValue(baton->deviceId);

      // Convert to number and add to object
      baton->levelNum = atoi(level);

      // Clean up the mess
      tdReleaseString(level);

    }


  }

  void DeviceEventCallbackAfter(uv_work_t *req, int status) {

    HandleScope scope;
    DeviceEventBaton *baton = static_cast<DeviceEventBaton *>(req->data);

    Local<Value> args[] = {
      Number::New(baton->deviceId),
      GetDeviceStatus(baton->deviceId,baton->lastSentCommand,baton->levelNum),
    };

    baton->callback->Call(baton->callback, 2, args);
    scope.Close(Undefined());

    delete baton;
    delete req;

  }

  void DeviceEventCallback( int deviceId, int method, const char * data, int callbackId, void* callbackVoid ) {

    DeviceEventBaton *baton = new DeviceEventBaton();

    baton->callback = static_cast<Function *>(callbackVoid);
    baton->deviceId = deviceId;

    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(uv_default_loop(), req, (uv_work_cb)DeviceEventCallbackWorking, (uv_after_work_cb)DeviceEventCallbackAfter);

  }

  Handle<Value> addDeviceEventListener( const Arguments& args ) {

    HandleScope scope;

    if (!args[0]->IsFunction()) {
    return ThrowException(Exception::TypeError(String::New("Expected 1 argument: (function callback)")));
    }

    Persistent<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[0]));
    Local<Number> num = Number::New(tdRegisterDeviceEvent(&DeviceEventCallback, *callback));

    return scope.Close(num);

  }

  void SensorEventCallbackWorking(uv_work_t *req) { }

  void SensorEventCallbackAfter(uv_work_t *req, int status) {

    HandleScope scope;

    SensorEventBaton *baton = static_cast<SensorEventBaton *>(req->data);

    Local<Value> args[] = {
      Number::New(baton->sensorId),
      String::New(baton->model),
      String::New(baton->protocol),
      Number::New(baton->dataType),
      String::New(baton->value),
      Number::New(baton->ts)
    };

    baton->callback->Call(baton->callback, 6, args);
    scope.Close(Undefined());

    delete baton;
    delete req;

  }

  void SensorEventCallback( const char *protocol, const char *model, int sensorId, int dataType, const char *value,
        int ts, int callbackId, void *callbackVoid ) {

    SensorEventBaton *baton = new SensorEventBaton();

    baton->callback = static_cast<Function *>(callbackVoid);
    baton->sensorId = sensorId;
    baton->protocol = protocol;
    baton->model = model;
    baton->ts = ts;
    baton->dataType = dataType;
    baton->value = value;

    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(uv_default_loop(), req, (uv_work_cb)SensorEventCallbackWorking, (uv_after_work_cb)SensorEventCallbackAfter);

  }

  Handle<Value> addSensorEventListener( const Arguments& args ) {

    HandleScope scope;

    if (!args[0]->IsFunction()) {
    return ThrowException(Exception::TypeError(String::New("Expected 1 argument: (function callback)")));
    }

    Persistent<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[0]));
    Local<Number> num = Number::New(tdRegisterSensorEvent(&SensorEventCallback, *callback));

    return scope.Close(num);

  }

  void RawDataEventCallbackWorking(uv_work_t *req) { }

  void RawDataEventCallbackAfter(uv_work_t *req, int status) {

    HandleScope scope;
    RawDeviceEventBaton *baton = static_cast<RawDeviceEventBaton *>(req->data);

    Local<Value> args[] = {
      Number::New(baton->controllerId),
      String::New(baton->data),
    };

    baton->callback->Call(baton->callback, 2, args);
    scope.Close(Undefined());

    delete baton;
    delete req;

  }

  void RawDataCallback(const char* data, int controllerId, int callbackId, void *callbackVoid) {

    RawDeviceEventBaton *baton = new RawDeviceEventBaton();

    baton->callback = static_cast<Function *>(callbackVoid);
    baton->data = data;
    baton->controllerId = controllerId;

    uv_work_t* req = new uv_work_t;
    req->data = baton;

    uv_queue_work(uv_default_loop(), req, (uv_work_cb)RawDataEventCallbackWorking, (uv_after_work_cb)RawDataEventCallbackAfter);

  }


  Handle<Value> addRawDeviceEventListener( const Arguments& args ) {

    HandleScope scope;
    if (!args[0]->IsFunction()) {
      return ThrowException(Exception::TypeError(String::New("Expected 1 argument: (function callback)")));
    }

    Persistent<Function> callback = Persistent<Function>::New(Handle<Function>::Cast(args[0]));
    Local<Number> num = Number::New(tdRegisterRawDeviceEvent(&RawDataCallback, *callback));
    return scope.Close(num);

  }


  struct js_work {

    uv_work_t req;
    Persistent<Function> callback;
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

    list<telldusDeviceInternals> l;

  };

  void RunWork(uv_work_t* req) {
    js_work* work = static_cast<js_work*>(req->data);
    switch(work->f) {
      case 0:
        work->rn = tdTurnOn(work->devID);
        break;
      case 1:
        work->rn = tdTurnOff(work->devID);
        break;
      case 2:
        work->rn = tdDim(work->devID,(unsigned char)work->v);
        break;
      case 3:
        work->rn = tdLearn(work->devID);
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
        tdInit();
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
        work->rn = tdStop(work->devID);
        break;
      case 19: // tdBell
        work->rn = tdBell(work->devID);
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
        work->rn = tdExecute(work->devID);
        break;
      case 24: // tdUp
        work->rn = tdUp(work->devID);
        break;
      case 25: // tdDown
        work->rn = tdDown(work->devID);
        break;
      case 26: // getDevices
        work->l = getDevicesRaw();
	break;
    }

  }

  void RunCallback(uv_work_t* req, int status) {

    js_work* work = static_cast<js_work*>(req->data);
    work->string_used = false;

    Handle<Value> argv[3];


    // This makes it possible to catch
    // the exception from JavaScript land using the
    // process.on('uncaughtException') event.
    TryCatch try_catch;

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
        argv[0] = Integer::New(work->rn); // Return number value
        argv[1] = Integer::New(work->f); // Return worktype

        work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

        break;

      // Return boolean
      case 5:
      case 7:
      case 9:
      case 12:
      case 15:
      case 16:
      case 22:
        argv[0] = Boolean::New(work->rb); // Return number value
        argv[1] = Integer::New(work->f); // Return worktype

        work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

        break;

      // Return String
      case 6:
      case 8:
      case 10:
      case 14:
      case 21:
        argv[0] = String::New(work->rs); // Return string value
        argv[1] = Integer::New(work->f); // Return callback function

        work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

        break;

      // Return list<telldusDeviceInternals>
      case 26:
        argv[0] = getDevicesFromInternals(work->l); // Return Object
        argv[1] = Integer::New(work->f); // Return callback function

        work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

        break;

    }

    // Handle any exceptions thrown inside the callback
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }

    // Check if we have an allocated string from telldus
    if( work->string_used ) {
      tdReleaseString(work->rs);
    }

    // properly cleanup, or death by millions of tiny leaks
    work->callback.Dispose();
    work->callback.Clear();

    free(work->s); // char* Created in AsyncCaller
    free(work->s2); // char* Created in AsyncCaller

    delete work;

  }

  Handle<Value> AsyncCaller(const Arguments& args) {

    HandleScope scope;

    // Make sure we don't get any funky data
    if(!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsString() || !args[4]->IsString()) {
      return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    }

    // Make a deep copy of the string argument as we don't want 
    // it memory managed by v8 in the worker thread
    String::Utf8Value str(args[3]);
    char * str_copy = strdup(*str); // Deleted at end of RunCallback

    String::Utf8Value str2(args[4]);
    char * str_copy2 = strdup(*str2); // Deleted at end of RunCallback

    js_work* work = new js_work;
    work->f = args[0]->NumberValue(); // Worktype
    work->devID = args[1]->NumberValue(); // Device ID
    work->v = args[2]->NumberValue(); // Arbitrary number value
    work->s = str_copy; // Arbitrary string value
    work->s2 = str_copy2; // Arbitrary string value

    work->req.data = work;
    work->callback = Persistent<Function>::New(Handle<Function>::Cast(args[5]));

    uv_queue_work(uv_default_loop(), &work->req, RunWork, (uv_after_work_cb)RunCallback);

    Local<String> retstr = String::New("Running asynchronous process initializer");

    return scope.Close(retstr);

  }


  Handle<Value> SyncCaller(const Arguments& args) {

    // Start a new scope
    HandleScope scope;

    // Make sure we don't get any funky data
    if(!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsString() || !args[4]->IsString()) {
       return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    }

    // Make a deep copy of the string argument
    String::Utf8Value str(args[3]);
    char * str_copy = strdup(*str); // Deleted at end of this function

    String::Utf8Value str2(args[4]);
    char * str_copy2 = strdup(*str2); // Deleted at end of this function

    js_work* work = new js_work;
    work->f = args[0]->NumberValue(); // Worktype
    work->devID = args[1]->NumberValue(); // Device ID
    work->v = args[2]->NumberValue(); // Arbitrary number value
    work->s = str_copy; // Arbitrary string value
    work->s2 = str_copy2; // Arbitrary string value

    work->string_used = false; // Used to keep track of used telldus strings

    // Run requested operation
    switch(work->f) {
       case 0:
          work->rn = tdTurnOn(work->devID);
          break;
       case 1:
          work->rn = tdTurnOff(work->devID);
          break;
       case 2:
          work->rn = tdDim(work->devID,(unsigned char)work->v);
          break;
       case 3:
          work->rn = tdLearn(work->devID);
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
          tdInit();
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
          work->rn = tdStop(work->devID);
          break;
        case 19: // tdBell
          work->rn = tdBell(work->devID);
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
          work->rn = tdExecute(work->devID);
          break;
        case 24: // tdUp
          work->rn = tdUp(work->devID);
          break;
        case 25: // tdDown
          work->rn = tdDown(work->devID);
          break;
        case 26: // getDevices
          work->l = getDevicesRaw();
    }

    // Run callback
    Handle<Value> argv;

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
        argv = Integer::New(work->rn); // Return number value
        break;

      // Return boolean
      case 5:
      case 7:
      case 9:
      case 12:
      case 15:
      case 16:
      case 22:
        argv = Boolean::New(work->rb); // Return number value
        break;

      // Return String
      case 6:
      case 8:
      case 10:
      case 14:
      case 21:
        argv = String::New(work->rs); // Return string value
        break;

      // Return list<telldusDeviceInternals>
      case 26:
        argv = getDevicesFromInternals(work->l); // Return Object
        break;
    }

    // Check if we have an allocated string from telldus
    if( work->string_used ) {
      tdReleaseString(work->rs);
    }

    free(str_copy); // char* Created in the beginning of this function
    free(str_copy2); // char* Created in beginning of this function

    delete work;

    return scope.Close(argv);

  }

}

extern "C"
void init(Handle<Object> target) {

  HandleScope scope;

  // Asynchronous function wrapper
  target->Set(String::NewSymbol("AsyncCaller"),
    FunctionTemplate::New(telldus_v8::AsyncCaller)->GetFunction());

  // Syncronous function wrapper
  target->Set(String::NewSymbol("SyncCaller"),
    FunctionTemplate::New(telldus_v8::SyncCaller)->GetFunction());

  // Functions to add event-listener callbacks 
  target->Set(String::NewSymbol("addDeviceEventListener"),
    FunctionTemplate::New(telldus_v8::addDeviceEventListener)->GetFunction());
  target->Set(String::NewSymbol("addSensorEventListener"),
    FunctionTemplate::New(telldus_v8::addSensorEventListener)->GetFunction());
  target->Set(String::NewSymbol("addRawDeviceEventListener"),
    FunctionTemplate::New(telldus_v8::addRawDeviceEventListener)->GetFunction());

}
NODE_MODULE(telldus, init)

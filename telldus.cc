#ifndef BUILDING_NODE_EXTENSION
   #define BUILDING_NODE_EXTENSION
#endif // BUILDING_NODE_EXTENSION

#include <node.h>
#include <v8.h>
#include <node_buffer.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <telldus-core.h>

using namespace v8;
using namespace node;
using namespace std;

#define SIZE 8

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

namespace telldus_v8 {

   struct DeviceEventBaton {
      Persistent<Function> callback;
      int deviceId;
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

   Handle<Value> getNumberOfDevices( const Arguments& args ) {
      HandleScope scope;

      Local<Number> num = Number::New(tdGetNumberOfDevices());
      return scope.Close(num);
   }

   Local<Object> GetSuportedMethods(int id){

      int methods = tdMethods( id, SUPPORTED_METHODS );
      Local<Array> methodsObj = Array::New();
      int i = 0;
      if (methods & TELLSTICK_TURNON)
      {
         methodsObj->Set(i++, String::New("TURNON"));
      }
      if (methods & TELLSTICK_TURNOFF)
      {
         methodsObj->Set(i++, String::New("TURNOFF"));
      }
      if (methods & TELLSTICK_BELL)
      {
         methodsObj->Set(i++, String::New("BELL"));
      }
      if (methods & TELLSTICK_TOGGLE)
      {
         methodsObj->Set(i++, String::New("TOGGLE"));
      }
      if (methods & TELLSTICK_DIM)
      {
         methodsObj->Set(i++, String::New("DIM"));
      }
      if (methods & TELLSTICK_UP)
      {
         methodsObj->Set(i++, String::New("UP"));
      }
      if (methods & TELLSTICK_DOWN)
      {
         methodsObj->Set(i++, String::New("DOWN"));
      }
      if (methods & TELLSTICK_STOP)
      {
         methodsObj->Set(i++, String::New("STOP"));
      }
      if (methods & TELLSTICK_LEARN)
      {
         methodsObj->Set(i++, String::New("LEARN"));
      }

      return methodsObj;

   }

   Local<String> GetDeviceType(int id){
      int type = tdGetDeviceType(id);

      if(type & TELLSTICK_TYPE_DEVICE){
         return String::New("DEVICE");
      }

      if(type & TELLSTICK_TYPE_GROUP){
         return String::New("GROUP");
      }

      if(type & TELLSTICK_TYPE_SCENE){
         return String::New("SCENE");
      }
      return String::New("UNKNOWN");
   }

   Local<Object> GetDeviceStatus(int id){
      Local<Object> status = Object::New();
      int lastSentCommand = tdLastSentCommand(id, SUPPORTED_METHODS);
      int level_num = 0;
      char *level = 0;
      switch(lastSentCommand) {
         case TELLSTICK_TURNON:
            status->Set(String::NewSymbol("status"), String::New("ON"));
            break;
         case TELLSTICK_TURNOFF:
            status->Set(String::NewSymbol("status"), String::New("OFF"));
            break;
         case TELLSTICK_DIM:
            status->Set(String::NewSymbol("status"), String::New("DIM"));

            // Get level, returned from telldus-core as char
            level = tdLastSentValue(id);

            // Convert to number and add to object
            level_num = atoi(level);
            status->Set(String::NewSymbol("level"), Number::New(level_num));

            // Clean up the mess
            tdReleaseString(level);

            break;
         default:
            status->Set(String::NewSymbol("status"), String::New("UNNKOWN"));
      }

      return status;
   }

   Local<Object> GetDevice(int index) {

      int id = tdGetDeviceId( index );
      char *name = tdGetName( id );
      char *model = tdGetModel(id);

      Local<Object> obj = Object::New();
      obj->Set(String::NewSymbol("name"), String::New(name, strlen(name)));
      obj->Set(String::NewSymbol("id"), Number::New(id));
      obj->Set(String::NewSymbol("methods"), GetSuportedMethods(id));
      obj->Set(String::NewSymbol("model"), String::New(model, strlen(model)));
      obj->Set(String::NewSymbol("type"), GetDeviceType(id));
      obj->Set(String::NewSymbol("status"), GetDeviceStatus(id));

      tdReleaseString(name);
      tdReleaseString(model);

      return obj;
   }

   Handle<Value> getDevices( const Arguments& args ) {
      HandleScope scope;

      int intNumberOfDevices = tdGetNumberOfDevices();
      Local<Array> devices = Array::New(intNumberOfDevices);

      for (int i = 0; i < intNumberOfDevices; i++) {
         devices->Set(i, GetDevice(i));
       }
      return scope.Close(devices);
   }

   Handle<Value> turnOn( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      Local<Number> num = Number::New(tdTurnOn(args[0]->NumberValue()));
      return scope.Close(num);
   }

   Handle<Value> turnOff( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      Local<Number> num = Number::New(tdTurnOff(args[0]->NumberValue()));
      return scope.Close(num);
   }

   Handle<Value> dim( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()|| !args[1]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      Local<Number> num = Number::New(tdDim(args[0]->NumberValue(),(unsigned char)args[1]->NumberValue() ));
      return scope.Close(num);
   }

   Handle<Value> learn( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      Local<Number> num = Number::New(tdLearn(args[0]->NumberValue()));
      return scope.Close(num);
   }

   Handle<Value> addDevice( const Arguments& args ) {
      HandleScope scope;
      Local<Number> num = Number::New(tdAddDevice());
      return scope.Close(num);
   }

   Handle<Value> getName( const Arguments& args ) {

      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      // Save the string temporarily
      char * l_str = tdGetName(args[0]->NumberValue());
      Local<String> str = String::New(l_str);

      // ... and release it
      tdReleaseString(l_str);

      return scope.Close(str);
   }

   Handle<Value> setName( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()|| !args[1]->IsString()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      v8::String::Utf8Value str(args[1]);
      const char* cstr = ToCString(str);
      
      Handle<Boolean> num = Boolean::New(tdSetName(args[0]->IntegerValue(), cstr ));
      return scope.Close(num);
   }
   
   Handle<Value> getProtocol( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      // Save the string temporarily
      char * l_str = tdGetProtocol(args[0]->NumberValue());
      Local<String> str = String::New(l_str);

      // ... and release it
      tdReleaseString(l_str);

      return scope.Close(str);
   }
   
   Handle<Value> setProtocol( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()|| !args[1]->IsString()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      
      v8::String::Utf8Value str(args[1]);
      const char* cstr = ToCString(str);
      
      Handle<Boolean> num = Boolean::New(tdSetProtocol(args[0]->IntegerValue(), cstr ));
      return scope.Close(num);
   }
   
   Handle<Value> getModel( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      
      // Save the string temporarily
      char * l_str = tdGetModel(args[0]->NumberValue());
      Local<String> str = String::New(l_str);

      // ... and release it
      tdReleaseString(l_str);

      return scope.Close(str);
   }
   
   Handle<Value> setModel( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()|| !args[1]->IsString()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      
      v8::String::Utf8Value str(args[1]);
      const char* cstr = ToCString(str);
      
      Handle<Boolean> num = Boolean::New(tdSetModel(args[0]->NumberValue(), cstr ));
      return scope.Close(num);
   }
   
   Handle<Value> getDeviceParameter( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber() || !args[1]->IsString() || !args[2]->IsString()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      
      // Prepare string arguments
      v8::String::Utf8Value str1(args[1]);
      const char* cstr1 = ToCString(str1);
      
      v8::String::Utf8Value str2(args[2]);
      const char* cstr2 = ToCString(str2);

      // Save the string temporarily
      char * l_str = tdGetDeviceParameter(args[0]->NumberValue(), cstr1, cstr2 );
      Local<String> str = String::New(l_str);

      // ... and release it
      tdReleaseString(l_str);

      return scope.Close(str);

   }
   
   Handle<Value> setDeviceParameter( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber() || !args[1]->IsString() || !args[2]->IsString()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      
      v8::String::Utf8Value str1(args[1]);
      const char* cstr1 = ToCString(str1);
      
      v8::String::Utf8Value str2(args[2]);
      const char* cstr2 = ToCString(str2);
      
      Local<Number> num = Number::New(tdSetDeviceParameter(args[0]->NumberValue(), cstr1, cstr2 ));
      return scope.Close(num);
   }
   
   Handle<Value> removeDevice( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }
      Local<Number> num = Number::New(tdRemoveDevice(args[0]->NumberValue()));
      return scope.Close(num);
   }
   
   Handle<Value> getErrorString( const Arguments& args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      // Save the string temporarily
      char * l_str = tdGetErrorString(args[0]->NumberValue());
      Local<String> str = String::New(l_str);

      // ... and release it
      tdReleaseString(l_str);

      return scope.Close(str);
   }
   

    void DeviceEventCallbackWorking(uv_work_t *req) {
        // Space for work ...
    }

    void DeviceEventCallbackAfter(uv_work_t *req, int status) {
        HandleScope scope;
        DeviceEventBaton *baton = static_cast<DeviceEventBaton *>(req->data);

        Local<Value> args[] = {
            Number::New(baton->deviceId),
            GetDeviceStatus(baton->deviceId),
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

    void SensorEventCallbackWorking(uv_work_t *req) {
        // Space for work ...
    }

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

    void RawDataEventCallbackWorking(uv_work_t *req) {
        // Space for work ...
    }

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

   Handle<Value> removeEventListener( const Arguments &args ) {
      HandleScope scope;
      if (!args[0]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Expected 1 argument: (int callbackId)")));
      }

      Local<Number> num = Number::New(tdUnregisterCallback(args[0]->ToInteger()->Value()));

      //FIXME: Fix leak of callback.

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
      const char* s; // Arbitrary string value
      bool string_used;

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
            work->rn = tdRemoveDevice(work->devID);
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
         case 12:
         case 13:
            argv[0] = Integer::New(work->rn); // Return number value
            argv[1] = Integer::New(work->f); // Return worktype

            work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

            break;

         // Return boolean
         case 5:
         case 7:
         case 9:
         case 15:
         case 16:
            argv[0] = Boolean::New(work->rb); // Return number value
            argv[1] = Integer::New(work->f); // Return worktype

            work->callback->Call(Context::GetCurrent()->Global(), 2, argv);

            break;

         // Return String
         case 6:
         case 8:
         case 10:
         case 14:
            argv[0] = String::New(work->rs); // Return string value
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

      delete work;

   }

   /* the JS entry point */
   Handle<Value> AsyncCaller(const Arguments& args) {
      HandleScope scope;

      if(!args[1]->IsNumber()) {
         return ThrowException(Exception::TypeError(String::New("Wrong arguments")));
      }

      // Prepare string
      v8::String::Utf8Value str(args[3]);
      const char* cstr = ToCString(str);

      js_work* work = new js_work;
      work->f = args[0]->NumberValue(); // Worktype
      work->devID = args[1]->NumberValue(); // Device ID
      work->v = args[2]->NumberValue(); // Arbitrary number value
      work->s = cstr; // Arbitrary string value

      work->req.data = work;
      work->callback = Persistent<Function>::New(Handle<Function>::Cast(args[4]));

      uv_queue_work(uv_default_loop(), &work->req, RunWork, (uv_after_work_cb)RunCallback);

      Local<String> retstr = String::New("Running process initializer");
      return scope.Close(retstr);

      //return Undefined();
   }

}

extern "C"
void init(Handle<Object> target) {

   HandleScope scope;

   target->Set(String::NewSymbol("AsyncCaller"),
     FunctionTemplate::New(telldus_v8::AsyncCaller)->GetFunction());
   target->Set(String::NewSymbol("getNumberOfDevices"),
     FunctionTemplate::New(telldus_v8::getNumberOfDevices)->GetFunction());
   target->Set(String::NewSymbol("getDevices"),
     FunctionTemplate::New(telldus_v8::getDevices)->GetFunction());
   target->Set(String::NewSymbol("turnOn"),
     FunctionTemplate::New(telldus_v8::turnOn)->GetFunction());
   target->Set(String::NewSymbol("turnOff"),
     FunctionTemplate::New(telldus_v8::turnOff)->GetFunction());
   target->Set(String::NewSymbol("dim"),
     FunctionTemplate::New(telldus_v8::dim)->GetFunction());
   target->Set(String::NewSymbol("learn"),
     FunctionTemplate::New(telldus_v8::learn)->GetFunction());
   target->Set(String::NewSymbol("addDeviceEventListener"),
     FunctionTemplate::New(telldus_v8::addDeviceEventListener)->GetFunction());
   target->Set(String::NewSymbol("addSensorEventListener"),
     FunctionTemplate::New(telldus_v8::addSensorEventListener)->GetFunction());
   target->Set(String::NewSymbol("addRawDeviceEventListener"),
     FunctionTemplate::New(telldus_v8::addRawDeviceEventListener)->GetFunction());
   target->Set(String::NewSymbol("removeEventListener"),
     FunctionTemplate::New(telldus_v8::removeEventListener)->GetFunction());
   target->Set(String::NewSymbol("addDevice"),
     FunctionTemplate::New(telldus_v8::addDevice)->GetFunction());

   target->Set(String::NewSymbol("getName"),
     FunctionTemplate::New(telldus_v8::getName)->GetFunction());
   target->Set(String::NewSymbol("setName"),
     FunctionTemplate::New(telldus_v8::setName)->GetFunction());

   target->Set(String::NewSymbol("getProtocol"),
     FunctionTemplate::New(telldus_v8::getProtocol)->GetFunction());
   target->Set(String::NewSymbol("setProtocol"),
     FunctionTemplate::New(telldus_v8::setProtocol)->GetFunction());

   target->Set(String::NewSymbol("getModel"),
     FunctionTemplate::New(telldus_v8::getModel)->GetFunction());
   target->Set(String::NewSymbol("setModel"),
     FunctionTemplate::New(telldus_v8::setModel)->GetFunction());

/* target->Set(String::NewSymbol("myGetDeviceType"),
     FunctionTemplate::New(telldus_v8::myGetDeviceType)->GetFunction());
   target->Set(String::NewSymbol("setType"),
     FunctionTemplate::New(telldus_v8::setType)->GetFunction());*/

/* target->Set(String::NewSymbol("getDeviceParameter"),
     FunctionTemplate::New(telldus_v8::getDeviceParameter)->GetFunction());
   target->Set(String::NewSymbol("setDeviceParameter"),
     FunctionTemplate::New(telldus_v8::setDeviceParameter)->GetFunction());*/

   target->Set(String::NewSymbol("removeDevice"),
     FunctionTemplate::New(telldus_v8::removeDevice)->GetFunction());
   target->Set(String::NewSymbol("getErrorString"),
     FunctionTemplate::New(telldus_v8::getErrorString)->GetFunction());

 
}
NODE_MODULE(telldus, init)

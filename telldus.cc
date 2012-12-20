#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <telldus-core.h>

using namespace v8;
using namespace node;
using namespace std;

namespace telldus_v8 {

    struct DeviceEventBatton {
        Persistent<Function> callback;
        int deviceId;
    };

    struct SensorEventBatton {
        Persistent<Function> callback;
        int sensorId;
        const char *model;
        const char *protocol;
        const char *value;
        int ts;
        int dataType;
    };

    struct RawDeviceEventBatton {
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
        //tdInit();
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
                level = tdLastSentValue(id);
                status->Set(String::NewSymbol("level"), String::New(level, strlen(level)));
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
        tdInit();

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

    int DeviceEventCallbackAfter(eio_req *req) {
        HandleScope scope;
        DeviceEventBatton *batton = static_cast<DeviceEventBatton *>(req->data);

        Local<Value> args[] = {
            Number::New(batton->deviceId),
            GetDeviceStatus(batton->deviceId),
        };

        batton->callback->Call(batton->callback, 3, args);
        scope.Close(Undefined());

        delete batton;
        return 0;
    }

    void DeviceEventCallback( int deviceId, int method, const char * data, int callbackId, void* callbackVoid ) {
        DeviceEventBatton *batton = new DeviceEventBatton();
        batton->callback = static_cast<Function *>(callbackVoid);
        batton->deviceId = deviceId;
        eio_nop(EIO_PRI_DEFAULT, DeviceEventCallbackAfter, batton);
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

    int SensorEventCallbackAfter(eio_req *req) {
        HandleScope scope;
        SensorEventBatton *batton = static_cast<SensorEventBatton *>(req->data);

        Local<Value> args[] = {
            Number::New(batton->sensorId),
            String::New(batton->model),
            String::New(batton->protocol),
            Number::New(batton->dataType),
            String::New(batton->value),
            Number::New(batton->ts)
        };

        batton->callback->Call(batton->callback, 6, args);
        scope.Close(Undefined());

        delete batton;
        return 0;
    }

    void SensorEventCallback( const char *protocol, const char *model, int sensorId, int dataType, const char *value,
            int ts, int callbackId, void *callbackVoid ) {

        SensorEventBatton *batton = new SensorEventBatton();
        batton->callback = static_cast<Function *>(callbackVoid);
        batton->sensorId = sensorId;
        batton->protocol = protocol;
        batton->model = model;
        batton->ts = ts;
        batton->dataType = dataType;
        batton->value = value;

        eio_nop(EIO_PRI_DEFAULT, SensorEventCallbackAfter, batton);
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

    int RawDataEventCallbackAfter(eio_req *req) {
        HandleScope scope;
        RawDeviceEventBatton *batton = static_cast<RawDeviceEventBatton *>(req->data);

        Local<Value> args[] = {
            Number::New(batton->controllerId),
            String::New(batton->data),
        };

        batton->callback->Call(batton->callback, 5, args);
        scope.Close(Undefined());

        delete batton;
        return 0;
    }

    void RawDataCallback(const char* data, int controllerId, int callbackId, void *callbackVoid) {
        RawDeviceEventBatton *batton = new RawDeviceEventBatton();
        batton->callback = static_cast<Function *>(callbackVoid);
        batton->data = data;
        batton->controllerId = controllerId;
        eio_nop(EIO_PRI_DEFAULT, RawDataEventCallbackAfter, batton);
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
        tdUnregisterCallback(args[0]->ToInteger()->Value());

        //FIXME: Fix leak of callback.

        return scope.Close(Undefined());
    }
}

extern "C"
void init(Handle<Object> target) {

    HandleScope scope;
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
    target->Set(String::NewSymbol("addDeviceEventListener"),
      FunctionTemplate::New(telldus_v8::addDeviceEventListener)->GetFunction());
    target->Set(String::NewSymbol("addSensorEventListener"),
      FunctionTemplate::New(telldus_v8::addSensorEventListener)->GetFunction());
    target->Set(String::NewSymbol("addRawDeviceEventListener"),
      FunctionTemplate::New(telldus_v8::addRawDeviceEventListener)->GetFunction());
    target->Set(String::NewSymbol("removeEventListener"),
      FunctionTemplate::New(telldus_v8::removeEventListener)->GetFunction());
}

NODE_MODULE(telldus, init)

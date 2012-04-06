#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <stdio.h>

#include <telldus-core.h>

using namespace v8;
using namespace node;

#ifdef _WINDOWS
#define strcasecmp _stricmp
#define DEGREE " "
#else
#define DEGREE "°"
#endif

const int SUPPORTED_METHODS =
    TELLSTICK_TURNON |
    TELLSTICK_TURNOFF |
    TELLSTICK_BELL |
    TELLSTICK_DIM;

const int DATA_LENGTH = 20;

namespace telldus_v8 {

    Handle<Value> TdGetNumberOfDevices( const Arguments& args ) {
        HandleScope scope;
        //tdInit();
        Local<Number> num = Number::New(tdGetNumberOfDevices());
        return scope.Close(num);
    }
}

extern "C"
void init(Handle<Object> target) {
    
     HandleScope scope;
  //Local<FunctionTemplate> t = FunctionTemplate::New(telldus_v8::tdGetNumberOfDevices);

  //target->Set( String::NewSymbol( "tdGetNumberOfDevices" ), t->GetFunction() );
   target->Set(String::NewSymbol("TdGetNumberOfDevices"),
      FunctionTemplate::New(telldus_v8::TdGetNumberOfDevices)->GetFunction());
}

NODE_MODULE(telldus, init)
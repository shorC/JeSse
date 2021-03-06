#include <stdlib.h>
#include <math.h>

#include "runtime.h"

#ifndef NSMALLPOSINTS
#define NSMALLPOSINTS           16//257
#endif
#ifndef NSMALLNEGINTS
#define NSMALLNEGINTS           0//7
#endif
#if NSMALLNEGINTS + NSMALLPOSINTS > 0
/* References to small integers are saved in this array so that they
   can be shared.
   The integers that are saved are those in the range
   -NSMALLNEGINTS (inclusive) to NSMALLPOSINTS (not inclusive).

*/  
    static JsNumObject *small_ints[NSMALLNEGINTS + NSMALLPOSINTS];
#endif

// ?: What's the proper buffer size?
#define NUM_BUFFERSIZE 24

// C - API
/**
    It has been fully tested on Mac OS X, clang;

    On most modern desktop Intel CPU, SSE instruction set is enabled.
    Conversion may use SSE like cvttsd2si, or cvtsi2sdq; but it won't 
    act like 'node.js(v8)'.
    PS:v8 uses Grisu?
*/
JsObject *
JsNum_FromDouble(double dval)
{
    register JsNumObject *v;

#if NSMALLNEGINTS + NSMALLPOSINTS > 0
    long ival = dval;
    // WARNING: maybe segmentation fault without init
    if (ival == dval && -NSMALLNEGINTS <= ival && ival < NSMALLPOSINTS) {
        v = small_ints[ival + NSMALLNEGINTS];
        Js_INCREF(v);
        return (JsObject *) v;
    }
#endif

    v = Js_Malloc(sizeof(JsNumObject));
    JsObject_INIT(v, &JsNum_Type);
    v->ob_ival = dval;
    return (JsObject *) v;
}

double
JsNum_GetDouble(JsObject *obj)
{
    if (obj && JsNum_CheckType(obj))
    {
        return ((JsNumObject *)obj)->ob_ival;
    }
    if (obj == NULL)
    {
        dbgprint("Invalid type in deallocation of intobject\n");
        errnoInNum = JsException_NullPtrError;
        return NAN;
    }
    if (JsString_CheckType(obj))
    {
        char *endptr;
        double ret = strtod(JsString_AS_CSTRING(obj), &endptr);
        if (*endptr != 0)
        {
            errnoInNum = JsException_ParseError;
            return NAN;
        }
        return ret;
    }
    if (JsBool_CheckType(obj))
    {
        return (obj == Js_True) ? 1 : 0;
    }

    errnoInNum = JsException_ParseError;
    return NAN;
}

int
JsNum_GetInt(JsObject *obj)
{
    double dval = JsNum_GetDouble(obj);

    if (isnan(dval))
        return 0;

    return (int)dval;
}

// methods

static void
num_dealloc(JsNumObject *obj)
{
    Js_Free(obj);
}

static void
_num_internal_tocstring(JsNumObject *v, char* buffer)
{
    /* TODO: Rewrite it, memory's leaking
     *  __Balloc_D2A (in /usr/lib/system/libsystem_c.dylib)
     *  __d2b_D2A (in /usr/lib/system/libsystem_c.dylib)
     *  __dtoa (in /usr/lib/system/libsystem_c.dylib)
     *  __vfprintf (in /usr/lib/system/libsystem_c.dylib)
     *  __v2printf (in /usr/lib/system/libsystem_c.dylib)
     *  vsprintf_l (in /usr/lib/system/libsystem_c.dylib)
     *  __sprintf_chk (in /usr/lib/system/libsystem_c.dylib)
     *  _num_internal_tocstring (numobject.c)
     */

    // Grisu 3
    sprintf(buffer, "%g", v->ob_ival);
}

static int
num_print(JsNumObject *v, FILE *fp)
{
    char buf[NUM_BUFFERSIZE] = {0};
    _num_internal_tocstring(v, buf);
    fprintf(fp, "%s", buf);
    return 0;
}

static JsObject *
num_tostring(JsNumObject *v) 
{
    char buf[NUM_BUFFERSIZE] = {0};
    _num_internal_tocstring(v, buf);
    return JsString_FromString(buf);
}

static int
num_compare(JsNumObject *v, JsNumObject *w)
{
    register double i = v->ob_ival;
    register double j = w->ob_ival;
    return (i < j) ? -1 : (i > j) ? 1 : 0;
}

static uhash
num_hash(JsNumObject *v)
{
    u64 xlong = *((u64*)&(v->ob_ival));
#ifdef HASH_AS_LONG
    return (uhash)xlong;
#else
    uhash ret = (uhash)(xlong ^ (xlong>>32));
    return ret;
#endif
}

int errnoInNum;

JsTypeObject JsNum_Type =
{
    JsObject_HEAD_INIT(&JsType_Type)
    "number",
    sizeof(JsNumObject),
    0,

    JS_TPFLAGS_DEFAULT | JS_TPFLAGS_BASETYPE |
        JS_TPFLAGS_INT_SUBCLASS,

    NULL,                               /* no new func, generate by api*/
    (destructor)num_dealloc,            /* tp_dealloc */
    (printfunc)num_print,               /* tp_print */
    (tostringfunc)num_tostring,         /* tp_tostr */

    (cmpfunc)num_compare,               /* tp_compare */
    (hashfunc)num_hash,                 /* tp_hash */
};

// return -1 if error
int
_JsNum_Init(void)
{
    JsNumObject *v;
    int ival;
#if NSMALLNEGINTS + NSMALLPOSINTS > 0
    v = malloc(sizeof(JsNumObject)*(NSMALLNEGINTS + NSMALLPOSINTS));
    if (v == NULL)
    {
        dbgprint("Initialization of numobject failed");
        return -1;
    }
    for (ival = -NSMALLNEGINTS; ival < NSMALLPOSINTS; ival++, v++) {
        JsObject_INIT(v, &JsNum_Type);
        v->ob_ival = ival;
        small_ints[ival + NSMALLNEGINTS] = v;
    }
#endif
    return 0;
}

void
_JsNum_Deinit(void)
{
#if NSMALLNEGINTS + NSMALLPOSINTS > 0
    if (small_ints[0]!=NULL)
        free(small_ints[0]);
#endif
}

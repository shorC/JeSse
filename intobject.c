#include "intobject.h"

#ifndef NSMALLPOSINTS
#define NSMALLPOSINTS           257
#endif
#ifndef NSMALLNEGINTS
#define NSMALLNEGINTS           7
#endif
#if NSMALLNEGINTS + NSMALLPOSINTS > 0
/* References to small integers are saved in this array so that they
   can be shared.
   The integers that are saved are those in the range
   -NSMALLNEGINTS (inclusive) to NSMALLPOSINTS (not inclusive).

*/
static JsIntObject *small_ints[NSMALLNEGINTS + NSMALLPOSINTS];
#endif

// c-api
JsTypeObject* JsInt_FromLong(long ival)
{
	return NULL;
}

// methods
static JsObject *
int_new(JsTypeObject *type, JsObject *args, JsObject *kwds)
{
	return NULL;
}

static void
int_dealloc(JsIntObject *type)
{
	// free something
}

static int
int_print(JsIntObject *v, FILE *fp, int flags)
     /* flags -- not used but required by interface */
{
    long int_val = v->ob_ival;
    fprintf(fp, "%ld", int_val);
    return 0;
}

static JsObject *
int_to_decimal_string(JsIntObject *v) {
    char buf[sizeof(long)*CHAR_BIT/3+6], *p, *bufend;
    long n = v->ob_ival;
    unsigned long absn;
    p = bufend = buf + sizeof(buf);
    absn = n < 0 ? 0UL - n : n;
    do {
        *--p = '0' + (char)(absn % 10);
        absn /= 10;
    } while (absn);
    if (n < 0)
        *--p = '-';
    // TODO:
    //return JsString_FromStringAndSize(p, bufend - p);
    return NULL;
}

static int
int_compare(JsIntObject *v, JsIntObject *w)
{
    register long i = v->ob_ival;
    register long j = w->ob_ival;
    return (i < j) ? -1 : (i > j) ? 1 : 0;
}

static long
int_hash(JsIntObject *v)
{
	long x = v -> ob_ival;
	if (x == -1)
		x = -2;
	return x;
}

JsTypeObject JsInt_Type =
{
	JsObject_HEAD_INIT(&JsType_Type)
	"int",
    sizeof(JsIntObject),
    0,

    JS_TPFLAGS_DEFAULT | JS_TPFLAGS_BASETYPE,

	int_new,							/* tp_new */
	(destructor)int_dealloc,			/* tp_dealloc */
	(printfunc)int_print,				/* tp_print */
	(tostringfunc)int_to_decimal_string,/* tp_tostr */

	(cmpfunc)int_compare,				/* tp_compare */
	(hashfunc)int_hash,					/* tp_hash */
};

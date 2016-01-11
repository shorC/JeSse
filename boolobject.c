#include <stdlib.h>
#include <string.h>

#include "runtime.h"

#define VAR_BOOL_TRUE "true"
#define VAR_BOOL_FALSE "false"

static JsObject *true_str;
static JsObject *false_str;

JsObject*
JsBool_FromInt(int val)
{
	JsObject* ret;
	if (val)
		ret = Js_True;
	else
		ret = Js_False;

	Js_INCREF(ret);
	return ret;
}

int
JsBool_GetBool(JsObject *obj)
{
	return ((JsBoolObject*)obj)->ob_ival;
}

static int
bool_print(JsBoolObject *obj, FILE *fp)
{
	fputs(obj->ob_ival == 0 ? VAR_BOOL_FALSE : VAR_BOOL_TRUE, fp);
	return 0;
}

static JsObject *
bool_tostring(JsBoolObject *v)
{
	JsObject* ret;
	if (v->ob_ival)
		ret = true_str;
	else
		ret = false_str;

	Js_INCREF(ret);
	return ret;
}

JsTypeObject JsBool_Type =
{
	JsObject_HEAD_INIT(&JsType_Type)
	"boolean",
	sizeof(JsBoolObject),
	0,

	JS_TPFLAGS_DEFAULT | JS_TPFLAGS_BASETYPE,

	NULL,								/* no new func, generate by api*/
	NULL,								/* tp_dealloc */
	(printfunc)bool_print,				/* tp_print */
	(tostringfunc)bool_tostring,		/* tp_tostr */

	NULL,								/* tp_compare */
	NULL,								/* tp_hash */
};

int
_JsBool_Init(void)
{
	JsObject *v, *w;

	v = JsString_FromString(VAR_BOOL_TRUE);
	if (v == NULL)
	{
		dbgprint("Initialization of boolobject failed");
		return -1;
	}
	w = JsString_FromString(VAR_BOOL_FALSE);
	if (w == NULL)
	{
		dbgprint("Initialization of boolobject failed");
		return -1;
	}

	true_str = v;
	false_str = w;

	return 0;
}

void
_JsBool_Deinit(void)
{
	if (true_str != NULL && false_str != NULL)
	{
		free(true_str);
		free(false_str);
	}
}

JsBoolObject _Js_FalseStruct = {
    JsObject_HEAD_INIT(&JsBool_Type)
    0
};

JsBoolObject _Js_TrueStruct = {
    JsObject_HEAD_INIT(&JsBool_Type)
    1
};
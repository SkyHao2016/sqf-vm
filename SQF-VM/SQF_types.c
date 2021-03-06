#include "basetype.h"
#include "vector.h"
#include "string_op.h"
#include "string_map.h"
#include "SQF.h"
#include "SQF_types.h"
#include "SQF_object_type.h"
#include "SQF_side_type.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void TYPE_CODE_CALLBACK(void* input, CPCMD self);
PCMD SCALAR_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("SCALAR", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD BOOL_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("BOOL", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD IF_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("IF", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD WHILE_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("WHILE", 't', TYPE_CODE_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD NOTHING_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("NOTHING", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD ANY_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("ANY", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD NAN_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("NaN", 't', 0, 0, NULL, NULL, NULL);
	}
	return cmd;
}





void TYPE_CODE_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PCODE code = val->val.ptr;
	if (val->type == 0)
	{
		code->refcount--;
		if (code->refcount <= 0)
		{
			code_destroy(code);
		}
	}
	else
	{
		code->refcount++;
	}
}
PCMD CODE_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("CODE", 't', TYPE_CODE_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCODE code_create(const char* txt, int offset, int len)
{
	PCODE code = malloc(sizeof(CODE));
	code->length = len;
	code->val = malloc(sizeof(char) * (len + 1));
	strncpy(code->val, txt + offset, len);
	code->val[len] = '\0';
	code->refcount = 0;
	code->stack = create_stack(0, 1);
	return code;
}
void code_destroy(PCODE code)
{
	free(code->val);
	destroy_stack(code->stack);
	free(code);
}


void TYPE_STRING_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PSTRING string = val->val.ptr;
	if (val->type == 0)
	{
		string->refcount--;
		if (string->refcount <= 0)
		{
			string_destroy(string);
		}
	}
	else
	{
		string->refcount++;
	}
}
PCMD STRING_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("STRING", 't', TYPE_STRING_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PSTRING string_create(unsigned int len)
{
	PSTRING string = malloc(sizeof(STRING));
	string->length = len;
	string->val = 0;
	string->refcount = 0;
	if (len > 0)
	{
		string->val = malloc(sizeof(char) * (len + 1));
		if (string->val == 0)
		{
			string->length = 0;
		}
		else
		{
			memset(string->val, 0, sizeof(char) * (len + 1));
		}
	}
	return string;
}
PSTRING string_create2(const char* str)
{
	int len = strlen(str);
	PSTRING string = string_create(len);
	if (string->length > 0)
	{
		strcpy(string->val, str);
	}
	return string;
}
void string_destroy(PSTRING string)
{
	if (string->val != 0)
	{
		free(string->val);
	}
	free(string);
}
PSTRING string_concat(const PSTRING l, const PSTRING r)
{
	PSTRING string = string_create(l->length + r->length);
	if (string->length == 0)
		return string;
	if (l->length != 0)
	{
		strcpy(string->val, l->val);
	}
	if (r->length != 0)
	{
		strcpy(string->val + l->length, r->val);
	}
	return string;
}
PSTRING string_substring(const PSTRING string, unsigned int start, int length)
{
	if (length < 0)
	{
		length = string->length - start;
	}
	PSTRING str = string_create(length);
	if (str->length == 0)
		return str;
	strncpy(str->val, string->val + start, length);
	return str;
}
void string_modify_append(PSTRING string, const char* append)
{
	if (append == 0)
		return;
	unsigned int len = strlen(append);
	char* ptr;
	if (len == 0)
		return;
	ptr = realloc(string->val, sizeof(char) * (string->length + len + 1));
	if (ptr == 0)
		return;
	string->val = ptr;
	strcpy(string->val + string->length, append);
	string->length += len;
	string->val[string->length] = '\0';
}
void string_modify_nappend(PSTRING string, const char* append, unsigned int len)
{
	if (append == 0 || len == 0)
		return;
	char* ptr = realloc(string->val, sizeof(char) * (string->length + len + 1));
	if (ptr == 0)
		return;
	string->val = ptr;
	strncpy(string->val + string->length, append, len);
	string->length += len;
	string->val[string->length] = '\0';
}
void string_modify_append2(PSTRING string, int len)
{
	if (len == 0)
		return;
	char* ptr;
	ptr = realloc(string->val, sizeof(char) * (string->length + len + 1));
	if (ptr == 0)
		return;
	string->val = ptr;
	string->length += len;
	string->val[string->length] = '\0';
}


void TYPE_ARRAY_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PARRAY arr = val->val.ptr;
	if (val->type == 0)
	{
		arr->refcount--;
		if (arr->refcount <= 0)
		{
			array_destroy(arr);
		}
	}
	else
	{
		arr->refcount++;
	}
}
PCMD ARRAY_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("ARRAY", 't', TYPE_ARRAY_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PARRAY array_create2(unsigned int initialsize)
{
	PARRAY arr = malloc(sizeof(ARRAY));
	arr->data = malloc(sizeof(PVALUE) * initialsize);
	arr->size = initialsize;
	arr->top = 0;
	arr->refcount = 0;
	return arr;
}
PARRAY array_create(void)
{
	return array_create2(ARRAY_DEFAULT_INC);
}
void array_destroy(PARRAY arr)
{
	int i;
	for (i = 0; i < arr->top; i++)
	{
		inst_destroy_value(arr->data[i]);
	}
	free(arr->data);
	free(arr);
}
void array_resize(PARRAY arr, unsigned int newsize)
{
	PVALUE* data = realloc(arr->data, sizeof(PVALUE) * newsize);
	if (data != 0)
	{
		arr->data = data;
		arr->size = newsize;
	}
}
void array_push(PARRAY arr, VALUE val)
{
	if (arr->top >= arr->size)
	{
		array_resize(arr, arr->size + ARRAY_DEFAULT_INC);
	}
	arr->data[arr->top] = malloc(sizeof(VALUE));
	arr->data[arr->top]->type = val.type;
	arr->data[arr->top]->val = val.val;
	arr->top++;
};
PARRAY array_copy(const PARRAY arrIn)
{
	PARRAY arrOut = array_create2(arrIn->top);
	int i;
	PVALUE val;
	PARRAY tmp;
	for (i = 0; i < arrIn->top; i++)
	{
		val = arrIn->data[i];
		if (val->type == ARRAY_TYPE())
		{
			tmp = array_copy(val->val.ptr);
			array_push(arrOut, value(ARRAY_TYPE(), base_voidptr(tmp)));
		}
		else
		{
			array_push(arrOut, value(val->type, val->val));
		}
	}
	return arrOut;
}







void TYPE_FOR_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PFOR f = val->val.ptr;
	if (val->type == 0)
	{
		f->refcount--;
		if (f->refcount <= 0)
		{
			for_destroy(f);
		}
	}
	else
	{
		f->refcount++;
	}
}
PCMD FOR_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("FOR", 't', TYPE_FOR_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PFOR for_create(const char* varname)
{
	PFOR f = malloc(sizeof(FOR));
	int len = strlen(varname);
	f->variable = malloc(sizeof(char) * (len + 1));
	strcpy(f->variable, varname);
	f->variable[len] = '\0';
	f->variable_length = len;
	f->start = 0;
	f->end = 0;
	f->step = 1;
	f->started = 0;
	f->refcount = 0;

	return f;
}
void for_destroy(PFOR f)
{
	if (f->variable != 0)
	{
		free(f->variable);
	}
	free(f);
}


void TYPE_NAMESPACE_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PNAMESPACE namespace = val->val.ptr;
	if (val->type == 0)
	{
		namespace->refcount--;
		if (namespace->refcount <= 0)
		{
			namespace_destroy(namespace);
		}
	}
	else
	{
		namespace->refcount++;
	}
}
PCMD NAMESPACE_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("NAMESPACE", 't', TYPE_NAMESPACE_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PCMD WITH_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("WITH", 't', TYPE_NAMESPACE_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PNAMESPACE namespace_create(void)
{
	PNAMESPACE namespace = malloc(sizeof(NAMESPACE));
	namespace->refcount = 0;
	namespace->data = sm_create_list(74, 10, 10);
	return namespace;
}
void NAMESPACE_SM_LIST_DESTROY(void* ptr)
{
	PVALUE val = ptr;
	if (ptr == 0)
		return;
	inst_destroy_value(val);
}
void namespace_destroy(PNAMESPACE namespace)
{
	sm_destroy_list(namespace->data, NAMESPACE_SM_LIST_DESTROY);
	free(namespace);
}
void namespace_set_var(PNAMESPACE namespace, const char* var, VALUE val)
{
	if (val.type == NOTHING_TYPE())
	{
		NAMESPACE_SM_LIST_DESTROY(sm_drop_value(namespace->data, var));
	}
	else
	{
		NAMESPACE_SM_LIST_DESTROY(sm_set_value(namespace->data, var, value_create_noref(val.type, val.val)));
	}
}
PVALUE namespace_get_var(PNAMESPACE namespace, const char* var)
{
	return sm_get_value(namespace->data, var);
}


PNAMESPACE sqf_missionNamespace(void)
{
	static PNAMESPACE ns = 0;
	if (ns == 0)
	{
		ns = namespace_create();
		ns->refcount++;
	}
	return ns;
}
PNAMESPACE sqf_uiNamespace(void)
{
	static PNAMESPACE ns = 0;
	if (ns == 0)
	{
		ns = namespace_create();
		ns->refcount++;
	}
	return ns;
}
PNAMESPACE sqf_profileNamespace(void)
{
	static PNAMESPACE ns = 0;
	if (ns == 0)
	{
		ns = namespace_create();
		ns->refcount++;
	}
	return ns;
}
PNAMESPACE sqf_parsingNamespace(void)
{
	static PNAMESPACE ns = 0;
	if (ns == 0)
	{
		ns = namespace_create();
		ns->refcount++;
	}
	return ns;
}



void TYPE_SWITCH_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PSWITCH swtch = val->val.ptr;
	if (val->type == 0)
	{
		swtch->refcount--;
		if (swtch->refcount <= 0)
		{
			switch_destroy(swtch);
		}
	}
	else
	{
		swtch->refcount++;
	}
}
PCMD SWITCH_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("SWITCH", 't', TYPE_SWITCH_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PSWITCH switch_create(VALUE val)
{
	PSWITCH swtch = malloc(sizeof(SWITCH));
	swtch->default_code = 0;
	swtch->selected_code = 0;
	swtch->refcount = 0;
	swtch->was_executed = 0;
	swtch->switch_value = malloc(sizeof(VALUE));
	swtch->switch_value->type = val.type;
	swtch->switch_value->val = val.val;
	return swtch;
}
void switch_destroy(PSWITCH swtch)
{
	if (swtch->default_code != 0)
	{
		inst_destroy_value(swtch->default_code);
	}
	if (swtch->selected_code != 0)
	{
		inst_destroy_value(swtch->selected_code);
	}
	inst_destroy_value(swtch->switch_value);
	free(swtch);
}


void TYPE_GROUP_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PGROUP group = val->val.ptr;
	if (val->type == 0)
	{
		group->refcount--;
		if (group->refcount <= 0)
		{
			group_destroy(group);
		}
	}
	else
	{
		group->refcount++;
	}
}
PCMD GROUP_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("GROUP", 't', TYPE_GROUP_CALLBACK, 0, NULL, NULL, NULL);
	}
	return cmd;
}
PGROUP group_create(int side)
{
	static count = 1;
	PGROUP group = malloc(sizeof(GROUP));
	group->refcount = 0;
	group->members = value_create(ARRAY_TYPE(), base_voidptr(array_create()));
	group->side = value_create(SIDE_TYPE(), base_int(side));
	group->ident_len = snprintf(0, 0, "%c ALPHA %d", side_displayname(side)[0], count);
	group->ident = malloc(sizeof(char) * (group->ident_len + 1));
	snprintf(group->ident, group->ident_len + 1, "%c ALPHA %d", side_displayname(side)[0], count);
	count++;
	return group;
}
void group_destroy(PGROUP group)
{
	inst_destroy_value(group->members);
	inst_destroy_value(group->side);
	free(group->ident);
	free(group);
}

PVALUE group_get_leader(PGROUP group)
{
	if (((PARRAY)group->members->val.ptr)->top == 0)
		return 0;
	else
		return ((PARRAY)group->members->val.ptr)->data[0];
}
PGROUP group_from_ident(PVM vm, const char* ident)
{
	PGROUP grp;
	int i;
	int j = sm_count(vm->groupmap);
	for (i = 0; i < j; i++)
	{
		grp = ((PVALUE)sm_get_value_index(vm->groupmap, i))->val.ptr;
		if (str_cmpi(grp->ident, -1, ident, -1))
		{
			return grp;
		}
	}
	return 0;
}


//NON-SQF TYPES

void TYPE_COUNT_CALLBACK(void* input, CPCMD self)
{
	PVALUE val = input;
	PCOUNT count = val->val.ptr;
	if (val->type == 0)
	{
		count->refcount--;
		if (count->refcount <= 0)
		{
			count_destroy(count);
		}
	}
	else
	{
		count->refcount++;
	}
}
PCMD COUNT_TYPE(void)
{
	static PCMD cmd = 0;
	if (cmd == 0)
	{
		cmd = create_command("COUNT__", 't', TYPE_COUNT_CALLBACK, 0, NULL, NULL, "non - sqf compliant helper type");
	}
	return cmd;
}
PCOUNT count_create(PCODE code, PARRAY arr)
{
	PCOUNT count = malloc(sizeof(COUNT));

	PVALUE pvalue = malloc(sizeof(VALUE));
	VALUE val = value(CODE_TYPE(), base_voidptr(code));
	pvalue->type = val.type;
	pvalue->val = val.val;
	count->code = pvalue;

	pvalue = malloc(sizeof(VALUE));
	val = value(ARRAY_TYPE(), base_voidptr(arr));
	pvalue->type = val.type;
	pvalue->val = val.val;
	count->arr = pvalue;

	count->curtop = 0;
	count->refcount = 0;
	count->count = 0;
	return count;
}
void count_destroy(PCOUNT count)
{
	inst_destroy_value(count->code);
	inst_destroy_value(count->arr);
	free(count);
}
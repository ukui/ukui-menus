/*
 * Copyright (C) 2005 Red Hat, Inc.
 * Copyright (C) 2017,Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <config.h>

#include <Python.h>
#include <ukuimenu-tree.h>

typedef struct {
	PyObject_HEAD
	UkuiMenuTree* tree;
	GSList* callbacks;
} PyUkuiMenuTree;

typedef struct {
	PyObject* tree;
	PyObject* callback;
	PyObject* user_data;
} PyUkuiMenuTreeCallback;

typedef struct {
	PyObject_HEAD
	UkuiMenuTreeItem* item;
} PyUkuiMenuTreeItem;

typedef PyUkuiMenuTreeItem PyUkuiMenuTreeDirectory;
typedef PyUkuiMenuTreeItem PyUkuiMenuTreeEntry;
typedef PyUkuiMenuTreeItem PyUkuiMenuTreeSeparator;
typedef PyUkuiMenuTreeItem PyUkuiMenuTreeHeader;
typedef PyUkuiMenuTreeItem PyUkuiMenuTreeAlias;

static PyUkuiMenuTree* pyukuimenu_tree_wrap(UkuiMenuTree* tree);
static PyUkuiMenuTreeDirectory* pyukuimenu_tree_directory_wrap(UkuiMenuTreeDirectory* directory);
static PyUkuiMenuTreeEntry* pyukuimenu_tree_entry_wrap(UkuiMenuTreeEntry* entry);
static PyUkuiMenuTreeSeparator* pyukuimenu_tree_separator_wrap(UkuiMenuTreeSeparator* separator);
static PyUkuiMenuTreeHeader* pyukuimenu_tree_header_wrap(UkuiMenuTreeHeader* header);
static PyUkuiMenuTreeAlias* pyukuimenu_tree_alias_wrap(UkuiMenuTreeAlias* alias);

static inline PyObject* lookup_item_type_str(const char* item_type_str)
{
	PyObject* module;

	module = PyDict_GetItemString(PyImport_GetModuleDict(), "ukuimenu");

	return PyDict_GetItemString(PyModule_GetDict(module), item_type_str);
}

static void pyukuimenu_tree_item_dealloc(PyUkuiMenuTreeItem* self)
{
	if (self->item != NULL)
	{
		ukuimenu_tree_item_set_user_data(self->item, NULL, NULL);
		ukuimenu_tree_item_unref(self->item);
		self->item = NULL;
	}

	PyObject_DEL (self);
}

static PyObject* pyukuimenu_tree_item_get_type(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeItem* item;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Item.get_type"))
		{
			return NULL;
		}
	}

	item = (PyUkuiMenuTreeItem*) self;

	switch (ukuimenu_tree_item_get_type(item->item))
	{
		case UKUIMENU_TREE_ITEM_DIRECTORY:
			retval = lookup_item_type_str("TYPE_DIRECTORY");
			break;

		case UKUIMENU_TREE_ITEM_ENTRY:
			retval = lookup_item_type_str("TYPE_ENTRY");
			break;

		case UKUIMENU_TREE_ITEM_SEPARATOR:
			retval = lookup_item_type_str("TYPE_SEPARATOR");
			break;

		case UKUIMENU_TREE_ITEM_HEADER:
			retval = lookup_item_type_str("TYPE_HEADER");
			break;

		case UKUIMENU_TREE_ITEM_ALIAS:
			retval = lookup_item_type_str("TYPE_ALIAS");
			break;

		default:
			g_assert_not_reached();
			break;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pyukuimenu_tree_item_get_parent(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeItem* item;
	UkuiMenuTreeDirectory* parent;
	PyUkuiMenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Item.get_parent"))
		{
			return NULL;
		}
	}

	item = (PyUkuiMenuTreeItem*) self;

	parent = ukuimenu_tree_item_get_parent(item->item);

	if (parent == NULL)
	{
		Py_INCREF(Py_None);

		return Py_None;
	}

	retval = pyukuimenu_tree_directory_wrap(parent);

	ukuimenu_tree_item_unref(parent);

	return (PyObject*) retval;
}

static struct PyMethodDef pyukuimenu_tree_item_methods[] = {
	{"get_type", pyukuimenu_tree_item_get_type,   METH_VARARGS},
	{"get_parent", pyukuimenu_tree_item_get_parent, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTreeItem_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Item",                               /* tp_name */
	sizeof(PyUkuiMenuTreeItem),                    /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,     /* tp_dealloc */
	0,              		               /* tp_print */
	0,                              	       /* tp_getattr */
	0,                         		       /* tp_setattr */
	0,                  			       /* tp_reserved */
	0,                            		       /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	0,                            		       /* tp_hash */
	0,                          		       /* tp_call */
	0,                              	       /* tp_str */
	0,                         		       /* tp_getattro */
	0,                           		       /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,      /* tp_flags */
	NULL,                                          /* Documentation string */
	0,                          		       /* tp_traverse */
	0,                              	       /* tp_clear */
	0,                         		       /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	0,                         		       /* tp_iter */
	0,                            		       /* tp_iternext */
	pyukuimenu_tree_item_methods,                  /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	0,                        		       /* tp_base */
	0,                             		       /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
};

static PyObject* pyukuimenu_tree_directory_get_contents(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	PyObject* retval;
	GSList* contents;
	GSList* tmp;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_contents"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	retval = PyList_New(0);

	contents = ukuimenu_tree_directory_get_contents(UKUIMENU_TREE_DIRECTORY(directory->item));

	tmp = contents;

	while (tmp != NULL)
	{
		UkuiMenuTreeItem* item = tmp->data;
		PyObject* pyitem;

		switch (ukuimenu_tree_item_get_type(item))
		{
			case UKUIMENU_TREE_ITEM_DIRECTORY:
				pyitem = (PyObject*) pyukuimenu_tree_directory_wrap(UKUIMENU_TREE_DIRECTORY(item));
				break;

			case UKUIMENU_TREE_ITEM_ENTRY:
				pyitem = (PyObject*) pyukuimenu_tree_entry_wrap(UKUIMENU_TREE_ENTRY(item));
				break;

			case UKUIMENU_TREE_ITEM_SEPARATOR:
				pyitem = (PyObject*) pyukuimenu_tree_separator_wrap(UKUIMENU_TREE_SEPARATOR(item));
				break;

			case UKUIMENU_TREE_ITEM_HEADER:
				pyitem = (PyObject*) pyukuimenu_tree_header_wrap(UKUIMENU_TREE_HEADER(item));
				break;

			case UKUIMENU_TREE_ITEM_ALIAS:
				pyitem = (PyObject*) pyukuimenu_tree_alias_wrap(UKUIMENU_TREE_ALIAS(item));
				break;

			default:
				g_assert_not_reached();
				break;
		}

		PyList_Append(retval, pyitem);
		Py_DECREF(pyitem);

		ukuimenu_tree_item_unref(item);

		tmp = tmp->next;
	}

	g_slist_free(contents);

	return retval;
}

static PyObject* pyukuimenu_tree_directory_get_name(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	const char* name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_name"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	name = ukuimenu_tree_directory_get_name(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(name);
}

static PyObject* pyukuimenu_tree_directory_get_comment(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	const char* comment;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_comment"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	comment = ukuimenu_tree_directory_get_comment(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (comment == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(comment);
}

static PyObject* pyukuimenu_tree_directory_get_icon(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	const char* icon;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_icon"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	icon = ukuimenu_tree_directory_get_icon(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (icon == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
    }

	return PyBytes_FromString(icon);
}

static PyObject* pyukuimenu_tree_directory_get_desktop_file_path(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	const char* path;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_desktop_file_path"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	path = ukuimenu_tree_directory_get_desktop_file_path(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(path);
}

static PyObject* pyukuimenu_tree_directory_get_menu_id(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	const char* menu_id;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Directory.get_menu_id"))
		{
			return NULL;
		}
    }

	directory = (PyUkuiMenuTreeDirectory*) self;

	menu_id = ukuimenu_tree_directory_get_menu_id(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (menu_id == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(menu_id);
}

static PyObject* pyukuimenu_tree_directory_get_tree(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	UkuiMenuTree* tree;
	PyUkuiMenuTree* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Item.get_tree"))
		{
			return NULL;
		}
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	tree = ukuimenu_tree_directory_get_tree(UKUIMENU_TREE_DIRECTORY(directory->item));

	if (tree == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pyukuimenu_tree_wrap(tree);

	ukuimenu_tree_unref(tree);

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_directory_make_path(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeDirectory* directory;
	PyUkuiMenuTreeEntry* entry;
	PyObject* retval;
	char* path;

	if (!PyArg_ParseTuple(args, "O:ukuimenu.Directory.make_path", &entry))
	{
		return NULL;
	}

	directory = (PyUkuiMenuTreeDirectory*) self;

	path = ukuimenu_tree_directory_make_path(UKUIMENU_TREE_DIRECTORY(directory->item), UKUIMENU_TREE_ENTRY(entry->item));

	if (path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = PyBytes_FromString(path);

	g_free(path);

	return retval;
}

static PyObject* pyukuimenu_tree_directory_getattro(PyUkuiMenuTreeDirectory* self, PyObject* py_attr)
{
	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sssssssss]",
				"type",
				"parent",
				"contents",
				"name",
				"comment",
				"icon",
				"desktop_file_path",
				"menu_id",
				"tree");
		}
		else if (!strcmp(attr, "type"))
		{
			return pyukuimenu_tree_item_get_type((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "parent"))
		{
			return pyukuimenu_tree_item_get_parent((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "contents"))
		{
			return pyukuimenu_tree_directory_get_contents((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "name"))
		{
			return pyukuimenu_tree_directory_get_name((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "comment"))
		{
			return pyukuimenu_tree_directory_get_comment((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "icon"))
		{
			return pyukuimenu_tree_directory_get_icon((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "desktop_file_path"))
		{
			return pyukuimenu_tree_directory_get_desktop_file_path((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "menu_id"))
		{
			return pyukuimenu_tree_directory_get_menu_id((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "tree"))
		{
			return pyukuimenu_tree_directory_get_tree((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pyukuimenu_tree_directory_methods[] = {
	{"get_contents", pyukuimenu_tree_directory_get_contents, METH_VARARGS},
	{"get_name", pyukuimenu_tree_directory_get_name, METH_VARARGS},
	{"get_comment", pyukuimenu_tree_directory_get_comment, METH_VARARGS},
	{"get_icon", pyukuimenu_tree_directory_get_icon, METH_VARARGS},
	{"get_desktop_file_path", pyukuimenu_tree_directory_get_desktop_file_path, METH_VARARGS},
	{"get_menu_id", pyukuimenu_tree_directory_get_menu_id, METH_VARARGS},
	{"get_tree", pyukuimenu_tree_directory_get_tree, METH_VARARGS},
	{"make_path", pyukuimenu_tree_directory_make_path, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTreeDirectory_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Directory",                           /* tp_name */
	sizeof(PyUkuiMenuTreeDirectory),                /* tp_basicsize */
	0,                                              /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,      /* tp_dealloc */
	(printfunc) 0,                                  /* tp_print */
	(getattrfunc) 0,                                /* tp_getattr */
	(setattrfunc) 0,                                /* tp_setattr */
	(PyAsyncMethods *) 0,                             /* tp_reserved */
	(reprfunc) 0,                                   /* tp_repr */
	0,                                              /* tp_as_number */
	0,                                              /* tp_as_sequence */
	0,                                              /* tp_as_mapping */
	(hashfunc) 0,                                   /* tp_hash */
	(ternaryfunc) 0,                                /* tp_call */
	(reprfunc) 0,                                   /* tp_str */
	(getattrofunc) pyukuimenu_tree_directory_getattro, /* tp_getattro */
	(setattrofunc) 0,                               /* tp_setattro */
	0,                                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                             /* tp_flags */
	NULL,                                           /* Documentation string */
	(traverseproc) 0,                               /* tp_traverse */
	(inquiry) 0,                                    /* tp_clear */
	(richcmpfunc) 0,                                /* tp_richcompare */
	0,                                              /* tp_weaklistoffset */
	(getiterfunc) 0,                                /* tp_iter */
	(iternextfunc) 0,                               /* tp_iternext */
	pyukuimenu_tree_directory_methods,              /* tp_methods */
	0,                                              /* tp_members */
	0,                                              /* tp_getset */
	(PyTypeObject*) 0,                              /* tp_base */
	(PyObject*) 0,                                  /* tp_dict */
	0,                                              /* tp_descr_get */
	0,                                              /* tp_descr_set */
	0,                                              /* tp_dictoffset */
	(initproc) 0,                                   /* tp_init */
	0,                                              /* tp_alloc */
	0,                                              /* tp_new */
};

static PyUkuiMenuTreeDirectory* pyukuimenu_tree_directory_wrap(UkuiMenuTreeDirectory* directory)
{
	PyUkuiMenuTreeDirectory* retval;

	if ((retval = ukuimenu_tree_item_get_user_data(UKUIMENU_TREE_ITEM(directory))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTreeDirectory*) PyObject_NEW(PyUkuiMenuTreeDirectory, &PyUkuiMenuTreeDirectory_Type)))
	{
		return NULL;
	}

	retval->item = ukuimenu_tree_item_ref(directory);

	ukuimenu_tree_item_set_user_data(UKUIMENU_TREE_ITEM(directory), retval, NULL);

	return retval;
}

static PyObject* pyukuimenu_tree_entry_get_name(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_name"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	name = ukuimenu_tree_entry_get_name(UKUIMENU_TREE_ENTRY(entry->item));

	if (name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(name);
}

static PyObject* pyukuimenu_tree_entry_get_generic_name(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* generic_name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_generic_name"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	generic_name = ukuimenu_tree_entry_get_generic_name(UKUIMENU_TREE_ENTRY(entry->item));

	if (generic_name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(generic_name);
}

static PyObject* pyukuimenu_tree_entry_get_display_name(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* display_name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_display_name"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	display_name = ukuimenu_tree_entry_get_display_name(UKUIMENU_TREE_ENTRY(entry->item));

	if (display_name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(display_name);
}

static PyObject* pyukuimenu_tree_entry_get_comment(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* comment;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_comment"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	comment = ukuimenu_tree_entry_get_comment(UKUIMENU_TREE_ENTRY(entry->item));

	if (comment == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(comment);
}

static PyObject* pyukuimenu_tree_entry_get_icon(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* icon;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_icon"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	icon = ukuimenu_tree_entry_get_icon(UKUIMENU_TREE_ENTRY(entry->item));

	if (icon == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(icon);
}

static PyObject* pyukuimenu_tree_entry_get_exec(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* exec;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_exec"))
		{
			return NULL;
		}
    }

	entry = (PyUkuiMenuTreeEntry*) self;

	exec = ukuimenu_tree_entry_get_exec(UKUIMENU_TREE_ENTRY(entry->item));

	if (exec == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(exec);
}

static PyObject* pyukuimenu_tree_entry_get_launch_in_terminal(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_launch_in_terminal"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	if (ukuimenu_tree_entry_get_launch_in_terminal(UKUIMENU_TREE_ENTRY(entry->item)))
	{
		retval = Py_True;
	}
	else
	{
		retval = Py_False;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pyukuimenu_tree_entry_get_desktop_file_path(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* desktop_file_path;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_desktop_file_path"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	desktop_file_path = ukuimenu_tree_entry_get_desktop_file_path(UKUIMENU_TREE_ENTRY(entry->item));

	if (desktop_file_path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(desktop_file_path);
}

static PyObject* pyukuimenu_tree_entry_get_desktop_file_id(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	const char* desktop_file_id;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_desktop_file_id"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	desktop_file_id = ukuimenu_tree_entry_get_desktop_file_id(UKUIMENU_TREE_ENTRY(entry->item));

	if (desktop_file_id == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(desktop_file_id);
}

static PyObject* pyukuimenu_tree_entry_get_is_excluded(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_is_excluded"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	retval = ukuimenu_tree_entry_get_is_excluded(UKUIMENU_TREE_ENTRY(entry->item)) ? Py_True : Py_False;
	Py_INCREF(retval);

	return retval;
}

static PyObject* pyukuimenu_tree_entry_get_is_nodisplay(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Entry.get_is_nodisplay"))
		{
			return NULL;
		}
	}

	entry = (PyUkuiMenuTreeEntry*) self;

	if (ukuimenu_tree_entry_get_is_nodisplay(UKUIMENU_TREE_ENTRY(entry->item)))
	{
		retval = Py_True;
	}
	else
	{
		retval = Py_False;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pyukuimenu_tree_entry_getattro(PyUkuiMenuTreeEntry* self, PyObject* py_attr)
{
	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sssssssssss]",
				"type",
				"parent",
				"name",
				"comment",
				"icon",
				"exec_info",
				"launch_in_terminal",
				"desktop_file_path",
				"desktop_file_id",
				"is_excluded",
				"is_nodisplay");
		}
		else if (!strcmp(attr, "type"))
		{
			return pyukuimenu_tree_item_get_type((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "parent"))
		{
			return pyukuimenu_tree_item_get_parent((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "name"))
		{
			return pyukuimenu_tree_entry_get_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "generic_name"))
		{
			return pyukuimenu_tree_entry_get_generic_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "display_name"))
		{
			return pyukuimenu_tree_entry_get_display_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "comment"))
		{
			return pyukuimenu_tree_entry_get_comment((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "icon"))
		{
			return pyukuimenu_tree_entry_get_icon((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "exec_info"))
		{
			return pyukuimenu_tree_entry_get_exec((PyObject*) self, NULL);
		}
			else if (!strcmp(attr, "launch_in_terminal"))
		{
			return pyukuimenu_tree_entry_get_launch_in_terminal((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "desktop_file_path"))
		{
			return pyukuimenu_tree_entry_get_desktop_file_path((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "desktop_file_id"))
		{
			return pyukuimenu_tree_entry_get_desktop_file_id((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "is_excluded"))
		{
			return pyukuimenu_tree_entry_get_is_excluded((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "is_nodisplay"))
		{
			return pyukuimenu_tree_entry_get_is_nodisplay((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pyukuimenu_tree_entry_methods[] = {
	{"get_name", pyukuimenu_tree_entry_get_name, METH_VARARGS},
	{"get_generic_name", pyukuimenu_tree_entry_get_generic_name, METH_VARARGS},
	{"get_display_name", pyukuimenu_tree_entry_get_display_name, METH_VARARGS},
	{"get_comment", pyukuimenu_tree_entry_get_comment, METH_VARARGS},
	{"get_icon", pyukuimenu_tree_entry_get_icon, METH_VARARGS},
	{"get_exec", pyukuimenu_tree_entry_get_exec, METH_VARARGS},
	{"get_launch_in_terminal", pyukuimenu_tree_entry_get_launch_in_terminal, METH_VARARGS},
	{"get_desktop_file_path", pyukuimenu_tree_entry_get_desktop_file_path, METH_VARARGS},
	{"get_desktop_file_id", pyukuimenu_tree_entry_get_desktop_file_id, METH_VARARGS},
	{"get_is_excluded", pyukuimenu_tree_entry_get_is_excluded, METH_VARARGS},
	{"get_is_nodisplay", pyukuimenu_tree_entry_get_is_nodisplay, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTreeEntry_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Entry",                              /* tp_name */
	sizeof(PyUkuiMenuTreeEntry),                   /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(PyAsyncMethods *) 0,                            /* tp_reserved */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pyukuimenu_tree_entry_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pyukuimenu_tree_entry_methods,                 /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
};

static PyUkuiMenuTreeEntry* pyukuimenu_tree_entry_wrap(UkuiMenuTreeEntry* entry)
{
	PyUkuiMenuTreeEntry* retval;

	if ((retval = ukuimenu_tree_item_get_user_data(UKUIMENU_TREE_ITEM(entry))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTreeEntry*) PyObject_NEW(PyUkuiMenuTreeEntry, &PyUkuiMenuTreeEntry_Type)))
	{
		return NULL;
	}

	retval->item = ukuimenu_tree_item_ref(entry);

	ukuimenu_tree_item_set_user_data(UKUIMENU_TREE_ITEM(entry), retval, NULL);

	return retval;
}

static PyTypeObject PyUkuiMenuTreeSeparator_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Separator",                          /* tp_name */
	sizeof(PyUkuiMenuTreeSeparator),               /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(PyAsyncMethods *) 0,                            /* tp_reserved */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) 0,                              /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	NULL,                                          /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
};

static PyUkuiMenuTreeSeparator* pyukuimenu_tree_separator_wrap(UkuiMenuTreeSeparator* separator)
{
	PyUkuiMenuTreeSeparator* retval;

	if ((retval = ukuimenu_tree_item_get_user_data(UKUIMENU_TREE_ITEM(separator))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTreeSeparator*) PyObject_NEW(PyUkuiMenuTreeSeparator, &PyUkuiMenuTreeSeparator_Type)))
	{
		return NULL;
	}

	retval->item = ukuimenu_tree_item_ref(separator);

	ukuimenu_tree_item_set_user_data(UKUIMENU_TREE_ITEM(separator), retval, NULL);

	return retval;
}

static PyObject* pyukuimenu_tree_header_get_directory(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeHeader* header;
	UkuiMenuTreeDirectory* directory;
	PyUkuiMenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Header.get_directory"))
		{
			return NULL;
		}
	}

	header = (PyUkuiMenuTreeHeader*) self;

	directory = ukuimenu_tree_header_get_directory(UKUIMENU_TREE_HEADER(header->item));

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
    }

	retval = pyukuimenu_tree_directory_wrap(directory);

	ukuimenu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_header_getattro(PyUkuiMenuTreeHeader* self, PyObject* py_attr)
{
	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sss]",
				"type",
				"parent",
				"directory");
		}
		else if (!strcmp(attr, "type"))
		{
			return pyukuimenu_tree_item_get_type((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "parent"))
		{
			return pyukuimenu_tree_item_get_parent((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "directory"))
		{
			return pyukuimenu_tree_header_get_directory((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pyukuimenu_tree_header_methods[] = {
	{"get_directory", pyukuimenu_tree_header_get_directory, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTreeHeader_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Header",                             /* tp_name */
	sizeof(PyUkuiMenuTreeHeader),                  /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(PyAsyncMethods *) 0,                            /* tp_reserved */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pyukuimenu_tree_header_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pyukuimenu_tree_header_methods,                /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
};

static PyUkuiMenuTreeHeader* pyukuimenu_tree_header_wrap(UkuiMenuTreeHeader* header)
{
	PyUkuiMenuTreeHeader* retval;

	if ((retval = ukuimenu_tree_item_get_user_data(UKUIMENU_TREE_ITEM(header))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTreeHeader*) PyObject_NEW(PyUkuiMenuTreeHeader, &PyUkuiMenuTreeHeader_Type)))
	{
		return NULL;
	}

	retval->item = ukuimenu_tree_item_ref(header);

	ukuimenu_tree_item_set_user_data(UKUIMENU_TREE_ITEM(header), retval, NULL);

	return retval;
}

static PyObject* pyukuimenu_tree_alias_get_directory(PyObject*self, PyObject* args)
{
	PyUkuiMenuTreeAlias* alias;
	UkuiMenuTreeDirectory* directory;
	PyUkuiMenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Alias.get_directory"))
		{
			return NULL;
		}
	}

	alias = (PyUkuiMenuTreeAlias*) self;

	directory = ukuimenu_tree_alias_get_directory(UKUIMENU_TREE_ALIAS(alias->item));

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pyukuimenu_tree_directory_wrap(directory);

	ukuimenu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_alias_get_item(PyObject* self, PyObject* args)
{
	PyUkuiMenuTreeAlias* alias;
	UkuiMenuTreeItem* item;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Alias.get_item"))
		{
			return NULL;
		}
	}

	alias = (PyUkuiMenuTreeAlias*) self;

	item = ukuimenu_tree_alias_get_item(UKUIMENU_TREE_ALIAS(alias->item));

	if (item == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	switch (ukuimenu_tree_item_get_type(item))
	{
		case UKUIMENU_TREE_ITEM_DIRECTORY:
			retval = (PyObject*) pyukuimenu_tree_directory_wrap(UKUIMENU_TREE_DIRECTORY(item));
			break;

		case UKUIMENU_TREE_ITEM_ENTRY:
			retval = (PyObject*) pyukuimenu_tree_entry_wrap(UKUIMENU_TREE_ENTRY(item));
			break;

		default:
			g_assert_not_reached();
			break;
	}

	ukuimenu_tree_item_unref(item);

	return retval;
}

static PyObject* pyukuimenu_tree_alias_getattro(PyUkuiMenuTreeAlias* self, PyObject* py_attr)
{
	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[ssss]",
				"type",
				"parent",
				"directory",
				"item");
		}
		else if (!strcmp(attr, "type"))
		{
			return pyukuimenu_tree_item_get_type((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "parent"))
		{
			return pyukuimenu_tree_item_get_parent((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "directory"))
		{
			return pyukuimenu_tree_alias_get_directory((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "item"))
		{
			return pyukuimenu_tree_alias_get_item((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pyukuimenu_tree_alias_methods[] = {
	{"get_directory", pyukuimenu_tree_alias_get_directory, METH_VARARGS},
	{"get_item", pyukuimenu_tree_alias_get_item, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTreeAlias_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Alias",                              /* tp_name */
	sizeof(PyUkuiMenuTreeAlias),                   /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pyukuimenu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(PyAsyncMethods *) 0,                            /* tp_reserved */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pyukuimenu_tree_alias_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pyukuimenu_tree_alias_methods,                 /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
};

static PyUkuiMenuTreeAlias* pyukuimenu_tree_alias_wrap(UkuiMenuTreeAlias* alias)
{
	PyUkuiMenuTreeAlias* retval;

	if ((retval = ukuimenu_tree_item_get_user_data(UKUIMENU_TREE_ITEM(alias))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTreeAlias*) PyObject_NEW(PyUkuiMenuTreeAlias, &PyUkuiMenuTreeAlias_Type)))
	{
		return NULL;
	}

	retval->item = ukuimenu_tree_item_ref(alias);

	ukuimenu_tree_item_set_user_data(UKUIMENU_TREE_ITEM(alias), retval, NULL);

	return retval;
}

static PyObject* pyukuimenu_tree_get_menu_file(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	const char* menu_file;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Tree.get_menu_file"))
		{
			return NULL;
		}
	}

	tree = (PyUkuiMenuTree*) self;

	menu_file = ukuimenu_tree_get_menu_file(tree->tree);

	if (menu_file == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyBytes_FromString(menu_file);
}

static PyObject* pyukuimenu_tree_get_root_directory(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	UkuiMenuTreeDirectory* directory;
	PyUkuiMenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Tree.get_root_directory"))
		{
			return NULL;
		}
	}

	tree = (PyUkuiMenuTree*) self;

	directory = ukuimenu_tree_get_root_directory(tree->tree);

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pyukuimenu_tree_directory_wrap (directory);

	ukuimenu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_get_directory_from_path(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	UkuiMenuTreeDirectory* directory;
	PyUkuiMenuTreeDirectory* retval;
	char* path;

	if (!PyArg_ParseTuple(args, "s:ukuimenu.Tree.get_directory_from_path", &path))
	{
		return NULL;
	}

	tree = (PyUkuiMenuTree*) self;

	directory = ukuimenu_tree_get_directory_from_path(tree->tree, path);

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pyukuimenu_tree_directory_wrap(directory);

	ukuimenu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_get_sort_key(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":ukuimenu.Tree.get_sort_key"))
		{
			return NULL;
		}
	}

	tree = (PyUkuiMenuTree*) self;

	switch (ukuimenu_tree_get_sort_key(tree->tree))
	{
		case UKUIMENU_TREE_SORT_NAME:
			retval = lookup_item_type_str("SORT_NAME");
			break;

		case UKUIMENU_TREE_SORT_DISPLAY_NAME:
			retval = lookup_item_type_str("SORT_DISPLAY_NAME");
			break;

		default:
			g_assert_not_reached();
			break;
	}

	return (PyObject*) retval;
}

static PyObject* pyukuimenu_tree_set_sort_key(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	int sort_key;

	if (!PyArg_ParseTuple(args, "i:ukuimenu.Tree.set_sort_key", &sort_key))
	{
		return NULL;
	}

	tree = (PyUkuiMenuTree*) self;

	ukuimenu_tree_set_sort_key(tree->tree, sort_key);

	return Py_None;
}

static PyUkuiMenuTreeCallback* pyukuimenu_tree_callback_new(PyObject* tree, PyObject* callback, PyObject* user_data)
{
	PyUkuiMenuTreeCallback* retval;

	retval = g_new0(PyUkuiMenuTreeCallback, 1);

	Py_INCREF(tree);
	retval->tree = tree;

	Py_INCREF(callback);
	retval->callback = callback;

	Py_XINCREF(user_data);
	retval->user_data = user_data;

	return retval;
}

static void pyukuimenu_tree_callback_free(PyUkuiMenuTreeCallback* callback)
{
	Py_XDECREF(callback->user_data);
	callback->user_data = NULL;

	Py_DECREF(callback->callback);
	callback->callback = NULL;

	Py_DECREF(callback->tree);
	callback->tree = NULL;

	g_free(callback);
}

static void pyukuimenu_tree_handle_monitor_callback(UkuiMenuTree* tree, PyUkuiMenuTreeCallback* callback)
{
	PyObject* args;
	PyObject* ret;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	args = PyTuple_New(callback->user_data ? 2 : 1);

	Py_INCREF(callback->tree);
	PyTuple_SET_ITEM(args, 0, callback->tree);

	if (callback->user_data != NULL)
	{
		Py_INCREF(callback->user_data);
		PyTuple_SET_ITEM(args, 1, callback->user_data);
	}

	ret = PyObject_CallObject(callback->callback, args);

	Py_XDECREF(ret);
	Py_DECREF(args);

	PyGILState_Release(gstate);
}

static PyObject* pyukuimenu_tree_add_monitor(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	PyUkuiMenuTreeCallback* callback;
	PyObject* pycallback;
	PyObject* pyuser_data = NULL;

	if (!PyArg_ParseTuple(args, "O|O:ukuimenu.Tree.add_monitor", &pycallback, &pyuser_data))
	{
		return NULL;
	}

	if (!PyCallable_Check(pycallback))
	{
		PyErr_SetString(PyExc_TypeError, "callback must be callable");
		return NULL;
	}

	tree = (PyUkuiMenuTree*) self;

	callback = pyukuimenu_tree_callback_new(self, pycallback, pyuser_data);

	tree->callbacks = g_slist_append(tree->callbacks, callback);

	{
		UkuiMenuTreeDirectory* dir = ukuimenu_tree_get_root_directory(tree->tree);
		if (dir)
		{
			ukuimenu_tree_item_unref(dir);
		}
	}

	ukuimenu_tree_add_monitor(tree->tree, (UkuiMenuTreeChangedFunc) pyukuimenu_tree_handle_monitor_callback, callback);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyukuimenu_tree_remove_monitor(PyObject* self, PyObject* args)
{
	PyUkuiMenuTree* tree;
	PyObject* pycallback;
	PyObject* pyuser_data;
	GSList* tmp;

	if (!PyArg_ParseTuple(args, "O|O:ukuimenu.Tree.remove_monitor", &pycallback, &pyuser_data))
	{
		return NULL;
	}

	tree = (PyUkuiMenuTree*) self;

	tmp = tree->callbacks;

	while (tmp != NULL)
	{
		PyUkuiMenuTreeCallback* callback = tmp->data;
		GSList* next = tmp->next;

		if (callback->callback  == pycallback && callback->user_data == pyuser_data)
		{
			tree->callbacks = g_slist_delete_link(tree->callbacks, tmp);
			pyukuimenu_tree_callback_free(callback);
		}

		tmp = next;
	}

	Py_INCREF(Py_None);

	return Py_None;
}

static void pyukuimenu_tree_dealloc(PyUkuiMenuTree* self)
{
	g_slist_foreach(self->callbacks, (GFunc) pyukuimenu_tree_callback_free, NULL);
	g_slist_free(self->callbacks);
	self->callbacks = NULL;

	if (self->tree != NULL)
	{
		ukuimenu_tree_unref(self->tree);
	}

	self->tree = NULL;

	PyObject_DEL(self);
}

static PyObject* pyukuimenu_tree_getattro(PyUkuiMenuTree* self, PyObject* py_attr)
{
	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sss]", "root", "menu_file", "sort_key");
		}
		else if (!strcmp(attr, "root"))
		{
			return pyukuimenu_tree_get_root_directory((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "menu_file"))
		{
			return pyukuimenu_tree_get_menu_file((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "sort_key"))
		{
			return pyukuimenu_tree_get_sort_key((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static int pyukuimenu_tree_setattro(PyUkuiMenuTree* self, PyObject* py_attr, PyObject* py_value)
{
	PyUkuiMenuTree* tree;

	tree = (PyUkuiMenuTree*) self;

	if (PyBytes_Check(py_attr))
	{
		char* attr;

		attr = PyBytes_AS_STRING(py_attr);

		if (!strcmp(attr, "sort_key"))
		{
			if (PyLong_Check(py_value))
			{
				int sort_key;

				sort_key = PyLong_AsLong(py_value);

				if (sort_key < UKUIMENU_TREE_SORT_FIRST || sort_key > UKUIMENU_TREE_SORT_LAST)
				{
					return -1;
				}

				ukuimenu_tree_set_sort_key(tree->tree, sort_key);

				return 0;
			}
		}
	}

	return -1;
}

static struct PyMethodDef pyukuimenu_tree_methods[] = {
	{"get_menu_file", pyukuimenu_tree_get_menu_file, METH_VARARGS},
	{"get_root_directory", pyukuimenu_tree_get_root_directory, METH_VARARGS},
	{"get_directory_from_path", pyukuimenu_tree_get_directory_from_path, METH_VARARGS},
	{"get_sort_key", pyukuimenu_tree_get_sort_key, METH_VARARGS},
	{"set_sort_key", pyukuimenu_tree_set_sort_key, METH_VARARGS},
	{"add_monitor", pyukuimenu_tree_add_monitor, METH_VARARGS},
	{"remove_monitor", pyukuimenu_tree_remove_monitor, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyUkuiMenuTree_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"ukuimenu.Tree",                      /* tp_name */
	sizeof(PyUkuiMenuTree),               /* tp_basicsize */
	0,                                    /* tp_itemsize */
	(destructor) pyukuimenu_tree_dealloc, /* tp_dealloc */
	(printfunc) 0,                        /* tp_print */
	(getattrfunc) 0,                      /* tp_getattr */
	(setattrfunc) 0,                      /* tp_setattr */
	(PyAsyncMethods *) 0,                   /* tp_reserved */
	(reprfunc) 0,                         /* tp_repr */
	0,                                    /* tp_as_number */
	0,                                    /* tp_as_sequence */
	0,                                    /* tp_as_mapping */
	(hashfunc) 0,                         /* tp_hash */
	(ternaryfunc) 0,                      /* tp_call */
	(reprfunc) 0,                         /* tp_str */
	(getattrofunc) pyukuimenu_tree_getattro, /* tp_getattro */
	(setattrofunc) pyukuimenu_tree_setattro, /* tp_setattro */
	0,                                    /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                   /* tp_flags */
	NULL,                                 /* Documentation string */
	(traverseproc) 0,                     /* tp_traverse */
	(inquiry) 0,                          /* tp_clear */
	(richcmpfunc) 0,                      /* tp_richcompare */
	0,                                    /* tp_weaklistoffset */
	(getiterfunc) 0,                      /* tp_iter */
	(iternextfunc) 0,                     /* tp_iternext */
	pyukuimenu_tree_methods,              /* tp_methods */
	0,                                    /* tp_members */
	0,                                    /* tp_getset */
	(PyTypeObject*) 0,                    /* tp_base */
	(PyObject*) 0,                        /* tp_dict */
	0,                                    /* tp_descr_get */
	0,                                    /* tp_descr_set */
	0,                                    /* tp_dictoffset */
	(initproc) 0,                         /* tp_init */
	0,                                    /* tp_alloc */
	0,                                    /* tp_new */
};

static PyUkuiMenuTree* pyukuimenu_tree_wrap(UkuiMenuTree* tree)
{
	PyUkuiMenuTree* retval;

	if ((retval = ukuimenu_tree_get_user_data(tree)) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyUkuiMenuTree*) PyObject_NEW(PyUkuiMenuTree, &PyUkuiMenuTree_Type)))
	{
		return NULL;
	}

	retval->tree = ukuimenu_tree_ref(tree);
	retval->callbacks = NULL;

	ukuimenu_tree_set_user_data(tree, retval, NULL);

	return retval;
}

static PyObject* pyukuimenu_lookup_tree(PyObject* self, PyObject* args)
{
	char* menu_file;

	UkuiMenuTree* tree;
	PyUkuiMenuTree* retval;
	int flags;

	flags = UKUIMENU_TREE_FLAGS_NONE;

	if (!PyArg_ParseTuple(args, "s|i:ukuimenu.lookup_tree", &menu_file, &flags))
	{
		return NULL;
	}

	if (!(tree = ukuimenu_tree_lookup(menu_file, flags)))
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pyukuimenu_tree_wrap(tree);

	ukuimenu_tree_unref(tree);

	return (PyObject*) retval;
}

static struct PyMethodDef pyukuimenu_methods[] = {
	{"lookup_tree", pyukuimenu_lookup_tree, METH_VARARGS, "lookup tree"},
	{NULL, NULL, 0 }
};

static struct PyModuleDef ukuimenumodule = {
	PyModuleDef_HEAD_INIT,
	"ukuimenumodule",		
	NULL,					
	-1,					 
	pyukuimenu_methods
};

PyMODINIT_FUNC PyInit_ukuimenu(void)
{
	PyObject* mod;
	printf("init aaaaaaaaaaaaaaaaaaaaaaaaaa\n");

	mod = PyModule_Create(&ukuimenumodule);
	printf("init bbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");

	#define REGISTER_TYPE(t, n) G_STMT_START \
	{ \
		PyType_Ready(&t); \
		PyModule_AddObject(mod, n, (PyObject*)&t); \
	} G_STMT_END
	printf("init cccccccccccccccccccccccccccccc\n");

	REGISTER_TYPE(PyUkuiMenuTree_Type,     "Tree");
	REGISTER_TYPE(PyUkuiMenuTreeItem_Type, "Item");
	printf("init dddddddddddddddddddddddddddddddddd\n");

	#define REGISTER_ITEM_TYPE(t, n) G_STMT_START \
	{ \
		t.tp_base = &PyUkuiMenuTreeItem_Type; \
		PyType_Ready((PyTypeObject*)&t); \
		PyModule_AddObject(mod, n, (PyObject*)&t); \
	} G_STMT_END

	REGISTER_ITEM_TYPE(PyUkuiMenuTreeDirectory_Type, "Directory");
	REGISTER_ITEM_TYPE(PyUkuiMenuTreeEntry_Type,     "Entry");
	REGISTER_ITEM_TYPE(PyUkuiMenuTreeSeparator_Type, "Separator");
	REGISTER_ITEM_TYPE(PyUkuiMenuTreeHeader_Type,    "Header");
	REGISTER_ITEM_TYPE(PyUkuiMenuTreeAlias_Type,     "Alias");
	printf("init eeeeeeeeeeeeeeeeeeeeeeeeeee\n");

	PyModule_AddIntConstant(mod, "TYPE_INVALID",   UKUIMENU_TREE_ITEM_INVALID);
	PyModule_AddIntConstant(mod, "TYPE_DIRECTORY", UKUIMENU_TREE_ITEM_DIRECTORY);
	PyModule_AddIntConstant(mod, "TYPE_ENTRY",     UKUIMENU_TREE_ITEM_ENTRY);
	PyModule_AddIntConstant(mod, "TYPE_SEPARATOR", UKUIMENU_TREE_ITEM_SEPARATOR);
	PyModule_AddIntConstant(mod, "TYPE_HEADER",    UKUIMENU_TREE_ITEM_HEADER);
	PyModule_AddIntConstant(mod, "TYPE_ALIAS",     UKUIMENU_TREE_ITEM_ALIAS);

	PyModule_AddIntConstant(mod, "FLAGS_NONE",                UKUIMENU_TREE_FLAGS_NONE);
	PyModule_AddIntConstant(mod, "FLAGS_INCLUDE_EXCLUDED",    UKUIMENU_TREE_FLAGS_INCLUDE_EXCLUDED);
	PyModule_AddIntConstant(mod, "FLAGS_SHOW_EMPTY",          UKUIMENU_TREE_FLAGS_SHOW_EMPTY);
	PyModule_AddIntConstant(mod, "FLAGS_INCLUDE_NODISPLAY",   UKUIMENU_TREE_FLAGS_INCLUDE_NODISPLAY);
	PyModule_AddIntConstant(mod, "FLAGS_SHOW_ALL_SEPARATORS", UKUIMENU_TREE_FLAGS_SHOW_ALL_SEPARATORS);

	PyModule_AddIntConstant(mod, "SORT_NAME",         UKUIMENU_TREE_SORT_NAME);
	PyModule_AddIntConstant(mod, "SORT_DISPLAY_NAME", UKUIMENU_TREE_SORT_DISPLAY_NAME);

	printf("init fffffffffffffffffffffffffffffffff\n");
	return mod;
}

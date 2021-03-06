/*
 * Copyright (C) 2004 Red Hat, Inc.
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
 * Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include "ukuimenu-tree.h"

#include <string.h>

/* This is only a test program, so we don't need translations. Still keep the
 * infrastructure in place in case we suddenly decide we want to localize this
 * program. Don't forget to reenable the call to bindtextdomain() if going back
 * to real localization. */
#define _(x) x
#define N_(x) x

static char* menu_file = NULL;
static gboolean monitor = FALSE;
static gboolean include_excluded = FALSE;
static gboolean include_nodisplay = FALSE;

static GOptionEntry options[] = {
	{"file",              'f', 0, G_OPTION_ARG_STRING, &menu_file,         N_("Menu file"),                      N_("MENU_FILE")},
	{"monitor",           'm', 0, G_OPTION_ARG_NONE,   &monitor,           N_("Monitor for menu changes"),       NULL},
	{"include-excluded",  'i', 0, G_OPTION_ARG_NONE,   &include_excluded,  N_("Include <Exclude>d entries"),     NULL},
	{"include-nodisplay", 'n', 0, G_OPTION_ARG_NONE,   &include_nodisplay, N_("Include NoDisplay=true entries"), NULL},
	{NULL}
};

static void append_directory_path(UkuiMenuTreeDirectory* directory, GString* path)
{
	UkuiMenuTreeDirectory* parent = ukuimenu_tree_item_get_parent(UKUIMENU_TREE_ITEM(directory));

	if (!parent)
	{
		g_string_append_c(path, '/');
		return;
	}

	append_directory_path(parent, path);

	g_string_append(path, ukuimenu_tree_directory_get_name(directory));
	g_string_append_c(path, '/');

	ukuimenu_tree_item_unref(parent);
}

static char* make_path(UkuiMenuTreeDirectory* directory)
{
	g_return_val_if_fail(directory != NULL, NULL);

	GString* path = g_string_new(NULL);

	append_directory_path(directory, path);

	return g_string_free(path, FALSE);
}

static void print_entry(UkuiMenuTreeEntry* entry, const char* path)
{
	char* utf8_path = g_filename_to_utf8(ukuimenu_tree_entry_get_desktop_file_path(entry), -1, NULL, NULL, NULL);
	char* utf8_file_id = g_filename_to_utf8(ukuimenu_tree_entry_get_desktop_file_id(entry), -1, NULL, NULL, NULL);

	g_print("%s    %s    %s%s%s\n",
		path,
		utf8_file_id ? utf8_file_id : _("Invalid desktop file ID"),
		utf8_path ? utf8_path : _("[Invalid Filename]"),
		ukuimenu_tree_entry_get_is_excluded(entry) ? _(" <excluded>") : "",
		ukuimenu_tree_entry_get_is_nodisplay(entry) ? _(" <nodisplay>") : "");

	g_free(utf8_file_id);
	g_free(utf8_path);
}

static void print_directory(UkuiMenuTreeDirectory* directory)
{
	const char* path;
	char* freeme = make_path(directory);

	if (!strcmp(freeme, "/"))
	{
		path = freeme;
	}
	else
	{
		path = freeme + 1;
	}

	GSList* items = ukuimenu_tree_directory_get_contents(directory);
	GSList* tmp = items;

	while (tmp != NULL)
	{
		UkuiMenuTreeItem* item = tmp->data;

		switch (ukuimenu_tree_item_get_type(item))
		{
			case UKUIMENU_TREE_ITEM_ENTRY:
				print_entry(UKUIMENU_TREE_ENTRY(item), path);
				break;

			case UKUIMENU_TREE_ITEM_DIRECTORY:
				print_directory(UKUIMENU_TREE_DIRECTORY(item));
				break;

			case UKUIMENU_TREE_ITEM_HEADER:
			case UKUIMENU_TREE_ITEM_SEPARATOR:
				break;

			case UKUIMENU_TREE_ITEM_ALIAS:
				{
					UkuiMenuTreeItem* aliased_item = ukuimenu_tree_alias_get_item(UKUIMENU_TREE_ALIAS(item));

					if (ukuimenu_tree_item_get_type(aliased_item) == UKUIMENU_TREE_ITEM_ENTRY)
					{
						print_entry(UKUIMENU_TREE_ENTRY(aliased_item), path);
					}
				}
				break;

			default:
				g_assert_not_reached();
				break;
		}

		ukuimenu_tree_item_unref(tmp->data);

		tmp = tmp->next;
	}

	g_slist_free(items);

	g_free(freeme);
}

static void handle_tree_changed(UkuiMenuTree* tree)
{
	g_print(_("\n\n\n==== Menu changed, reloading ====\n\n\n"));

	UkuiMenuTreeDirectory* root = ukuimenu_tree_get_root_directory(tree);

	if (root == NULL)
	{
		g_warning(_("Menu tree is empty"));
		return;
	}

	print_directory(root);
	ukuimenu_tree_item_unref(root);
}

int main(int argc, char** argv)
{
	#if 0
	  /* See comment when defining _() at the top of this file. */
	  bindtextdomain(GETTEXT_PACKAGE, UKUILOCALEDIR);
	  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	  textdomain(GETTEXT_PACKAGE);
	#endif

	GOptionContext* options_context = g_option_context_new(_("- test UKUI's implementation of the Desktop Menu Specification"));
	g_option_context_add_main_entries(options_context, options, GETTEXT_PACKAGE);
	g_option_context_parse(options_context, &argc, &argv, NULL);
	g_option_context_free(options_context);

	UkuiMenuTreeFlags flags = UKUIMENU_TREE_FLAGS_NONE;

	if (include_excluded)
	{
		flags |= UKUIMENU_TREE_FLAGS_INCLUDE_EXCLUDED;
	}

	if (include_nodisplay)
	{
		flags |= UKUIMENU_TREE_FLAGS_INCLUDE_NODISPLAY;
	}

	// Usamos applications.menu is existe. Para compatibilidad con GNOME
	UkuiMenuTree* tree = ukuimenu_tree_lookup(menu_file ? menu_file : "ukui-applications.menu", flags);

	g_assert(tree != NULL);

	UkuiMenuTreeDirectory* root = ukuimenu_tree_get_root_directory(tree);

	if (root != NULL)
	{
		print_directory(root);
		ukuimenu_tree_item_unref(root);
	}
	else
	{
		g_warning(_("Menu tree is empty"));
	}

	if (monitor)
	{
		ukuimenu_tree_add_monitor(tree, (UkuiMenuTreeChangedFunc) handle_tree_changed, NULL);

		GMainLoop* main_loop = g_main_loop_new(NULL, FALSE);
		g_main_loop_run(main_loop);
		g_main_loop_unref(main_loop);

		ukuimenu_tree_remove_monitor(tree, (UkuiMenuTreeChangedFunc) handle_tree_changed, NULL);
	}

	ukuimenu_tree_unref(tree);

	return 0;
}

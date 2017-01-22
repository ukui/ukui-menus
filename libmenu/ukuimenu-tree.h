/*
 * Copyright (C) 2004 Red Hat, Inc.
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

#ifndef __UKUIMENU_TREE_H__
#define __UKUIMENU_TREE_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UkuiMenuTree          UkuiMenuTree;
typedef struct UkuiMenuTreeItem      UkuiMenuTreeItem;
typedef struct UkuiMenuTreeDirectory UkuiMenuTreeDirectory;
typedef struct UkuiMenuTreeEntry     UkuiMenuTreeEntry;
typedef struct UkuiMenuTreeSeparator UkuiMenuTreeSeparator;
typedef struct UkuiMenuTreeHeader    UkuiMenuTreeHeader;
typedef struct UkuiMenuTreeAlias     UkuiMenuTreeAlias;

typedef void (*UkuiMenuTreeChangedFunc) (UkuiMenuTree* tree, gpointer user_data);

typedef enum {
	UKUIMENU_TREE_ITEM_INVALID = 0,
	UKUIMENU_TREE_ITEM_DIRECTORY,
	UKUIMENU_TREE_ITEM_ENTRY,
	UKUIMENU_TREE_ITEM_SEPARATOR,
	UKUIMENU_TREE_ITEM_HEADER,
	UKUIMENU_TREE_ITEM_ALIAS
} UkuiMenuTreeItemType;

#define UKUIMENU_TREE_ITEM(i)      ((UkuiMenuTreeItem*)(i))
#define UKUIMENU_TREE_DIRECTORY(i) ((UkuiMenuTreeDirectory*)(i))
#define UKUIMENU_TREE_ENTRY(i)     ((UkuiMenuTreeEntry*)(i))
#define UKUIMENU_TREE_SEPARATOR(i) ((UkuiMenuTreeSeparator*)(i))
#define UKUIMENU_TREE_HEADER(i)    ((UkuiMenuTreeHeader*)(i))
#define UKUIMENU_TREE_ALIAS(i)     ((UkuiMenuTreeAlias*)(i))

typedef enum {
	UKUIMENU_TREE_FLAGS_NONE                = 0,
	UKUIMENU_TREE_FLAGS_INCLUDE_EXCLUDED    = 1 << 0,
	UKUIMENU_TREE_FLAGS_SHOW_EMPTY          = 1 << 1,
	UKUIMENU_TREE_FLAGS_INCLUDE_NODISPLAY   = 1 << 2,
	UKUIMENU_TREE_FLAGS_SHOW_ALL_SEPARATORS = 1 << 3,
	UKUIMENU_TREE_FLAGS_MASK                = 0x0f
} UkuiMenuTreeFlags;

typedef enum {
	#define UKUIMENU_TREE_SORT_FIRST UKUIMENU_TREE_SORT_NAME
	UKUIMENU_TREE_SORT_NAME = 0,
	UKUIMENU_TREE_SORT_DISPLAY_NAME
	#define UKUIMENU_TREE_SORT_LAST UKUIMENU_TREE_SORT_DISPLAY_NAME
} UkuiMenuTreeSortKey;

UkuiMenuTree* ukuimenu_tree_lookup(const char* menu_file, UkuiMenuTreeFlags flags);

UkuiMenuTree* ukuimenu_tree_ref(UkuiMenuTree* tree);
void ukuimenu_tree_unref(UkuiMenuTree* tree);

void ukuimenu_tree_set_user_data(UkuiMenuTree* tree, gpointer user_data, GDestroyNotify dnotify);
gpointer ukuimenu_tree_get_user_data(UkuiMenuTree* tree);

const char* ukuimenu_tree_get_menu_file(UkuiMenuTree* tree);
UkuiMenuTreeDirectory* ukuimenu_tree_get_root_directory(UkuiMenuTree* tree);
UkuiMenuTreeDirectory* ukuimenu_tree_get_directory_from_path(UkuiMenuTree* tree, const char* path);

UkuiMenuTreeSortKey ukuimenu_tree_get_sort_key(UkuiMenuTree* tree);
void ukuimenu_tree_set_sort_key(UkuiMenuTree* tree, UkuiMenuTreeSortKey sort_key);



gpointer ukuimenu_tree_item_ref(gpointer item);
void ukuimenu_tree_item_unref(gpointer item);

void ukuimenu_tree_item_set_user_data(UkuiMenuTreeItem* item, gpointer user_data, GDestroyNotify dnotify);
gpointer ukuimenu_tree_item_get_user_data(UkuiMenuTreeItem* item);

UkuiMenuTreeItemType ukuimenu_tree_item_get_type(UkuiMenuTreeItem* item);
UkuiMenuTreeDirectory* ukuimenu_tree_item_get_parent(UkuiMenuTreeItem* item);


GSList* ukuimenu_tree_directory_get_contents(UkuiMenuTreeDirectory* directory);
const char* ukuimenu_tree_directory_get_name(UkuiMenuTreeDirectory* directory);
const char* ukuimenu_tree_directory_get_comment(UkuiMenuTreeDirectory* directory);
const char* ukuimenu_tree_directory_get_icon(UkuiMenuTreeDirectory* directory);
const char* ukuimenu_tree_directory_get_desktop_file_path(UkuiMenuTreeDirectory* directory);
const char* ukuimenu_tree_directory_get_menu_id(UkuiMenuTreeDirectory* directory);
UkuiMenuTree* ukuimenu_tree_directory_get_tree(UkuiMenuTreeDirectory* directory);

gboolean ukuimenu_tree_directory_get_is_nodisplay(UkuiMenuTreeDirectory* directory);

char* ukuimenu_tree_directory_make_path(UkuiMenuTreeDirectory* directory, UkuiMenuTreeEntry* entry);


const char* ukuimenu_tree_entry_get_name(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_generic_name(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_display_name(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_comment(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_icon(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_exec(UkuiMenuTreeEntry* entry);
gboolean ukuimenu_tree_entry_get_launch_in_terminal(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_desktop_file_path(UkuiMenuTreeEntry* entry);
const char* ukuimenu_tree_entry_get_desktop_file_id(UkuiMenuTreeEntry* entry);
gboolean ukuimenu_tree_entry_get_is_excluded(UkuiMenuTreeEntry* entry);
gboolean ukuimenu_tree_entry_get_is_nodisplay(UkuiMenuTreeEntry* entry);

UkuiMenuTreeDirectory* ukuimenu_tree_header_get_directory(UkuiMenuTreeHeader* header);

UkuiMenuTreeDirectory* ukuimenu_tree_alias_get_directory(UkuiMenuTreeAlias* alias);
UkuiMenuTreeItem* ukuimenu_tree_alias_get_item(UkuiMenuTreeAlias* alias);

void ukuimenu_tree_add_monitor(UkuiMenuTree* tree, UkuiMenuTreeChangedFunc callback, gpointer user_data);
void ukuimenu_tree_remove_monitor(UkuiMenuTree* tree, UkuiMenuTreeChangedFunc callback, gpointer user_data);

#ifdef __cplusplus
}
#endif

#endif /* __UKUIMENU_TREE_H__ */

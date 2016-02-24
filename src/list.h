/*
   This file is part of ctoolbox library collection. 
   Copyright (C) 2016 Anssi Kulju 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIST_H
#define LIST_H

typedef struct ListNode_s ListNode_t;
struct ListNode_s
{
	void *data;

	ListNode_t *next;
};

typedef struct List_s List_t;
struct List_s
{
	ListNode_t *hd;
	int (*ls_cmp)(void*, void*);
};

List_t *ls_new();
void ls_add(List_t *l, void *data);
void ls_rm(List_t *l, void *data);
void *ls_search(List_t *l, void *data);
void ls_node_free(ListNode_t *ln);
void ls_free(List_t* l);
void ls_traverse(List_t *l, void (*ls_tr_cb)(void*));
int  ls_count(List_t* l);

#endif


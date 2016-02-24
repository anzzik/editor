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
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

List_t *ls_new(void *cmp_fn)
{
	List_t *l;

	l = malloc(sizeof(List_t));
	l->ls_cmp = cmp_fn;
	l->hd = malloc(sizeof(ListNode_t));
	l->hd->next = 0;

	return l;
}

void ls_add(List_t *l, void *data)
{
	ListNode_t *ln;

	ln = malloc(sizeof(ListNode_t));
	ln->data = data;
	ln->next = 0;

	ln->next = l->hd->next;
	l->hd->next = ln;
}

void ls_rm(List_t *l, void *data)
{
	ListNode_t **pp;
	ListNode_t  *tmp;
	int	     r;

	pp = &l->hd->next;

	while (*pp)
	{
		r = l->ls_cmp( (*pp)->data, data );
		if (!r)
		{
			tmp = (*pp)->next;
			ls_node_free(*pp);
			*pp = tmp;
			
			break;
		}

		pp = &(*pp)->next;
	}
}

void *ls_search(List_t *l, void *data)
{
	ListNode_t *ln;
	int	    r;

	ln = l->hd->next;

	while (ln)
	{
		r = l->ls_cmp(ln->data, data);
		if (!r)
			break;

		ln = ln->next;
	}

	if (!ln)
		return 0;

	return ln->data;
}

void ls_node_free(ListNode_t *ln)
{
	free(ln);
}

void ls_free(List_t* l)
{
	ListNode_t *ln, *tmp;

	ln = l->hd->next;
	while (ln)
	{
		tmp = ln->next;
		ls_node_free(ln);
		ln = tmp;
	}

	free(l->hd);
	free(l);
}

int ls_count(List_t* l)
{
	ListNode_t *ln;
	int	    c;

	c = 0;

	ln = l->hd->next;
	while (ln)
	{
		c++;
		ln = ln->next;
	}

	return c;
}

void ls_traverse(List_t *l, void (*ls_tr_cb)(void*))
{
	ListNode_t *ln;

	ln = l->hd->next;
	while (ln)
	{
		ls_tr_cb(ln->data);
		ln = ln->next;
	}
}


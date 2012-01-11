/*
 * lookup_tree.c -- the tree of host lookup engine
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/* 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at you option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111,
 * USA.
 */


/* lookup_tree.c
 * this file contains the functions called by lookup.c
 * all name/ip couples are cached in a balanced tree (in a similar way
 * to the avl tree)
 */

#include "tcpick.h"
#include "extern.h"

#define PARENT_POSITION_IS_LEFT(p) (p->parent->right == p)

struct _l_node *  _l_root; /* the first node */

/* prototypes */

struct _l_node *
_l_alloc(struct in_addr, char *);

char *
_l_get(struct in_addr);

int
_l_insert(struct _l_node *);

int
_l_walkup(struct _l_node *);

int
_l_checkweight(struct _l_node *);

int
_l_balance_right(struct _l_node *);

int
_l_balance_left(struct _l_node *);

int
_l_adjust_h(struct _l_node *);

struct _l_node *
_l_alloc(struct in_addr ip, char * name)
{
	register struct _l_node * new;

	new = (struct _l_node *) S_malloc(sizeof(struct _l_node));
	memset(new, 0, sizeof(struct _l_node));
	
	new->name = (char *)strdup(name); 
	/* FIXME: not sure strdup is a good thing*/
	memcpy(&(new->ip), &ip, sizeof(struct in_addr));
	
	return new;
}

char *
_l_get(struct in_addr ia)
{
	register struct _l_node * p;

	p = _l_root;

	while(p) {
		if ( p->ip.s_addr == ia.s_addr )
			return p->name; /* found */
		p = ( ia.s_addr > p->ip.s_addr ) 
			? p->right : p->left;
	}

	return NULL; /* not found */
}

int
_l_insert(struct _l_node * new)
{
	/* FIXME: could be better */

	register struct _l_node ** p;
	register struct _l_node * pre = NULL;
	
	p = & _l_root;

	while(*p) {
		pre = *p;
		p = ( new->ip.s_addr > (*p)->ip.s_addr ) ?
			&((*p)->right) : &((*p)->left);
	}
	
	*p = new;
	new->parent = pre;

	_l_walkup(new);

	return 1;
}

int
_l_walkup(struct _l_node * node)
/* FIXME: could be improved maybe */
{
	register struct _l_node * par;

	par = node->parent;

	while( par != NULL ) {

		if( par->right == node )
			par->right_h = MAX(par->right->right_h,
					   par->right->left_h) + 1;
		else
			par->left_h = MAX(par->left->right_h,
					   par->left->left_h) + 1;

		if( _l_checkweight(par) != 0 ) 
			/* something was done */
			return 1;

		node = par;
		par = par->parent;
	}
	return 0;
}

int
_l_checkweight(struct _l_node * node)
{
	if( (node->right_h - node->left_h) > 1) {
		/* need balance because of right too weight */
		_l_balance_right(node->right);
		return 1;
	}

	if( (node->left_h - node->right_h) > 1) {
		/* need balance because of left too weight */
		_l_balance_left(node->left);
		return 2;
	}
	
	return 0;
}

int
_l_balance_right(struct _l_node * D)
{
/* before:

        B
       / \
      A   D
         / \
        C   E

   after:
   
       D
      / \
     B   E
    / \
   A   C

*/

	register struct _l_node * 
		B = D->parent;

	/* 1. step: put up the node */
	if( B->parent != NULL ) {
		
		if( PARENT_POSITION_IS_LEFT(B) )
			B->parent->right = D;
		else
			B->parent->left  = D;

		D->parent = B->parent;

	} else { /* this is the root */
		_l_root = D;
		_l_root->parent = NULL;
	}

	/* 2. step: the left side C of the node D becames the
	 * right of the node B */

	B->right = D->left;

	if( B->right )
		B->right->parent = B;
	
	/* 3. step: left side of D is B */
	D->left = B;
	B->parent = D;

	/* 4. step: adjust height values */
	_l_adjust_h(B);
	_l_adjust_h(D);

	return 1;
}

int
_l_balance_left(struct _l_node * D)
{
/* before:

        B
       / \
      D   A
     / \
    E   C

   after:
   
       D
      / \
     E   B
        / \
       C   A

*/

	register struct _l_node * 
		B = D->parent;
	
	/* 1. step: put up the node */
	if( B->parent != NULL ) {

		if( PARENT_POSITION_IS_LEFT(B) )
			B->parent->right = D;
		else
			B->parent->left  = D;
	}

	D->parent = B->parent;
	
	/* 2. step: the right side C of the node D becames the
	 * left of the node B */
	B->left = D->right;

	if( B->left )
		B->left->parent = B;
	
	/* 3. step: right side of D is B */
	D->right = B;
	B->parent = D;

	/* 4. step: adjust height values */
	_l_adjust_h(B);
	_l_adjust_h(D);

	return 1;
}

int
_l_adjust_h(struct _l_node * node)
{

	node->right_h = ( node->right != NULL )
		? MAX(node->right->left_h, 
		      node->right->right_h) + 1
		: 0;

	node->left_h = ( node->left != NULL )
		? MAX(node->left->left_h,
		      node->left->right_h) + 1
		: 0;

	return 1;
}

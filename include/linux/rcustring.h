/*
 * RCU-friendly strings
 *
 * Copyright (C) 2012 Red Hat.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 */

#ifndef _LINUX_RCUSTRING_H
#define _LINUX_RCUSTRING_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/printk.h>
#include <linux/rcupdate.h>
#include <linux/slab.h>

struct rcu_string {
	struct rcu_head rcu;
	char str[0];
};

/**
 * rcu_string_strdup() - create an RCU string from a string
 * @src: The string to copy
 * @flags: Flags for kmalloc
 */
static inline struct rcu_string *rcu_string_strdup(const char *src, gfp_t flags)
{
	struct rcu_string *ret;
	size_t len = strlen(src) + 1;

	ret = kmalloc(sizeof(*ret) + (len * sizeof(char)), flags);
	if (ret)
		memcpy(ret->str, src, len);
	return ret;
}

/**
 * rcu_string_free() - free an RCU string
 * @str: The string
 */
static inline void rcu_string_free(struct rcu_string *str)
{
	if (str)
		kfree_rcu(str, rcu);
}

/**
 * rcu_string_dereference() - dereference an RCU string
 * @str: The string
 *
 * Like rcu_dereference, this must be done in an RCU critical section.
 */
static inline char *rcu_string_dereference(struct rcu_string __rcu *rcu_str)
{
	return rcu_dereference(rcu_str)->str;
}

/**
 * printk_in_rcu() - printk in an RCU read-side critical section
 * @fmt: Format string
 * @...: Values
 */
#define printk_in_rcu(fmt, ...)			\
	do {					\
		rcu_read_lock();		\
		printk(fmt, ##__VA_ARGS__);	\
		rcu_read_unlock();		\
	} while (0)

/**
 * printk_ratelimited_in_rcu() - printk_ratelimited in an RCU read-side critical
 * section
 * @fmt: Format string
 * @...: Values
 */
#define printk_ratelimited_in_rcu(fmt, ...)		\
	do {						\
		rcu_read_lock();			\
		printk_ratelimited(fmt, ##__VA_ARGS__);	\
		rcu_read_unlock();			\
	} while (0)

#endif

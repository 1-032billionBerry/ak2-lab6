// SPDX-License-Identifier: Dual BSD/GPL
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/bug.h>

MODULE_AUTHOR("Kyrykovych Vitalii <kyrykovych.vitalii@lll.kpi.ua>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint repeat_count = 1;
module_param(repeat_count, uint, 0444);
MODULE_PARM_DESC(repeat_count, "The number of times to print 'Hello, world!'");

struct hello_item {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

void print_hello(int iteration)
{
	struct hello_item *item;

	if (iteration == 8)
		item = NULL;
	else
		item = kmalloc(sizeof(*item), GFP_KERNEL);

	BUG_ON(!item);

	item->time = ktime_get();
	pr_info("Hello, world!\n");
	list_add(&item->list, &hello_list);
}

static int __init hello_init(void)
{
	int i;

	if (repeat_count == 0 || (repeat_count >= 5 && repeat_count <= 10))
		pr_warn("Warning: repeat_count is %u\n", repeat_count);

	BUG_ON(repeat_count > 10);

	for (i = 0; i < repeat_count; i++)
		print_hello(i);

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_item *item, *tmp;

	list_for_each_entry_safe(item, tmp, &hello_list, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(item->time));
		list_del(&item->list);
		kfree(item);
	}
	pr_info("Goodbye, World\n");
}

module_init(hello_init);
module_exit(hello_exit);

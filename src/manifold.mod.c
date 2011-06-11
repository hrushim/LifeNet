#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf13803af, "module_layout" },
	{ 0xb9a12636, "per_cpu__current_task" },
	{ 0x9bb69589, "kmalloc_caches" },
	{ 0x6980fe91, "param_get_int" },
	{ 0xd0d8621b, "strlen" },
	{ 0x75b279cd, "dev_get_by_name" },
	{ 0xead8f25, "skb_copy" },
	{ 0x105e2727, "__tracepoint_kmalloc" },
	{ 0x484ec6d3, "remove_proc_entry" },
	{ 0x6a9f26c9, "init_timer_key" },
	{ 0xff964b25, "param_set_int" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0x8f947111, "netif_rx" },
	{ 0x41344088, "param_get_charp" },
	{ 0xe83fea1, "del_timer_sync" },
	{ 0xb86e4ab9, "random32" },
	{ 0xb72397d5, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0xb4390f9a, "mcount" },
	{ 0x6c2e3320, "strncmp" },
	{ 0x3c0fa393, "skb_push" },
	{ 0x44aa17a6, "dev_remove_pack" },
	{ 0x45450063, "mod_timer" },
	{ 0x46085e4f, "add_timer" },
	{ 0x13ea0269, "skb_pull" },
	{ 0x79213868, "init_net" },
	{ 0x4903a933, "skb_copy_expand" },
	{ 0x2e2400f, "kmem_cache_alloc" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x327e513b, "kfree_skb" },
	{ 0xe01294d, "eth_type_trans" },
	{ 0x3bae4e21, "create_proc_entry" },
	{ 0x6ad065f4, "param_set_charp" },
	{ 0x1d2e87c6, "do_gettimeofday" },
	{ 0x37a0cba, "kfree" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xc4cdad2e, "dev_add_pack" },
	{ 0xa227cdf7, "dev_queue_xmit" },
	{ 0xd6c963c, "copy_from_user" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "110EA8CD2EE61BC238AF754");

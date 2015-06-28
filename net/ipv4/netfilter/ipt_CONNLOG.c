/* Optionally log start and end of connections.
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_protocol.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>

/* Use lock to serialize, so printks don't overlap */
static spinlock_t log_lock = SPIN_LOCK_UNLOCKED;

static unsigned int
target(struct sk_buff **pskb,
       unsigned int hooknum,
       const struct net_device *in,
       const struct net_device *out,
       const void *targinfo,
       void *userinfo)
{
	enum ip_conntrack_info ctinfo;
	struct ip_conntrack *ct = ip_conntrack_get((*pskb), &ctinfo);

	if (ct)
		set_bit(IPS_LOG_BIT, &ct->status);

	return IPT_CONTINUE;
}

static int
checkentry(const char *tablename,
	   const struct ipt_entry *e,
           void *targinfo,
           unsigned int targinfosize,
           unsigned int hook_mask)
{
	if (targinfosize != IPT_ALIGN(0))
		return 0;

	return 1;
}

static struct ipt_target ipt_connlog_reg = {
	.name = "CONNLOG",
	.target = target,
	.checkentry = checkentry,
	.me = THIS_MODULE,
};

static void dump_tuple(char *buffer, const struct ip_conntrack_tuple *tuple,
		       struct ip_conntrack_protocol *proto)
{
	printk("src=%u.%u.%u.%u dst=%u.%u.%u.%u ",
		NIPQUAD(tuple->src.ip), NIPQUAD(tuple->dst.ip));

	if (proto->print_tuple(buffer, tuple))
		printk("%s", buffer);
}

#ifdef CONFIG_IP_NF_CT_ACCT
static void dump_counters(struct ip_conntrack_counter *counter)
{
	printk("packets=%llu bytes=%llu ", counter->packets, counter->bytes);
}
#else
static inline void dump_counters(struct ip_conntrack_counter *counter) {}
#endif

static void dump_conntrack(struct ip_conntrack *ct)
{
	struct ip_conntrack_protocol *proto
		= __ip_ct_find_proto(ct->tuplehash[IP_CT_DIR_ORIGINAL]
			       .tuple.dst.protonum);
	char buffer[256];

	printk("id=%p ", ct);
	printk("proto=%s ", proto->name);

	if (proto->print_conntrack(buffer, ct))
		printk("%s", buffer);

	dump_tuple(buffer, &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple, proto);
	dump_counters(&ct->counters[IP_CT_DIR_ORIGINAL]);

	dump_tuple(buffer, &ct->tuplehash[IP_CT_DIR_REPLY].tuple, proto);
	dump_counters(&ct->counters[IP_CT_DIR_REPLY]);

#if defined(CONFIG_IP_NF_CONNTRACK_MARK)
	printk("mark=%ld ", ct->mark);
#endif
}

static int conntrack_event(struct notifier_block *this,
			   unsigned long event,
			   void *ptr)
{
	struct ip_conntrack *ct = ptr;
	const char *log_prefix = NULL;

	if (event & (IPCT_NEW|IPCT_RELATED))
		log_prefix = "create";
	else if (event & (IPCT_DESTROY))
		log_prefix = "destroy";

	if (log_prefix && test_bit(IPS_LOG_BIT, &ct->status)) {
		spin_lock_bh(&log_lock);
		printk(KERN_INFO "conntrack %s: ", log_prefix);
		dump_conntrack(ct);
		printk("\n");
		spin_unlock_bh(&log_lock);
	}

	return NOTIFY_DONE;
}

static struct notifier_block conntrack_notifier = {
	.notifier_call = conntrack_event,
};

static int __init init(void)
{
	int ret;

	ret = ipt_register_target(&ipt_connlog_reg);
	if (ret != 0)
		return ret;

	ret = ip_conntrack_register_notifier(&conntrack_notifier);
	if (ret != 0)
		goto unregister_connlog;

	return ret;

unregister_connlog:
	ipt_unregister_target(&ipt_connlog_reg);

	return ret;
}

static void __exit fini(void)
{
	ipt_unregister_target(&ipt_connlog_reg);
	ip_conntrack_unregister_notifier(&conntrack_notifier);
}

module_init(init);
module_exit(fini);

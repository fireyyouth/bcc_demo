#define KBUILD_MODNAME "udp_counter"

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

BPF_HASH(map);

int udp_counter(struct xdp_md *ctx)
{
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    const struct ethhdr *eth = data;

    if ((void *)eth + sizeof(*eth) > data_end) {
        return XDP_PASS;
    }

    const struct iphdr *ip = data + sizeof(*eth);
    if ((void *)ip + sizeof(*ip) > data_end) {
        return XDP_PASS;
    }

    if (ip->protocol != IPPROTO_UDP) {
        return XDP_PASS;
    }

    const struct udphdr *udp = (void *)ip + sizeof(*ip);
    if ((void *)udp + sizeof(*udp) > data_end) {
        return XDP_PASS;
    }

    u32 addr = ntohl(ip->daddr);
    u16 port = ntohs(udp->dest);
    u64 key = ((u64)addr << 32) | port;
    u64 *exist = map.lookup(&key);
    bpf_trace_printk("(%x, %u) => %p\n", addr, port, exist);

    return XDP_PASS;
}

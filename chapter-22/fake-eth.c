#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/of.h>                   /* For DT*/
#include <linux/platform_device.h>      /* For platform devices */

struct eth_struct {
    int bar;
    int foo;
    struct net_device *dummy_ndev;
};


static int fake_eth_open(struct net_device *dev) {
    printk("fake_eth_open called\n");
    /* We are now ready to accept transmit requests from
	 * the queueing layer of the networking.
	 */
	netif_start_queue(dev);
    return 0;
}

static int fake_eth_release(struct net_device *dev) {
    pr_info("fake_eth_release called\n");
    netif_stop_queue(dev);
    return 0;
}

static int fake_eth_xmit(struct sk_buff *skb, struct net_device *ndev) {    
    pr_info("dummy xmit called...\n");
    ndev->stats.tx_bytes += skb->len;
    ndev->stats.tx_packets++;

    skb_tx_timestamp(skb);
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}

static int fake_eth_init(struct net_device *dev)
{
    pr_info("fake eth device initialized\n");
    return 0;
};


static const struct net_device_ops my_netdev_ops = {
     .ndo_init = fake_eth_init,
     .ndo_open = fake_eth_open,
     .ndo_stop = fake_eth_release,
     .ndo_start_xmit = fake_eth_xmit,
     .ndo_validate_addr	= eth_validate_addr,
     .ndo_validate_addr	= eth_validate_addr,
};


static const struct of_device_id fake_eth_dt_ids[] = {
    { .compatible = "packt,fake-eth", },
    { /* sentinel */ }
};


static int fake_eth_probe(struct platform_device *pdev)
{
    int ret;
    struct eth_struct *priv;
    struct net_device *dummy_ndev;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    dummy_ndev = alloc_etherdev(sizeof(struct eth_struct));
    dummy_ndev->if_port = IF_PORT_10BASET;
    dummy_ndev->netdev_ops = &my_netdev_ops;
    
    /* If needed, dev->ethtool_ops = &fake_ethtool_ops; */

    ret = register_netdev(dummy_ndev);
    if(ret) {
        pr_info("dummy net dev: Error %d initalizing card ...", ret);
        return ret;
    }

    priv->dummy_ndev = dummy_ndev;
    platform_set_drvdata(pdev, priv);
    return 0;
}


static int fake_eth_remove(struct platform_device *pdev)
{
    struct eth_struct *priv;
	priv = platform_get_drvdata(pdev);
	pr_info("Cleaning Up the Module\n");
    unregister_netdev(priv->dummy_ndev);
    free_netdev(priv->dummy_ndev);

	return 0;
}

static struct platform_driver mypdrv = {
    .probe      = fake_eth_probe,
    .remove     = fake_eth_remove,
    .driver     = {
        .name     = "fake-eth",
        .of_match_table = of_match_ptr(fake_eth_dt_ids),  
        .owner    = THIS_MODULE,
    },
};
module_platform_driver(mypdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Madieu <john.madieu@gmail.com>");
MODULE_DESCRIPTION("Fake ethernet driver");









/*

Dmesg

[146626.222072] fake-eth added
[146689.422000] fake-fake removed
[146698.060074] fake eth device initialized
[146698.060285] fake-eth added
[146698.087297] IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
jma@jma:~/work/tutos/sources/chapter-22$ 



    Before:

enp3s0f1  Link encap:Ethernet  HWaddr 34:97:f6:ba:5e:d7  
          UP BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:49727 errors:0 dropped:0 overruns:0 frame:0
          TX packets:49727 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1 
          RX bytes:8431650 (8.4 MB)  TX bytes:8431650 (8.4 MB)

*/

/*
    After:

enp3s0f1  Link encap:Ethernet  HWaddr 34:97:f6:ba:5e:d7  
          UP BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

eth0      Link encap:Ethernet  HWaddr 00:00:00:00:00:00  
          BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:49727 errors:0 dropped:0 overruns:0 frame:0
          TX packets:49727 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1 
          RX bytes:8431650 (8.4 MB)  TX bytes:8431650 (8.4 MB)


After sudo ifconfig eth0 192.168.1.45

eth0      Link encap:Ethernet  HWaddr 00:00:00:00:00:00  
          inet addr:192.168.1.45  Bcast:192.168.1.255  Mask:255.255.255.0
          BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

*/

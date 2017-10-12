# Helloworld drivers

After running `make` command, there will be two modules:

* helloworld.ko
* helloworld-params.ko

The fist module is basic helloworld driver, the second one is the same, but
accept some parameters, and print then in the kernel debug messages.
After loading the first module, two entries will be added in kernel debug message:

```bash
#dmesg
[146626.222072] fake-eth added
[146689.422000] fake-fake removed
[146698.060074] fake eth device initialized
[146698.060285] fake-eth added
[146698.087297] IPv6: ADDRCONF(NETDEV_UP): eth0: link is not ready
```


For the second module, one loads it with:

```bash
# insmod  ./helloworld-params.ko
```

If no parameters are provided, the defaults will be used:

```bash
$ dmesg
[...]
[37858.595126] Hello world with parameters!
[37858.595129] The *mystr* parameter: hello
[37858.595130] The *myint* parameter: 1
[37858.595131] The *myarr* parameter: 0, 1, 2
[37887.232643] End of the world
```

When parameters are provided, the will be printed


```bash
# insmod  ./helloworld-params.ko  mystr="packtpub" myint=255 myarr=23,4,7
# dmesg
[...]
[37892.417968] Hello world with parameters!
[37892.417970] The *mystr* parameter: packtpub
[37892.417971] The *myint* parameter: 255
[37892.417972] The *myarr* parameter: 23, 4, 7
[37895.222808] End of the world

```



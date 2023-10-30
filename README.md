# ubus-bm
ubus bandwidth monitor

Very minimal bandwidth monitor for Openwrt.
Exposes usage reports through ubus.
Written in C++

### usage
```
usage: /tmp/ubus-bm [args]

options:
 -h, --h                usage
 -s, --s <socket>       Set the ubus socket to connect to
 -v, --v                verbose logging
 -vv, --vv              extra verbose logging
 -d, --d                debug level logging
 -version, --version    show version information only
```

ubus-bm creates service network.usage with 2 methods.

 - list: lists information for all available interfaces
 - get: requires interface: ```ifd``` to retrieve only information for chosen interface

examples:
```
# ubus call network.usage list
```

and

```
# ubus call network.usage "{'interface': 'br-lan'}"
{
	"br-lan": {
		"rx": {
			"bytes": 59608112270,
			"errors": 0,
			"kilobytes": 58211047,
			"megabytes": 56846,
			"packets": 32322388,
			"rate": {
				"Bytes": 1254,
				"KBytes": 1,
				"MBytes": 0,
				"b": 10032,
				"k": 9,
				"m": 0
			}
		},
		"tx": {
			"bytes": 116064459805,
			"errors": 0,
			"kilobytes": 113344199,
			"megabytes": 110687,
			"packets": 30390653,
			"rate": {
				"Bytes": 99,
				"KBytes": 0,
				"MBytes": 0,
				"b": 792,
				"k": 0,
				"m": 0
			}
		}
	}
}
```

### License
MIT

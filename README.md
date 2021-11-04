# Todis [ [中文](README_CN.md) ]
## 1. Introduction
Todis is a massive, persistent Redis server developed by [Topling Inc.](https://topling.cn)

Currently(2021-10-11), there are many massive, persistent Redis server based on [RocksDB](https://github.com/facebook/rocksdb), and many big company developed their private or open sourced  alternatives(massive, persistent Redis).

These alternative Redis servers are low performance and high cost, and lack of scalability, operationality, monitoring, ....

Todis are aimed on all such pain points, and accomplished these goals, users can experiencing [Managed Todis](https://topling.cn/products) based on aliyun in 10 minutes.

## 2. Features
1. Computing and Storage are seperated for resilient scaling
2. Auto scaling, no need for sharding/partitioning
3. Plenty monitoring metrics
4. Web view for DB internal stats

## 3. Performance
1. With resilent distributed compaction, kicked off write stalls, achieved sustained write speed over 100MB/s on a 2C16G cloud server
2. With searchable in memory compression technology(terark-zip), achieved very high read performance(saturate network bandwitdh)
3. With shared storage and engine level leader-follwer mechanics
   * Sync latency is lower to milliseconds
   * New follower(read only node) can be started and available in 10 seconds

## 4. Architeture
### 4.1. Redis layer of Todis
Redis layer is forked from pika with many improvements:
1. Strorage engine is replaced from RocksDB to ToplingDB
2. Rewrite performance critical code of pika
3. Add many monitoring metrics(server side latency histogram for all commands and data len histogram...)
4. Adapt for ToplingDB SidePlugin, Web view for config and server stats
5. Adapt for ToplingDB distributed compaction
### 4.2. Storage Engine layer (ToplingDB)
ToplingDB is a storage engine developed by [Topling Inc.](https://topling.cn), which forked from [RocksDB](https://github.com/facebook/rocksdb) with many improvments:
1. [SidePlugin](https://github.com/topling/rockside/wiki)
2. [Distributed Compaction](https://github.com/topling/rockside/wiki/Distributed-Compaction) which offload compaction to a dedicated cluster, in [Managed Todis](https://topling.cn/products), this cluster is shared by all users and all Todis instances
3. Topling SST and MemTab which based on [topling-zip](https://github.com/topling/topling-zip) which forked from [terark-zip](https://github.com/bytedance/terark-zip)
   * Terark-zip was a core lib of **Terark Inc.** which was [acquired by bytedance at 2019](https://www.baidu.com/s?wd=%E5%AD%97%E8%8A%82%E8%B7%B3%E5%8A%A8%E6%94%B6%E8%B4%AD%E5%A5%87%E7%AE%80%E8%BD%AF%E4%BB%B6)
   * [Rockeet](https://github.com/rockeet) is the founder of **Terark Inc.** and the author of terark-zip
   * Bytedance open sourced [terark-zip](https://github.com/bytedance/terark-zip) at the end of 2020
   * As the author of terark-zip, rockeet added many enhancements to [topling-zip](https://github.com/topling/topling-zip)(his fork of [terark-zip](https://github.com/bytedance/terark-zip)) since then

<br/>
<hr>
<hr>
<hr>

<img src="https://s1.ax1x.com/2020/05/08/YnbjQf.png" alt="YnbjQf.png"  width="300" />

[![Build Status](https://travis-ci.org/Qihoo360/pika.svg?branch=master)](https://travis-ci.org/Qihoo360/pika) ![Downloads](https://img.shields.io/github/downloads/Qihoo360/pika/total)

## Introduction[中文](README_CN.md)

Pika is a persistent huge storage service , compatible  with the vast majority of redis interfaces ([details](https://github.com/Qihoo360/pika/wiki/pika-支持的redis接口及兼容情况)), including string, hash, list, zset, set and management interfaces. With the huge amount of data stored, redis may suffer for a capacity bottleneck, and pika was born for solving it. Except huge storage capacity, pika also support master-slave mode by slaveof command, including full and partial synchronization. You can also use pika together with twemproxy or codis(*pika has supported data migration in codis，thanks [left2right](https://github.com/left2right) and [fancy-rabbit](https://github.com/fancy-rabbit)*) for distributed Redis solution


## UserList

<table>
<tr>
<td height = "100" width = "150"><img src="http://i.imgur.com/dcHpCm4.png" alt="Qihoo"></td>
<td height = "100" width = "150"><img src="https://i.imgur.com/BIjqe9R.jpg" alt="360game"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/jjZczkN.png" alt="Weibo"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/zoel46r.gif" alt="Garena"></td>
</tr>
<tr>
<td height = "100" width = "150"><img src="http://i.imgur.com/kHqACbn.png" alt="Apus"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/2c57z8U.png" alt="Ffan"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/rUiO5VU.png" alt="Meituan"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/px5mEuW.png" alt="XES"></td>
</tr>
<tr>
<td height = "100" width = "150"><img src="http://imgur.com/yJe4FP8.png" alt="HX"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/o8ZDXCH.png" alt="XL"></td>
<td height = "100" width = "150"><img src="http://imgur.com/w3qNQ9T.png" alt="GWD"></td>
<td height = "100" width = "150"><img src="https://imgur.com/KMVr3Z6.png" alt="DYD"></td>
</tr>
<tr>
<td height = "100" width = "150"><img src="http://i.imgur.com/vJbAfri.png" alt="YM"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/aNxzwsY.png" alt="XM"></td>
<td height = "100" width = "150"><img src="http://i.imgur.com/mrWxwkF.png" alt="XL"></td>
<td height = "100" width = "150"><img src="http://imgur.com/0oaVKlk.png" alt="YM"></td>
</tr>
<tr>
<td height = "100" width = "150"><img src="https://i.imgur.com/PI89mec.png" alt="MM"></td>
<td height = "100" width = "150"><img src="https://i.imgur.com/G9MOvZe.jpg" alt="VIP"></td>
<td height = "100" width = "150"><img src="https://imgur.com/vQW5qr3.png" alt="LK"></td>
<td height = "100" width = "150"><img src="https://i.imgur.com/jIMG4mi.jpg" alt="KS"></td>
</tr>
</table>

[More](https://github.com/Qihoo360/pika/blob/master/USERS.md)

## Feature

* huge storage capacity
* compatible with redis interface, you can migrate to pika easily
* support master-slave mode (slaveof)
* various [management](https://github.com/Qihoo360/pika/wiki/pika的一些管理命令方式说明) interfaces

## For developer

### Releases
The User can download the binary release from [releases](https://github.com/Qihoo360/pika/releases) or compile the source release.

### Dependencies

* snappy - a library for fast data compression
* glog - google log library

Upgrade your gcc to version at least 4.8 to get C++11 support.

### Supported platforms

* linux - CentOS 5&6

* linux - Ubuntu

If it comes to some missing libs, install them according to the prompts and retry it.

### Compile

Upgrade your gcc to version at least 4.8 to get C++11 support.

Get the source code

```
git clone https://github.com/Qihoo360/pika.git
```


Then compile pika, all submodules will be updated automatically.

```
make
```

## Usage

```
./output/bin/pika -c ./conf/pika.conf
```

## Performance

More details on [Performance](https://github.com/Qihoo360/pika/wiki/3.2.x-Performance).


## Documents

1. [Wiki](https://github.com/Qihoo360/pika/wiki)

## Contact Us

Users Mail Group: [join in pika_users@groups.163.com](http://163.fm/7VSsSWUJ)

Developers Mail Group: [join in pika_developers@groups.163.com](http://163.fm/Q70Qbg3v)

QQ group: 294254078

For more information about Pika, Atlas and some other technology please pay attention to our Hulk platform official account

<img src="http://i.imgur.com/pL4ni57.png" height = "50%" width = "50%" alt="2">

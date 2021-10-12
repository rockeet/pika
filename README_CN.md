# Todis

## 简介
Todis 是 [topling（拓扑岭）](https://topling.cn) 出品的大容量、持久化 Redis 服务。

Todis 的 Redis 服务层 fork 自开源的 pika，存储引擎层基于 ToplingDB。

ToplingDB 是 [topling（拓扑岭）](https://topling.cn) 出品的兼容 RocksDB API 的分布式存储引擎。

目前(2021-10-11)，市面上存在很多大容量、持久化的 Redis 实现（一般基于 RocksDB），几乎每个大厂都有自己的相应产品。但是这类产品都存在性能低下、成本高昂、难以扩展、运维不易、监控缺失等问题，依托 ToplingDB 开创性的独有技术，Todis 解决了所有这些问题，并且在持续地不断改进。


## 功能
1. 存储计算分离，计算和存储可单独弹性伸缩
2. 一键扩容，无需分片
3. 完备、丰富的监控指标
4. 可视化 Web 数据观测

## 性能
1. 利用弹性分布式 Compact，彻底消除写卡顿（Write Stall）
2. 利用可检索内存压缩技术大幅提高读性能：CPU消耗降低，内存利用率、缓存命中率提高
3. 利用共享存储实现毫秒级主从同步，可在 10 秒内拉起新结点

## 架构
1. 在协议、执行层面上（pika/pink/blackwidow）
   * Todis 对 pika 进行了深度优化，重写了性能关键代码
   * 增加了一系列监控指标（各命令的延时直方图、数据大小分布直方图）
   * 适配 ToplingDB SidePlugin，Web 展示各种配置，数据概要
   * 适配 ToplingDB 的分布式 Compact
2. 在 ToplingDB 层面上
   * 为 RocksDB 增加 SidePlugin 体系
   * 使用可检索内存压缩替换 RocksDB 的 BlockBasedTable
   * 多租户共享计算结点的弹性分布式 Compact

# Pika

## 简介 [English](README.md)
Pika是一个可持久化的大容量redis存储服务，兼容string、hash、list、zset、set的绝大部分接口([兼容详情](https://github.com/Qihoo360/pika/wiki/pika-支持的redis接口及兼容情况))，解决redis由于存储数据量巨大而导致内存不够用的容量瓶颈，并且可以像redis一样，通过slaveof命令进行主从备份，支持全同步和部分同步，pika还可以用在twemproxy或者codis中来实现静态数据分片（pika已经可以支持codis的动态迁移slot功能，目前在合并到master分支，欢迎使用，感谢作者[left2right](https://github.com/left2right)和[fancy-rabbit](https://github.com/fancy-rabbit)提交的pr）

## Pika用户

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


[更多](https://github.com/Qihoo360/pika/blob/master/USERS.md)

## 特点
* 容量大，支持百G数据量的存储
* 兼容redis，不用修改代码即可平滑从redis迁移到pika
* 支持主从(slaveof)
* 完善的[运维](https://github.com/Qihoo360/pika/wiki/pika的一些管理命令方式说明)命令


## 使用

### 二进制包使用

用户可以直接从[releases](https://github.com/Qihoo360/pika/releases)下载最新的二进制版本包直接使用.

### 编译使用

1.在编译机上安装snappy，glog，CentOS系统可以用yum安装，Ubuntu可以用apt-get安装。如是CentOS系统，执行如下命令：

```
    yum install snappy-devel glog-devel
```

2.安装g++(若没有安装), 在CentOS上执行如下命令：

```
    yum install gcc-c++
```

3.把gcc版本临时切换到4.8(若已是，则忽略), 在CentOS上执行如下命令：

```
	a. sudo wget http://people.centos.org/tru/devtools-2/devtools-2.repo -O /etc/yum.repos.d/devtools-2.repo
	b. sudo yum install -y devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++
	c. scl enable devtoolset-2 bash
```
4.获取源代码

```
	git clone https://github.com/Qihoo360/pika.git && cd pika
```
5.切换到最新release版本

```
	a. 执行 git tag 查看最新的release tag，（如 v2.2.5）
	b. 执行 git checkout TAG切换到最新版本，（如 git checkout v2.2.5）
```

6.编译

```
	make
```

若编译过程中，提示有依赖的库没有安装，则有提示安装后再重新编译

**注：我们推荐使用TCMalloc来进行内存管理**

## 使用
```
	./output/bin/pika -c ./conf/pika.conf
```

## 清空编译

```
  如果需要清空编译内容，视不同情况使用以下两种方法其一：

  1. 执行make clean来清空pika的编译内容
  2. 执行make distclean来清空pika及所有依赖的编译内容（一般用于彻底重新编译）
```

## 性能 (感谢[deep011](https://github.com/deep011)提供性能测试结果)
### 注!!!
本测试结果是在特定环境特定场景下得出的，不能够代表所有环境及场景下的表现，__仅供参考__。

__推荐大家在使用pika前在自己的环境根据自己的使用场景详细测试以评估pika是否满足要求__

### 测试环境

**CPU型号**：Intel(R) Xeon(R) CPU E5-2690 v4 @ 2.60GHz

**CPU线程数**：56

**MEMORY**：256G

**DISK**：3T flash

**NETWORK**：10GBase-T/Full * 2

**OS**：centos 6.6

**Pika版本**：2.2.4

### 压测工具

[**vire-benchmark**](https://deep011.github.io/vire-benchmark)

### 测试一

#### 测试目的

测试在pika不同worker线程数量下，其QPS上限。

#### 测试条件

pika数据容量：800G

value：128字节

CPU未绑定

#### 测试结果

说明：横轴Pika线程数，纵轴QPS，value为128字节。set3/get7代表30%的set和70%的get。

<img src="https://deep011.github.io/public/images/pika_benchmark/pika_threads_test.png" height = "60%" width = "60%" alt="1"/>

#### 结论

从以上测试图可以看出，pika的worker线程数设置为20-24比较划算。

### 测试二

#### 测试目的

测试在最佳worker线程数（20线程）下，pika的rtt表现。

#### 测试条件

**pika数据容量**：800G

**value**：128字节

#### 测试结果

```c
====== GET ======
  10000000 requests completed in 23.10 seconds
  200 parallel clients
  3 bytes payload
  keep alive: 1
99.89% <= 1 milliseconds
100.00% <= 2 milliseconds
100.00% <= 3 milliseconds
100.00% <= 5 milliseconds
100.00% <= 6 milliseconds
100.00% <= 7 milliseconds
100.00% <= 7 milliseconds
432862.97 requests per second
```

```c
====== SET ======
  10000000 requests completed in 36.15 seconds
  200 parallel clients
  3 bytes payload
  keep alive: 1
91.97% <= 1 milliseconds
99.98% <= 2 milliseconds
99.98% <= 3 milliseconds
99.98% <= 4 milliseconds
99.98% <= 5 milliseconds
99.98% <= 6 milliseconds
99.98% <= 7 milliseconds
99.98% <= 9 milliseconds
99.98% <= 10 milliseconds
99.98% <= 11 milliseconds
99.98% <= 12 milliseconds
99.98% <= 13 milliseconds
99.98% <= 16 milliseconds
99.98% <= 18 milliseconds
99.99% <= 19 milliseconds
99.99% <= 23 milliseconds
99.99% <= 24 milliseconds
99.99% <= 25 milliseconds
99.99% <= 27 milliseconds
99.99% <= 28 milliseconds
99.99% <= 34 milliseconds
99.99% <= 37 milliseconds
99.99% <= 39 milliseconds
99.99% <= 40 milliseconds
99.99% <= 46 milliseconds
99.99% <= 48 milliseconds
99.99% <= 49 milliseconds
99.99% <= 50 milliseconds
99.99% <= 51 milliseconds
99.99% <= 52 milliseconds
99.99% <= 61 milliseconds
99.99% <= 63 milliseconds
99.99% <= 72 milliseconds
99.99% <= 73 milliseconds
99.99% <= 74 milliseconds
99.99% <= 76 milliseconds
99.99% <= 83 milliseconds
99.99% <= 84 milliseconds
99.99% <= 88 milliseconds
99.99% <= 89 milliseconds
99.99% <= 133 milliseconds
99.99% <= 134 milliseconds
99.99% <= 146 milliseconds
99.99% <= 147 milliseconds
100.00% <= 203 milliseconds
100.00% <= 204 milliseconds
100.00% <= 208 milliseconds
100.00% <= 217 milliseconds
100.00% <= 218 milliseconds
100.00% <= 219 milliseconds
100.00% <= 220 milliseconds
100.00% <= 229 milliseconds
100.00% <= 229 milliseconds
276617.50 requests per second
```

#### 结论

get/set 响应时间 99.9%都在2ms以内。

### 测试三

#### 测试目的

在pika最佳的worker线程数下，查看各命令的极限QPS。

#### 测试条件

**pika的worker线程数**：20

**key数量**：10000

**field数量**：100（list除外）

**value**：128字节

**命令执行次数**：1000万（lrange除外）

#### 测试结果

```c
PING_INLINE: 548606.50 requests per second
PING_BULK: 544573.31 requests per second
SET: 231830.31 requests per second
GET: 512163.91 requests per second
INCR: 230861.56 requests per second
MSET (10 keys): 94991.12 requests per second
LPUSH: 196093.81 requests per second
RPUSH: 195186.69 requests per second
LPOP: 131156.14 requests per second
RPOP: 152292.77 requests per second
LPUSH (needed to benchmark LRANGE): 196734.20 requests per second
LRANGE_10 (first 10 elements): 334448.16 requests per second
LRANGE_100 (first 100 elements): 50705.12 requests per second
LRANGE_300 (first 300 elements): 16745.16 requests per second
LRANGE_450 (first 450 elements): 6787.94 requests per second
LRANGE_600 (first 600 elements): 3170.38 requests per second
SADD: 160885.52 requests per second
SPOP: 128920.80 requests per second
HSET: 180209.41 requests per second
HINCRBY: 153364.81 requests per second
HINCRBYFLOAT: 141095.47 requests per second
HGET: 506791.00 requests per second
HMSET (10 fields): 27777.31 requests per second
HMGET (10 fields): 38998.52 requests per second
HGETALL: 109059.58 requests per second
ZADD: 120583.62 requests per second
ZREM: 161689.33 requests per second
PFADD: 6153.47 requests per second
PFCOUNT: 28312.57 requests per second
PFADD (needed to benchmark PFMERGE): 6166.37 requests per second
PFMERGE: 6007.09 requests per second
```

#### 结论

整体表现很不错，个别命令表现较弱（LRANGE，PFADD，PFMERGE）。

### 测试四

#### 测试目的

Pika与Redis的极限QPS对比。

#### 测试条件

**pika的worker线程数**：20

**key数量**：10000

**field数量**：100（list除外）

**value**：128字节

**命令执行次数**：1000万（lrange除外）

**Redis版本**：3.2.0

#### 测试结果

<img src="https://deep011.github.io/public/images/pika_benchmark/pika_vs_redis_qps.png" height = "60%" width = "60%" alt="1"/>

## 文档
1. [Wiki] (https://github.com/Qihoo360/pika/wiki)

## 联系方式
邮箱：g-infra@360.cn

QQ群：294254078

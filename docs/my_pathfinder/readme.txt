目的 把pathfinder A*寻路功能封装成dll

1. 0ad里使用了一些c运行时的库，变成dll后 造成全局变量不能顺利生成
2. 其使用的simulation及Component类似于iid类厂概念， 直接使用接口方式调用
3.解耦合后， 最大问题出在EDGE_EXPAND_DELTA， 该变量为全局变量， dll后不能顺利赋值， 被置为了0

使用
把当前目录中的全部文件覆盖到0ad目录下

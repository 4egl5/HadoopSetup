**NOT FOR PRODUCTION** 

```
wget https://raw.githubusercontent.com/4egl5/HadoopSetup/main/hadoopSetup.sh && chmod +x hadoopSetup.sh && ./hadoopSetup.sh -gs
```
Basic setup for hadoop
type exit after login to localhost ssh
Tested on Ubuntu 22.04 and 23.10 server

after the scipt run,
```
sudo reboot
```
wait for reboot and open terminal
```
start-dfs.sh
start-yarn.sh
$ZEPPELIN_HOME/bin/zeppelin-daemon.sh start
jps
```
Check if all the process running, sth like this
```
4898 ZeppelinServer
1733 DataNode
2246 NodeManager
2807 NameNode
2106 ResourceManager
5101 Jps
1919 SecondaryNameNode
```

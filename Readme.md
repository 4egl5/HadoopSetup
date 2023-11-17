# **NOT FOR PRODUCTION, and NOT SECURE**
# Debian based distro only

- Only tested on Ubuntu 22.04LTS and 23.10 server
- Modify the params of the script as needed
- Data in hadoop will loss after reboot due to default path is tmp, see: https://stackoverflow.com/questions/28379048/data-lost-after-shutting-down-hadoop-hdfs 
```
wget https://raw.githubusercontent.com/4egl5/HadoopSetup/main/hadoopSetup.sh && chmod +x hadoopSetup.sh && ./hadoopSetup.sh -gs
```


# Usage of the script
```
Usage: 		./hadoopSetup.sh [params] -d [default_desktop_env_name]
Usage example: 	./hadoopSetup.sh -g
Usage example: 	./hadoopSetup.sh -gxms -d g

-h:	Print help function

-g:	Install gnome
-x:	Install xfce
-m:	Install mate

-d	[default_desktop_env_name]: set default desktop environment
[default_desktop_env_name]:
g: gnome
x: xfce
m: mate

-s:	set VM default user password
```

# After create the VM
- This code use xrdp if install GUI params used (eg:-g)
- For VNC, please modify the script on your own by following https://docs.xenserver.com/en-us/citrix-hypervisor/vms/enable-vnc-linux.html 
- Ensure open ports for the RDP / VNC if you wish to use remote desktop 


Check if all the process running, especially namenode, sth like this
```
4898 ZeppelinServer
1733 DataNode
2246 NodeManager
2807 NameNode
2106 ResourceManager
5101 Jps
1919 SecondaryNameNode
```
And check from GUI:
- Resource Manager:    http://localhost:8088 
- Hadoop GUI:          http://localhost:9870
- Hadoop FileSystem:   http://localhost:9870/explorer.html#/
- Zeppelin:            http://localhost:8080/

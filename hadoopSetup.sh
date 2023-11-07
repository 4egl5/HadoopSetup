#!/usr/bin/bash -i
cd ~

gnome=0
xfce=0
mate=0
setpw=false
defaultDE=""
xrdp="xrdp"

if [[ $# > 0 ]]; then
    while getopts 'hgxmsd:' OPTION; do
        case "$OPTION" in
        h)
            echo "Usage: 		./hadoopSetup.sh [params] -d [default_desktop_env_name]"
            echo "Usage example: 	./hadoopSetup.sh -g"
            echo "Usage example: 	./hadoopSetup.sh -gxms -d g"
            echo ""
            echo "-h:	Print help function"
            echo ""
            echo "-g:	Install gnome"
            echo "-x:	Install xfce"
            echo "-m:	Install mate"
            echo ""
            echo "-d	[default_desktop_env_name]: set default desktop environment"
            echo "[default_desktop_env_name]:"
            echo "g: gnome"
            echo "x: xfce"
            echo "m: mate"
            echo ""
            echo "-s:	set VM default user password"
            exit
        ;;
        g)
            gnome=1
        ;;
        x)
            xfce=1
        ;;
        m)
            mate=1
        ;;
        s)
            setpw=true
        ;;
        d)
            defaultDE=${OPTARG}
        ;;
        \?) 
            exit
        ;;
        :) 
            exit
        ;;
        esac
    done
fi

if [[ $((gnome+xfce+mate)) < 1 ]]; then
    xrdp=""
fi

if [[ $((gnome+xfce+mate)) == 1 ]]; then
    if [[ $gnome == 1 ]]; then
        defaultDE="gnome-session"
    fi
    if [[ $xfce == 1 ]]; then
        defaultDE="xfce-session"
    fi
    if [[ $mate == 1 ]]; then
        defaultDE="mate-session"
    fi
fi

if [[ $((gnome+xfce+mate)) > 1 ]]; then
    if [ $defaultDE == "" ]; then
        echo "option requires an argument -- d"
    elif ( [[ $gnome == 1 ]] && ([[ $defaultDE == "g" ]] || [[ $defaultDE == "gnome" ]]));then
        defaultDE="gnome-session"
    elif ( [[ $xfce == 1 ]] && ([[ $defaultDE == "x" ]] || [[ $defaultDE == "xfce" ]]));then    
        defaultDE="xfce-session"
    elif ( [[ $mate == 1 ]] && ([[ $defaultDE == "m" ]] || [[ $defaultDE == "mate" ]]));then    
        defaultDE="mate-session"
    else
        echo "illegal argument -- d"
        exit
    fi
fi


if [[ $gnome == 1 ]];then 
    gnome="gnome-session gnome-terminal"
else
    gnome=""
fi

if [[ $xfce == 1 ]];then 
    xfce="xfce4 xfce4-session"
else
    xfce=""
fi

if [[ $mate == 1 ]];then 
    mate="mate-desktop-environment"
else
    mate=""
fi


sudo apt-get -qq update
sudo DEBIAN_FRONTEND=noninteractive apt-get -qq -y install $xrdp $gnome $xfce $mate

if [[ $xrdp != "" ]];then
    sudo systemctl enable xrdp
    sudo adduser xrdp ssl-cert
    echo $defaultDE>$HOME/.xsession
    sudo service xrdp restart
    if [[ $setpw == true ]];then
        sudo passwd $USER
    fi
fi

sudo apt install -qq -y openjdk-8-jdk
echo 'export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"'>>$HOME/.profile
echo 'export HADOOP_HOME="/usr/local/hadoop"'>>$HOME/.profile
source $HOME/.profile

wget https://dlcdn.apache.org/hadoop/common/hadoop-3.3.6/hadoop-3.3.6.tar.gz
tar -xzvf hadoop-3.3.6.tar.gz
sudo mv $HOME/hadoop-3.3.6 $HADOOP_HOME
echo 'export PATH="$PATH:$HADOOP_HOME/bin"'>>$HOME/.profile
source $HOME/.profile
sed '/<configuration>/a \ \ <property>\n\ \ \ \ <name>fs.defaultFS</name>\n\ \ \ \ <value>hdfs://localhost:9000</value>\n\ \ </property>' -i $HADOOP_HOME/etc/hadoop/core-site.xml
sed '/<configuration>/a \ \ <property>\n\ \ \ \ <name>dfs.replication</name>\n\ \ \ \ <value>1</value>\n\ \ </property>' -i $HADOOP_HOME/etc/hadoop/hdfs-site.xml

ssh-keygen
cat $HOME/.ssh/id_rsa.pub>>$HOME/.ssh/authorized_keys
chmod 600 $HOME/.ssh/authorized_keys
echo chown -R "$(whoami):" $HADOOP_HOME
hdfs namenode -format
echo 'export PATH="$PATH:$HADOOP_HOME/sbin"'>>$HOME/.profile
echo 'export JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64"'>>$HADOOP_HOME/etc/hadoop/hadoop-env.sh
source $HOME/.profile

start-dfs.sh&&start-yarn.sh&&jps

wget https://dlcdn.apache.org/hive/hive-3.1.3/apache-hive-3.1.3-bin.tar.gz
tar -xzf  apache-hive-3.1.3-bin.tar.gz
sudo mv apache-hive-3.1.3-bin /usr/local/hive

echo '#Hadoop Path'>>$HOME/.bashrc
echo 'export HADOOP_HOME=/usr/local/hadoop' >>$HOME/.bashrc
echo 'export PATH=$PATH:$HADOOP_HOME/bin' >>$HOME/.bashrc
echo 'export PATH=$PATH:$HADOOP_HOME/sbin' >>$HOME/.bashrc
echo 'export HADOOP_MAPRED_HOME=${HADOOP_HOME}' >>$HOME/.bashrc
echo 'export HADOOP_COMMON_HOME=${HADOOP_HOME}' >>$HOME/.bashrc
echo 'export HADOOP_HDFS_HOME=${HADOOP_HOME}' >>$HOME/.bashrc
echo 'export YARN_HOME=${HADOOP_HOME}' >>$HOME/.bashrc
echo ' '>>$HOME/.bashrc
echo '#Hive configurations' >>$HOME/.bashrc
echo 'export HIVE_HOME=/usr/local/hive' >>$HOME/.bashrc
echo 'export PATH=$PATH:$HIVE_HOME/bin' >>$HOME/.bashrc
echo 'export HIVE_CONF_DIR=$HIVE_HOME/conf' >>$HOME/.bashrc
echo 'export CLASSPATH=$CLASSPATH:$HADOOP_HOME/lib/*:.' >>$HOME/.bashrc
echo 'export CLASSPATH=$CLASSPATH:$HIVE_HOME/lib/*:.' >>$HOME/.bashrc
source $HOME/.bashrc

sed '/# HADOOP_HOME/c HADOOP_HOME=/usr/local/hadoop' $HIVE_HOME/conf/hive-env.sh.template > $HIVE_HOME/conf/hive-env.sh
sed '/<!-- Hive Execution Parameters -->/a \ \ <property>\n\ \ \ \ <name>system:java.io.tmpdir</name>\n\ \ \ \ <value>/tmp/hive/java</value>\n\ \ </property>\n\ \ <property>\n\ \ <name>system:user.name</name>\n\ \ \ \ <value>${user.name}</value>\n\ \ </property>' $HIVE_HOME/conf/hive-default.xml.template > $HIVE_HOME/conf/hive-site.xml

hadoop fs -mkdir -p $HIVE_HOME/user/hive/warehouse
hadoop fs -chmod g+w $HIVE_HOME/user/hive/warehouse

cd $HIVE_HOME
bin/schematool -dbType derby -initSchema
sed 's/\;databaseName=/\/usr\/local\/hive\//' -i $HIVE_HOME/conf/hive-site.xml
sed 's/\&\#8\;/\ /' t -i  $HIVE_HOME/conf/hive-site.xml
cd ~
tar xvzf pig-0.16.0.tar.gz
sudo mv pig-0.16.0 /usr/local/hadoop/pig

echo '#PIG settings'>> $HOME/.bashrc
echo 'export PIG_HOME=/usr/local/hadoop/pig'>> $HOME/.bashrc
echo 'export PATH=$PATH:$PIG_HOME/bin'>> $HOME/.bashrc
echo 'export PIG_CLASSPATH=$PIG_HOME/conf:$HADOOP_INSTALL/etc/hadoop/'>> $HOME/.bashrc
echo 'export PIG_CONF_DIR=$PIG_HOME/conf'>> $HOME/.bashrc
echo 'export PIG_CLASSPATH=$PIG_CONF_DIR:$PATH'>> $HOME/.bashrc
echo '#PIG setting ends'>> $HOME/.bashrc

source $HOME/.bashrc

stop-dfs.sh&&start-dfs.sh&&start-yarn.sh&&jps

wget https://dlcdn.apache.org/zeppelin/zeppelin-0.10.1/zeppelin-0.10.1-bin-all.tgz
tar -xzvf zeppelin-0.10.1-bin-all.tgz

echo '## zeppelin settings' >> $HOME/.bashrc
echo 'export ZEPPELIN_HOME=/usr/local/zeppelin'>> $HOME/.bashrc
source $HOME/.bashrc 

sudo mv zeppelin-0.10.1-bin-all $ZEPPELIN_HOME
cd $ZEPPELIN_HOME
bin/zeppelin-daemon.sh start
cd ~

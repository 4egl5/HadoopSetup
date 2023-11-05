#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <filesystem>
#include <fstream>

void helpFunction(){
    std::cout<<std::endl;
    std::cout<<"help"<<std::setw(29)<<"Print helper function"<<std::endl;
    std::cout<<std::endl;

    std::cout<<"gnome"<<std::setw(40)<<"Install Gnome Desktop Environment"<<std::endl;
    // std::cout<<"kde"<<std::setw(40)<<"Install KDE Desktop Environment"<<std::endl;
    std::cout<<"xfce"<<std::setw(40)<<"Install XFCE Desktop Environment"<<std::endl;
    std::cout<<"mate"<<std::setw(40)<<"Install MATE Desktop Environment"<<std::endl;
    std::cout<<std::endl;
    std::cout<<"default={de_name}"<<std::setw(78)<<"Set default desktop environment, use when more than 1 desktop env install"<<std::endl;
    std::cout<<std::endl;

    std::cout<<"skippw"<<std::setw(50)<<"Skip setting local user account password\n";
    std::cout<<std::setw(92)<<"Use if you created a password for your user account when you created your VM."<<std::endl;
}

std::string installDE_And_XRDP(bool g,/*bool k,*/ bool x,bool m,bool pw, std::string de){
    system("cd ~");
    std::string install = "sudo DEBIAN_FRONTEND=noninteractive apt-get -y install xrdp";
    if(g){
        install +=" gnome-session gnome-terminal";
        if (de ==""){
            de = "gnome-session";
        }
    }
    // if(k){
    //     install +=" kde-plasma-desktop";
    // }
    if(x){
        install +=" xfce4 xfce4-session";
        if (de ==""){
            de = "xfce-session";
        }
    }
    if(m){
        install +=" mate-desktop-environment-extras";
        if (de ==""){
            de = "mate-session";
        }
    }
    // const char* i = install;
    system(install.c_str());
    system("sudo systemctl enable xrdp");
    system("sudo adduser xrdp ssl-cert");
    system(("echo "+de+" >~/.xsession").c_str());
    system("sudo service xrdp restart");
    if (pw){
        system("sudo paasswd $USER");
    }
    return "Desktop Environment and XDRP setup successful\n";
}

std::string installJava(){
    system("cd ~");
    system("sudo apt install -y openjdk-8-jdk");
    system("echo 'export JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64''>>.profile");
    system("echo 'export HADOOP_HOME='/usr/local/hadoop''>>.profile");
    system("source ~/.profile");
    return "Java installed\n";
}

std::string installHadoop(){
    system("cd ~");
    system("wget https://dlcdn.apache.org/hadoop/common/hadoop-3.3.6/hadoop-3.3.6.tar.gz");
    system("tar -xzvf hadoop-3.3.6.tar.gz");
    system("sudo mv hadoop-3.3.6 '$HADOOP_HOME'");
    system("echo 'export PATH=\"$PATH:$HADOOP_HOME/bin\"'>>$HOME/.profile");
    system("source .profile");
    return "Hadoop installed\n";
}

std::string configHadoop(){
    system("cd ~");
    // config $HADOOP_HOME/etc/hadoop/core-site.xml
    std::ifstream coresite;
    std::ofstream coresite_o;
    coresite.open("$HADOOP_HOME/etc/hadoop/core-site.xml");
    coresite_o.open("$HADOOP_HOME/etc/hadoop/core-site2.xml");
    while(!coresite.eof()){
        std::string str = "";
        coresite>>str;
        coresite_o<<str<<" ";
        if(str=="<configuration>"){
            coresite_o<<"\n<property><name>fs.defaultFS</name><value>hdfs://localhost:9000</value></property>";

        }
        if(str!=""&&str[0]=='<'&&str[str.length()-1]=='>'){
            coresite_o<<std::endl;
        }
    }
    coresite.close();
    coresite_o.close();
    system("mv $HADOOP_HOME/etc/hadoop/core-site2.xml $HADOOP_HOME/etc/hadoop/core-site.xml");



    // config $HADOOP_HOME/etc/hadoop/hdfs-site.xml
    std::ifstream hdfssite;
    std::ofstream hdfssite_o;
    hdfssite.open("$HADOOP_HOME/etc/hadoop/hdfs-site.xml");
    hdfssite_o.open("$$HADOOP_HOME/etc/hadoop/hdfs-site2.xml");
    while(!coresite.eof()){
        std::string str = "";
        hdfssite>>str;
        hdfssite_o<<str<<" ";
        if(str=="<configuration>"){
            hdfssite_o<<"\n<property><name>dfs.replication</name><value>1</value></property>";
        }
        if(str!=""&&str[0]=='<'&&str[str.length()-1]=='>'){
            hdfssite_o<<std::endl;
        }
    }
    hdfssite.close();
    hdfssite_o.close();
    system("mv $HADOOP_HOME/etc/hadoop/hdfs-site2.xml $HADOOP_HOME/etc/hadoop/hdfs-site.xml");

    
    system("ssh-keygen");
    system("cat $HOME/.ssh/id_rsa.pub>>$HOME/.ssh/authorized_keys");
    system("chmod 600 $HOME/.ssh/authorized_keys");

    system("sudo chown -R '$(whoami):' '$HADOOP_HOME'");
    system("hdfs namenode -format");
    system("echo 'export PATH=\"$PATH:$HADOOP_HOME/sbin\"'>>$HOME/.profile");
    system("source $HOME/.profile");
    system("echo 'export JAVA_HOME=\"/usr/lib/jvm/java-8-openjdk-amd64\"'>>'$HADOOP_HOME/etc/hadoop/hadoop-env.sh'");
    system("start-dfs.sh");
    system("start-yarn.sh");
    return "Hadoop configured\n";
}

std::string installHive(){
    system("cd ~");
    system("wget https://dlcdn.apache.org/hive/hive-3.1.3/apache-hive-3.1.3-bin.tar.gz");
    system("tar -xzf  apache-hive-3.1.3-bin.tar.gz");
    system("sudo mv apache-hive-3.1.3-bin /usr/local/hive");
    std::ofstream bashrc;
    bashrc.open(".bashrc",std::ios_base::app);
    bashrc<<"\n#Hadoop Path\nexport HADOOP_HOME=/usr/local/hadoop\nexport PATH=$PATH:$HADOOP_HOME/bin\nexport PATH=$PATH:$HADOOP_HOME/sbin\nexport HADOOP_MAPRED_HOME=${HADOOP_HOME}\nexport HADOOP_COMMON_HOME=${HADOOP_HOME}\nexport HADOOP_HDFS_HOME=${HADOOP_HOME}\nexport YARN_HOME=${HADOOP_HOME}\n\n#Hive configurations\nexport HIVE_HOME=/usr/local/hive\nexport PATH=$PATH:$HIVE_HOME/bin\nexport HIVE_CONF_DIR=$HIVE_HOME/conf\nexport CLASSPATH=$CLASSPATH:$HADOOP_HOME/lib/*:.\nexport CLASSPATH=$CLASSPATH:$HIVE_HOME/lib/*:.\n";
    bashrc.close();
    system("source .bashrc");
    return "Hive installed\n";
}

std::string configHive(){
    system("cd ~");
    // config $HIVE_HOME/conf/hive-env.sh
    std::ifstream hiveenv;
    std::ofstream hiveenv_o;
    hiveenv.open("$HIVE_HOME/conf/hive-env.sh.template");
    hiveenv_o.open("$HIVE_HOME/conf/hive-env.sh");
    while(!hiveenv.eof()){
        std::string str = "";
        hiveenv>>str;
        if(str!=""&&str[0]=='#'){
            hiveenv_o<<std::endl;
        }
        if (str=="HADOOP_HOME=${bin}/../../hadoop"){
            str="\nHADOOP_HOME=/usr/local/hadoop";
        }
        hiveenv_o<<str<<" ";
    }
    hiveenv.close();
    hiveenv_o.close();

    // config $HIVE_HOME/conf/hive-site.xml
    std::ifstream hivesite;
    std::ofstream hivesite_o;
    hivesite.open("$HIVE_HOME/conf/hive-default.xml.template");
    hivesite_o.open("$HIVE_HOME/conf/hive-site.xml");    
    while(!hivesite.eof()){
        std::string str = "";
        hivesite>>str;
        hivesite_o<<str<<" ";
        if(str=="--><configuration>"){
            hivesite_o<<"\n<property>\n<name>system:java.io.tmpdir</name>\n<value>/tmp/hive/java</value>\n</property>\n<property>\n<name>system:user.name</name>\n<value>${user.name}</value>\n</property>";
        }
        if(str=="<value>jdbc:derby:;databaseName=metastore_db;create=true</value>"){
            hivesite_o<<"\n<value>jdbc:derby:/usr/local/hive/metastore_db;create=true</value>";
        }
        if(str!=""&&str[0]=='<'&&str[str.length()-1]=='>'){
            hivesite_o<<std::endl;
        }
    }
    hivesite.close();
    hivesite_o.close();

    system("hadoop fs -mkdir -p /user/hive/warehouse");
    system("hadoop fs -chmod g+w /user/hive/warehouse");

    system("cd $HIVE_HOME");
    system("bin/schematool -dbType derby -initSchema");
    return "Hive configured\n";
}

std::string installPig(){
    system("cd ~");
    system("wget https://dlcdn.apache.org/pig/pig-0.17.0/pig-0.17.0.tar.gz");
    system("tar xvzf pig-0.17.0.tar.gz");
    system("sudo mv pig-0.17.0 /usr/local/hadoop/pig");
    
    std::ofstream bashrc;
    bashrc.open(".bashrc",std::ios_base::app);
    bashrc<<"\n#PIG settings\nexport PIG_HOME=/usr/local/hadoop/pig\nexport PATH=$PATH:$PIG_HOME/bin\nexport PIG_CLASSPATH=$PIG_HOME/conf:$HADOOP_INSTALL/etc/hadoop/\nexport PIG_CONF_DIR=$PIG_HOME/conf\nexport PIG_CLASSPATH=$PIG_CONF_DIR:$PATH\n#PIG setting ends\n";
    bashrc.close();
    system("source .bashrc");
    system("stop-dfs.sh");
    system("start-dfs.sh");
    system("start-yarn.sh");
    return "Pig installed\n";

}

std::string installZeppelin(){
    system("cd ~");
    system("wget https://dlcdn.apache.org/zeppelin/zeppelin-0.10.1/zeppelin-0.10.1-bin-all.tgz");
    system("tar -xzvf zeppelin-0.10.1-bin-all.tgz");
    system("sudo mv zeppelin-0.10.1-bin-all /usr/local/zeppelin");

    std::ofstream bashrc;
    bashrc.open(".bashrc",std::ios_base::app);
    bashrc<<"\n## zeppelin settings\nexport ZEPPELIN_HOME=/usr/local/zeppelin\n";
    bashrc.close();
    system("source .bashrc");
    system("cd $ZEPPELIN_HOME");
    system("bin/zeppelin-daemon.sh start");
    return "Zeppelin installed\n";
}


int main(int argc, char **argv){
    bool help = false;
    bool gnome = false;
    // bool kde = false;
    bool xfce = false;
    bool mate = false;
    bool setpw = true;
    std::string defaultDE= "";


    for(int i =1; i<argc;i++){
        if(std::string(argv[i])!="help"&&std::string(argv[i])!="gnome"&&/*std::string(argv[i])!="kde"||*/std::string(argv[i])!="xfce"&&std::string(argv[i])!="mate"&&std::string(argv[i])!="skippw"&&std::string(argv[i]).substr(0,8)!="default="){
            std::cout<<"Unknown params: "<<argv[i]<<", quitting\n";
            return -1;
        }else{
            if (std::string(argv[i])=="help"){
                help = true;
            }
            if (std::string(argv[i])=="gnome"){
                gnome = true;
            }
            // if (std::string(argv[i])=="kde"){
            //     kde = true;
            // }
            if (std::string(argv[i])=="xfce"){
                xfce = true;
            }
            if (std::string(argv[i])=="mate"){
                mate = true;
            }
            
            if (std::string(argv[i])=="skippw"){
                setpw = false;
            }
            if (std::string(argv[i]).substr(0,8)=="default="){
                    defaultDE = std::string(argv[i]).substr(8,std::string(argv[i]).length()-8);
                        if(defaultDE!="gnome"&&/*defaultDE!="kde"&&*/defaultDE!="xfce"&&defaultDE!="mate"){
                            std::cout<<"Wrong default params\nExpect gnome, xfce, mate, get "<<defaultDE<<", quitting\n";
                            return -1;
                        }
            }
        }
    }
    if (defaultDE==""&&((setpw==false&&argc>2)||(setpw==true&&argc>3))){
        std::cout<<("Missing default params, quitting\n");
        return -1;
    }
    if (help==true){
        helpFunction();
        return 0;
    }
    system("sudo apt-get -y update");
    std::cout<<installDE_And_XRDP(gnome,/*kde,*/xfce,mate,setpw,defaultDE);
    std::cout<<installJava();
    std::cout<<installHadoop();
    std::cout<<configHadoop();
    std::cout<<installHive();
    std::cout<<configHive();
    std::cout<<installPig();
    std::cout<<installZeppelin();
    return 0;

}

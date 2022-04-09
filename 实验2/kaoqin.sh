#! /bin/bash

function main()
{
	CreateFile	
	hour=$(date +%H)
	minute=$(date +%M)
	second=$(date +%S)
	while true ;do
		Welcome
		read -p "请输入你的操作:" opt
		case $opt in
		1)
			Login
			LoginSuccess $username
		;;
		2)
			Register
		;;
		3)
			Exit
		;;
		esac
	done
}
function CreateFile() {
    if [ ! -e userinfo.dat ]; then
        touch userinfo.dat
        chmod 777 userinfo.dat
    fi
    if [ ! -e check.dat ]; then
        touch check.dat
        chmod 777 check.dat
    fi
}
function Welcome()
{
	clear
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo  "          欢迎使用考勤系统"
	echo  ""
	echo  "             1.登录打卡"
	echo  "             2.注册"
	echo  "             3.退出系统"
	echo  "         当前时间:$hour:$minute:$second"
	echo  ""
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
}
function Register()
{
	clear	
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo  "             用户注册"
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	while true;do
		read -p "用户名:" newusername
		read -p "密码:" newpasswd
		read -p "确认密码:" new_passwd
		if [ "$newpasswd" == "$new_passwd" ];then
			echo "$newusername $newpasswd" >> userinfo.dat
			echo "注册成功！"
			return
		else
			echo "两次密码不一致！请重新输入！"
		fi
	done
}
function Login()
{
	read -p "请输入用户名:" username
	read -p "请输入密码:" passward
	while read content; do
	if [ "$content" == "$username $passward" ];then
		return 0
	fi
	done < userinfo.dat
	echo "用户名或密码错误，请重新输入"
	Login
}
function LoginSuccess()
{
	clear	
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo  "          欢迎使用考勤系统"
	echo  ""
	echo  "             1.上班打卡"
	echo  "             2.下班打卡"
	echo  "             3.考勤查询"
	echo  "             4.登出"
	echo  ""
	echo  "         当前时间:$hour:$minute:$second"
	echo  ""
	echo  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	echo  ""
	while read -p "请输入你需要的操作:" useropt; do
		case $useropt in
		1)
			ClockIN $username
		;;
		2)
			ClockOut $username
		;;
		3)
			Check $username
		;;
		4)
			return
		;;
		esac
	done
}
function Exit()
{
	echo  "系统将在3秒后退出,谢谢使用..."
	sleep 3
	exit 0
}
function ClockIN()
{
    if [ $hour -gt 8 ]; then
        echo "你上班迟到了！"
        echo "$1 上班迟到-日期：$(date "+%y-%m-%d %H:%M:%S")" >> check.dat
    else
        echo "上班打卡成功！"
    fi
}
function ClockOut()
{
	echo "下班打卡成功！"
	if [ $hour -lt 17 ]; then
		echo "你早退了！"
		echo "$1 下班早退-日期：$(date "+%y-%m-%d %H:%M:%S")" >> check.dat
	fi
}
function Check()
{
	cat check.dat | grep -w "$1"
}

main


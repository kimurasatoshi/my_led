# 2019ロボットシステム学 課題１  
# 概要  
入力した数字の倍の回数点滅させる（３まで）
## 手法  
### インストール手順
```
$ git clone https://github.com/KosekiTakashi/myled.git
$ cd myled
$ make
$ sudo insmod myled.ko
$ sudo chmod 666 /dev/myled0
```    
## 実行
```
$ echo 1 > /dev/myled0
$ echo 2 > /dev/myled0
$ echo 3 > /dev/myled0
```  
## youtube
https://

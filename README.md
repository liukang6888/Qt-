# Qt-
	Qt汽车销售管理项目使用方法

1：首先创建一个数据库，把sql.txt中的sql语句执行
	可以看从sql语句中看出 
	账号：liukang
	密码：123456

2：把CarManger.pro.user文件删了(如果有的话，注意是.user)，重新配置
	
	点击CarManger.pro项目文件运行

3：打开login.cpp
	在38行的函数中修改你的数据库的登录用户和密码  （这是我的，修改成你的呀）
	    db.setUserName("root");   // 数据库登录账号
   	    db.setPassword("root"); // 密码
4：编译运行即可

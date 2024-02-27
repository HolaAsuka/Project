<?php
// 处理增加操作的页面 
require "dbconfig.php";
// 连接mysql
$link = @mysql_connect(HOST,USER,PASS) or die("提示：数据库连接失败！");
// 选择数据库
mysql_select_db(DBNAME,$link);
// 编码设置
mysql_set_charset('utf8',$link);

// 获取增加的电影
$id = $_POST['id'];
$title = $_POST['title'];
$director = $_POST['director'];
$actor = $_POST['actor'];
$duration = $_POST['duration'];
$type = $_POST['type'];
// 插入数据
mysql_query("INSERT INTO movies(id,title,director,actor,year,type) VALUES ('$id','$title','$director','$actor','$duration','$type')",$link) or die('添加数据出错：'.mysql_error());
header("Location:index.php");


<?php
// 处理编辑操作的页面 
require "dbconfig.php";
// 连接mysql
$link = @mysql_connect(HOST,USER,PASS) or die("提示：数据库连接失败！");
// 选择数据库
mysql_select_db(DBNAME,$link);
// 编码设置
mysql_set_charset('utf8',$link);

// 获取修改的电影信息
$id = $_POST['id'];
$title = $_POST['title'];
$director = $_POST['director'];
$actor = $_POST['actor'];
$duration = $_POST['duration'];
$type = $_POST['type'];
// 更新数据
mysql_query("UPDATE movies SET title='$title',director='$director',actor='$actor',duration='$duration',type='$type' WHERE id='$id'",$link) or die('修改数据出错：'.mysql_error());
header("Location:index.php");

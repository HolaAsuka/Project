<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>修改电影</title>
</head>
<body>
<?php
    require "dbconfig.php";

    $link = @mysql_connect(HOST,USER,PASS) or die("提示：数据库连接失败！");
    mysql_select_db(DBNAME,$link);
    mysql_set_charset('utf8',$link);
    
    $id = $_GET['id'];
    $sql = mysql_query("SELECT * FROM movies WHERE id=$id",$link);
    $sql_arr = mysql_fetch_assoc($sql); 

?>

<form action="action-editmovie.php" method="post">
    <label>ID：</label><input type="text" name="id" value="<?php echo $sql_arr['id']?>" readonly>
    <label>标题：</label><input type="text" name="title" value="<?php echo $sql_arr['title']?>">
    <label>导演：</label><input type="text" name="director" value="<?php echo $sql_arr['director']?>">
    <label>主演：</label><input type="text" name="actor" value="<?php echo $sql_arr['actor']?>">
    <label>时长：</label><input type="text" name="duration" value="<?php echo $sql_arr['duration']?>">
    <label>类别：</label><input type="text" name="type" value="<?php echo $sql_arr['type']?>">
    <input type="submit" value="提交">
</form>

</body>
</html>

<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>搜索电影结果</title>
</head>
<style type="text/css">
    .wrapper {width: 1000px;margin: 20px auto;}
    h2 {text-align: center;}
    .topbtn {margin-bottom: 20px;text-align: center;}
    .topbtn a {text-decoration: none;color: #fff;background-color: green;padding: 6px;border-radius: 5px;}
    .btn{text-decoration: none;color: #fff;background-color: red}
    td {text-align: center;}
</style>
<body>
<div class="wrapper">
    <h2>搜索电影结果</h2>
    <div class="topbtn">
        <a href="index.php">返回主页</a>
    </div>
    <table width="960" border="1">
        <tr>
            <th>ID</th>
            <th>标题</th>
            <th>导演</th>
            <th>主演</th>
            <th>时长</th>
            <th>类别</th>
            <th>操作</th>
        </tr>

        <?php

        require "dbconfig.php";


        $link = @mysql_connect(HOST,USER,PASS) or die("提示：数据库连接失败！");

        mysql_select_db(DBNAME,$link);

        mysql_set_charset('utf8',$link);

        $id = $_POST['id'];
        $title = $_POST['title'];
        $director = $_POST['director'];
        $actor = $_POST['actor'];
        $duration = $_POST['duration'];
        $type = $_POST['type'];

        $sql = "select * from movies where id like \"%$id%\" and title like \"%$title%\" and director like \"%$director%\" and actor like \"%$actor%\" and duration like \"%$duration%\" and type like \"%$type%\" order by id asc";

        $result = mysql_query($sql,$link);

        $moviesNum=mysql_num_rows($result);

        for($i=0; $i<$moviesNum; $i++){
            $row = mysql_fetch_assoc($result);
            echo "<tr>";
            echo "<td>{$row['id']}</td>";
            echo "<td>{$row['title']}</td>";
            echo "<td>{$row['director']}</td>";
            echo "<td>{$row['actor']}</td>";
            echo "<td>{$row['duration']}</td>";
            echo "<td>{$row['type']}</td>";
            echo "<td>
							<div><a class='btn' href='javascript:del({$row['id']})'>删除</a>
							<br/>
							<hr/>
							<a class='btn' href='editmovie.php?id={$row['id']}'>修改</a></div>	
						  </td>";
            echo "</tr>";
        }

        mysql_free_result($result);
        mysql_close($link);
        ?>

    </table>
</div>

<script type="text/javascript">
    function del (id) {
        if (confirm("确定删除这条电影吗？")){
            window.location = "action-delmovie.php?id="+id;
        }
    }
</script>
</body>
</html>

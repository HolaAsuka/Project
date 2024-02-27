<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>电影资料系统</title>
</head>
<style type="text/css">
.wrapper {width: 1000px;margin: 20px auto;}
h1 {text-align: center;}
.topbtn {margin-bottom: 20px;text-align: center;}
.topbtn a {text-decoration: none;color: #fff;background-color: green;padding: 6px;border-radius: 5px;}
.btn{text-decoration: none;color: #fff;background-color: red}
td {text-align: center;}
</style>
<body>
	<div class="wrapper">
		<h1>电影资料系统</h1>
		<div class="topbtn">
			<a href="addmovie.html">增加电影</a>
            <a href="searchmovie.html">查询电影</a>
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
				// 1.导入配置文件
				require "dbconfig.php";

				// 2. 连接mysql
				$link = @mysql_connect(HOST,USER,PASS) or die("提示：数据库连接失败！");
				// 选择数据库
				mysql_select_db(DBNAME,$link);
				// 编码设置
				mysql_set_charset('utf8',$link);

				// 3. 从DBNAME中查询到movies数据库，返回数据库结果集,并按照addtime降序排列
				$sql = 'select * from movies order by id asc';
				// 结果集
				$result = mysql_query($sql,$link);

				// 解析结果集,$row为电影所有数据，$moviesNum为电影数目
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

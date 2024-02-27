<%@ page contentType="text/html;charset=UTF-8" language="java" isELIgnored="false" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="pragma" content="no-cache" />
<meta http-equiv="cache-control" content="no-cache" />
<meta http-equiv="Expires" content="0" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Cookie</title>
	<base href="http://localhost:8080/book/">
<style type="text/css">

	ul li {
		list-style: none;
	}
	
</style>
</head>
<body>
	<iframe name="target" width="500" height="500" style="float: left;"></iframe>
	<div style="float: left;">
		<ul>
			<li><a href="cookieServlet?action=createCookie" target="target">Cookie的创建</a></li>
			<li><a href="cookieServlet?action=getCookie" target="target">Cookie的获取</a></li>
			<li><a href="cookieServlet?action=updateCookie" target="target">Cookie值的修改</a></li>
			<li>Cookie的存活周期</li>
			<li>
				<ul>
					<li><a href="cookieServlet?action=defaultLife" target="target">Cookie的默认存活时间（会话）</a></li>
					<li><a href="cookieServlet?action=deleteNow" target="target">Cookie立即删除</a></li>
					<li><a href="cookieServlet?action=life3600" target="target">Cookie存活3600秒（1小时）</a></li>
				</ul>
			</li>
			<li><a href="cookieServlet?action=testPath" target="target">Cookie的路径设置</a></li>
			<li><a href="" target="target">Cookie的用户免登录练习</a></li>
		</ul>
	</div>
</body>
</html>
<?php
  
// 1. 데이터베이스에서 데이터를 가져옴
$link = mysqli_connect("localhost", "root", "root","smartfactory");
  
// 3, 4 최종 결과 데이터를 JSON 스트링으로 변환 후 출력
$sql = 'DELETE FROM `sf` WHERE 1';
mysqli_query($link,$sql);

echo "<script>history.back();</script>"
?>

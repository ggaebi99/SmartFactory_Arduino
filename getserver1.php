<?php
header("Content-Type:application/json");
  
// 1. 데이터베이스에서 데이터를 가져옴
$link = mysqli_connect("localhost", "root", "root","smartfactory");
 
// 3, 4 최종 결과 데이터를 JSON 스트링으로 변환 후 출력
if ($result = mysqli_query($link, 'SELECT count(*) FROM `sf` where color="blue"')) {
    // 2. 데이터베이스로부터 반환된 데이터를
    // 객체 형태로 가공함
    $o = array();
    while ($row = mysqli_fetch_array($result)) {
        $o[] = $row[0];
        unset($row[0]);
    }
} 
else {
    $o = array( 0 => 'empty');
}

echo json_encode($o);

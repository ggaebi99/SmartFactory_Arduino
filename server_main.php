<!DOCTYPE html>
<html>

<head>
    <style>
        h1 {
            text-align: center;
            font-size: 100px;
        }

        td {
            text-align: center;
            font-size: 30px;
        }

        test {
            background-color: aqua;
        }

        ul {
            border-left: 1px solid #c0c0c0;
            /* 테두리와 메뉴 간격 벌리기, padding: 위 오른쪽 아래 왼쪽; */
            padding: 0px 10px 0px 10px;
            /* 메뉴와 테두리 사이 간격 벌리기, margin: 위 오른쪽 아래 왼쪽; */
            margin: 5px 0px 5px 0px;
        }

        ul li {
            font-size: 30px;
            display: inline;
            border-left: 1px solid #c0c0c0;
        }

        li:first-child {
            border-left: none;
        }

        input {
            font-size: 30px;
        }

    </style>
    <script src="https://code.jquery.com/jquery-1.9.1.min.js"></script>
    <meta charset="UTF-8">
    <title>SmartFactory</title>
</head>

<body>
    <h1>SmartFactory</h1>


    <script>
        $(document).ready(function() {
            updateData1();
            updateData2();
            updateData3();

        });

        function updateData1() {
            $.ajax({
                type: 'post',
                dataType: 'json',
                url: 'getserver.php',
                success: function(data) {
                    console.log(data);
                    $('#red').text(data[0]);

                },
                error: function(request, status, error) {
                    console.log('code: ' + request.status + "\n" + 'message: ' + request.responseText + "\n" + 'error: ' + error);
                }
            });
            timerID = setTimeout("updateData1()", 300); // 2초 단위로 갱신 처리
        }

        function updateData2() {
            $.ajax({
                type: 'post',
                dataType: 'json',
                url: 'getserver1.php',
                success: function(data) {
                    console.log(data);
                    $('#blue').text(data[0]);

                },
                error: function(request, status, error) {
                    console.log('code: ' + request.status + "\n" + 'message: ' + request.responseText + "\n" + 'error: ' + error);
                }
            });
            timerID = setTimeout("updateData2()", 300); // 2초 단위로 갱신 처리
        }

        function updateData3() {
            $.ajax({
                type: 'post',
                dataType: 'json',
                url: 'getserver2.php',
                success: function(data) {
                    console.log(data);
                    $('#green').text(data[0]);

                },
                error: function(request, status, error) {
                    console.log('code: ' + request.status + "\n" + 'message: ' + request.responseText + "\n" + 'error: ' + error);
                }
            });
            timerID = setTimeout("updateData3()", 300); // 2초 단위로 갱신 처리
        }

    </script>

    <table border="1" bordercolor="blue" width="500" height="300" align="center">

        <tr>
            <td colspan=3>
                오늘 생산량
            </td>
        </tr>
        <tr>
            <td>
                빨강
            </td>
            <td>
                파랑
            </td>
            <td>
                초록
            </td>
        </tr>

        <tr>
            <td>
                <p><span id="red"></span></p>
            </td>
            <td>
                <p><span id="blue"></span></p>
            </td>
            <td>
                <p><span id="green"></span></p>
            </td>

        </tr>

    </table>
    <center>
       <form action="getserver3.php">
        <input type="submit" value="리셋">
        </form>
    </center>


</body>

</html>

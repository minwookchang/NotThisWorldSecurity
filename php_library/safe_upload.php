<?php 

//file : $_FILES 객체
//path : 저장 경로
//max_size : 파일 최대 크기

function safe_upload($file, $path, $max_size = 2147483647){
	//path validation
	//if()

	if($max_size < $file['size']) return "업로드 파일이 지정된 파일크기보다 큽니다.\n";

	if(($file['error'] > 0) || ($file['size'] <= 0)) return "파일 업로드에 실패하였습니다.";

	// HTTP post로 전송된 것인지 체크합니다.
    if(!is_uploaded_file($file['tmp_name'])) return "HTTP로 전송된 파일이 아닙니다.";

    //확장자 체크


	//경로 지정
	$uploadfile = $path . basename($file['name']);

    // move_uploaded_file은 임시 저장되어 있는 파일을 ./uploads 디렉토리로 이동합니다.
    if(move_uploaded_file($file['tmp_name'], $uploadfile)) {
        return "성공적으로 업로드 되었습니다.\n";
    } else {
        echo "파일 업로드 실패입니다.\n";
    }

}
/*
echo '<pre>'; 
if($_POST['MAX_FILE_SIZE'] < $_FILES['userfile']['size']){
   echo "업로드 파일이 지정된 파일크기보다 큽니다.\n"; 
} else {
   if(($_FILES['userfile']['error'] > 0) || ($_FILES['userfile']['size'] <= 0)){
      echo "파일 업로드에 실패하였습니다.";
   } else {
          // HTTP post로 전송된 것인지 체크합니다.
          if(!is_uploaded_file($_FILES['userfile']['tmp_name'])) {
                echo "HTTP로 전송된 파일이 아닙니다.";
          } else {
                // move_uploaded_file은 임시 저장되어 있는 파일을 ./uploads 디렉토리로 이동합니다.
                if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
                     echo "성공적으로 업로드 되었습니다.\n";
                } else {
                     echo "파일 업로드 실패입니다.\n";
                }
          }
     }
 }
 print_r($_FILES);


 
 결과: 
 성공적으로 업로드 되었습니다. 
 Array 
 ( 
    [userfile] => Array 
        ( 
            [name] => config.sys 
            [type] => text/plain 
            [tmp_name] => /tmp/phpXTtzBW 
            [error] => 0 
            [size] => 10 
        ) 
 ) 
 */ 
 ?>

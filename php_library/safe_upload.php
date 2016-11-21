<?php 
//파일명 중복시 파일명 뒤에 (n) 추가
//file : $_FILES 객체
//path : 저장 경로
//max_size : 파일 최대 크기

//성공시 파일 경로 반환
//오류
//-1 : 오류
//-2 : 존재하지 않는 경로
//-3 : 업로드 파일이 지정된 파일크기보다 큼
//-4 : 오류
//-5 : HTTP로 전송된 파일이 아님
//-6 : php 또는 html 파일

function safe_upload($file, $path, $max_size = 2147483647){
  //path validation
  if(!is_dir($path)) return -2;
  
  //경로 끝에 /가 없으면 추가해줌
  if($path[strlen($path)-1] !== "/") $path.="/";

  if($max_size < $file['size']) return -3;

  if(($file['error'] > 0) || ($file['size'] <= 0)) return -4;

  // HTTP post로 전송된 것인지 체크합니다.
  if(!is_uploaded_file($file['tmp_name'])) return -5;

  //확장자 체크
  $file_name = basename($file['name']);
  if(preg_match("/\.(php|html)$/i", $file_name)) return -6;

  //경로 지정
  $uploadfile = $path . $file_name;

  //파일 이름 재설정
  $file_counter = 1;
  $path_parts = pathinfo($uploadfile);

  while(file_exists($uploadfile)){
    $uploadfile =  $path . $path_parts['filename'] . " (" . $file_counter++ . ")." . $path_parts['extension'];
  }


  // move_uploaded_file은 임시 저장되어 있는 파일을 ./uploads 디렉토리로 이동합니다.
  if(move_uploaded_file($file['tmp_name'], $uploadfile)) {
    return realpath($uploadfile);
  }
  else {
    return -1;
  }

}
 ?>
